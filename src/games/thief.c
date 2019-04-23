#include "project_settings.h"
#include "nrf24network.h"

static uint8_t initialized = 0;
static uint8_t sys_id;
static void (*ArmCallback)(void);
static void (*DisarmCallback)(void);
static void (*DifficultyCallback)(uint8_t);

#ifndef THIEF_SPI
#error "please define THIEF_SPI in project_settings.h"
#endif

static void ThiefMessageHandler(uint8_t * data, uint8_t length, uint8_t from);
static void TerminalCallback(int argc, char *argv[]);

void Thief_Trip(uint8_t score) {
	if(initialized) {
		uint8_t msg[2];
		msg[0] = TRIP_MSG;
		msg[1] = score;
		nrf24_SendMsg(MASTER, THIEF_MSG, msg, 2);
		LogMsg(sys_id, "TRIP sent, score %d", score);
	}
}

void Thief_Access(uint8_t score) {
	if(initialized) {
		uint8_t msg[2];
		msg[0] = ACCESS_MSG;
		msg[1] = score;
		nrf24_SendMsg(MASTER, THIEF_MSG, msg, 2);
		LogMsg(sys_id, "ACCESS sent, score %d", score);
	}	
}

void Thief_Error(uint8_t error_code) {
	if(initialized) {
		uint8_t msg[2];
		msg[0] = ERROR_MSG;
		msg[1] = error_code;
		nrf24_SendMsg(MASTER, THIEF_MSG, msg, 2);
		LogMsg(sys_id, "ERROR sent, code %d", error_code);
	}
}

void Thief_Init(void(*ChipEnable)(uint8_t), void(*ChipSelectNot)(uint8_t), 
                void(*Arm)(void), void(*Disarm)(void), void(*Difficulty)(uint8_t))
{
	version_t version;
	version.word = 0x01010001;
	nrf24_NetworkInit(ChipEnable, ChipSelectNot, THIEF_SPI);
	nrf24_RegisterMsgHandler(THIEF_MSG, ThiefMessageHandler);
	ArmCallback = Arm;
	DisarmCallback = Disarm;
	DifficultyCallback = Difficulty;
	sys_id = Subsystem_Init("Thief", version, TerminalCallback);
	initialized = 1;
	LogMsg(sys_id, "Thief Initialized");
}

static void ThiefMessageHandler(uint8_t * data, uint8_t length, uint8_t from) {
	if(from == MASTER) {
		switch(*data) {
			case ARM_MSG:
				if(ArmCallback) {
					ArmCallback();
				}
				LogMsg(sys_id, "Armed");
				break;
			case DISARM_MSG:
				if(DisarmCallback) {
					DisarmCallback();
				}
				LogMsg(sys_id, "Disarmed");
				break;
			case DIFFICULTY_MSG:
				break;
			default:
				LogMsg(sys_id, "Unexpected msg id %d from brainframe", *data);
				return;
		}
		// increment the data pointer which will then point to the difficulty
		// if non-zero
		if(*(++data)) {
			// make sure callback exists
			if(DifficultyCallback) {
				DifficultyCallback(*data);
				LogMsg(sys_id, "Difficulty changed to %d", *data);
			}
		}
	}else {
		LogMsg(sys_id, "Node %d is impersonating the Brainframe", from);
	}
}

static void TerminalCallback(int argc, char *argv[]) {
	uint8_t value = 0;
	uint8_t msg[2];
	// if a value was received then convert the string to an int
	if(argc >= 2) {
		value = ArgToU8(argv[1]);
	}
	if(argc) {
		if(strcasecmp(argv[0], "trip") == 0) {
			Thief_Trip(value);
		}else if(strcasecmp(argv[0], "access") == 0) {
			Thief_Access(value);
		}else if(strcasecmp(argv[0], "arm") == 0) {
			msg[0] = ARM_MSG;
			msg[1] = value;
			ThiefMessageHandler(msg, 2, MASTER);
		}else if(strcasecmp(argv[0], "disarm") == 0) {
			msg[0] = DISARM_MSG;
			msg[1] = value;
			ThiefMessageHandler(msg, 2, MASTER);
		}else if(strcasecmp(argv[0], "difficulty") == 0) {
			msg[0] = DIFFICULTY_MSG;
			msg[1] = value;
			ThiefMessageHandler(msg, 2, MASTER);
		}else {
			LogMsg(sys_id, "Unknown command: %s", argv[0]);
		}
	}else {
		LogMsg(sys_id, "Missing command argument");
	}
}










