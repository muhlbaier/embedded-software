#include "project_settings.h"
#include "nrf24network.h"
#include "subsystem.h"
#include "hal_general.h"
#include "spi.h"
#include "string.h"

static uint8_t initialized = 0;
static uint8_t sys_id;
static void (*TripCallback)(uint8_t score, uint8_t address);
static void (*AccessCallback)(uint8_t score, uint8_t address);

#ifndef THIEF_SPI
#error "please define THIEF_SPI in project_settings.h"
#endif

#ifdef THIEF_BRAINFRAME_NETWORK
nrfnet_t Brainframe_Net;
#endif

static void ThiefMessageHandler(uint8_t * data, uint8_t length, uint8_t from);
static void TerminalCallback(int argc, char *argv[]);

void Thief_Arm(uint8_t difficulty, uint8_t address) {
	if(initialized) {
		uint8_t msg[2];
		msg[0] = ARM_MSG;
		msg[1] = difficulty;
#ifndef THIEF_BRAINFRAME_NETWORK
		nrf24_SendMsg(address, THIEF_MSG, msg, 2);
#else
		nrf24_SendMsgN(&Brainframe_Net, address, THIEF_MSG, msg, 2);
#endif
		LogMsg(sys_id, "ARM sent to %d, difficulty %d", address, difficulty);
	}
}

void Thief_Disarm(uint8_t difficulty, uint8_t address) {
	if(initialized) {
		uint8_t msg[2];
		msg[0] = DISARM_MSG;
		msg[1] = difficulty;
#ifndef THIEF_BRAINFRAME_NETWORK
        nrf24_SendMsg(address, THIEF_MSG, msg, 2);
#else
        nrf24_SendMsgN(&Brainframe_Net, address, THIEF_MSG, msg, 2);
#endif
		LogMsg(sys_id, "DISARM sent to %d, difficulty %d", address, difficulty);
	}
}

void Thief_Difficulty(uint8_t difficulty, uint8_t address) {
	if(initialized) {
		uint8_t msg[2];
		msg[0] = DIFFICULTY_MSG;
		msg[1] = difficulty;
#ifndef THIEF_BRAINFRAME_NETWORK
        nrf24_SendMsg(address, THIEF_MSG, msg, 2);
#else
        nrf24_SendMsgN(&Brainframe_Net, address, THIEF_MSG, msg, 2);
#endif
		LogMsg(sys_id, "DIFFICULTY sent to %d, difficulty %d", address, difficulty);
	}
}

void Thief_BrainframeInit(void(*ChipEnable)(uint8_t), void(*ChipSelectNot)(uint8_t), 
						 void(*Trip)(uint8_t, uint8_t), void(*Access)(uint8_t, uint8_t))
{
	version_t version;
	version.word = 0x01010001;
    // THIEF_BRAINFRAME_NETWORK should be defined in project_settings.h to use the alternate
    // nRF24 module instead of the default
#ifndef THIEF_BRAINFRAME_NETWORK
	nrf24_NetworkInit(ChipEnable, ChipSelectNot, THIEF_SPI);
	nrf24_RegisterMsgHandler(THIEF_MSG, ThiefMessageHandler);
#else
	nrf24_NetworkInitN(&Brainframe_Net, ChipEnable, ChipSelectNot, THIEF_SPI, MASTER);
	nrf24_RegisterMsgHandlerN(&Brainframe_Net, THIEF_MSG, ThiefMessageHandler);
#endif

	TripCallback = Trip;
	AccessCallback = Access;
	sys_id = Subsystem_Init("Brainframe", version, TerminalCallback);
	initialized = 1;
	LogMsg(sys_id, "Thief Brainframe Initialized");
}

static void ThiefMessageHandler(uint8_t * data, uint8_t length, uint8_t from) {
	switch(*data++) {
		case TRIP_MSG:
			if(TripCallback) {
				TripCallback(*data, from);
			}
			LogMsg(sys_id, "Trip detected by %s, score %d", NameFromAddress(from), *data);
			break;
		case ACCESS_MSG:
			if(AccessCallback) {
				AccessCallback(*data, from);
			}
			LogMsg(sys_id, "Access granted by %s, score %d", NameFromAddress(from), *data);
			break;
		case ERROR_MSG:
			LogMsg(sys_id, "Error message from %s, code %d", NameFromAddress(from), *data);
			break;
		default:
			LogMsg(sys_id, "Unexpected msg id %d from %s", *data, NameFromAddress(from));
			return;
	}
}

static void TerminalCallback(int argc, char *argv[]) {
	uint8_t value = 0;
	uint8_t msg[2];
	uint8_t address = ALL_ALL;
	// if a value was received then convert the string to an int
	if(argc >= 3) {
		value = ArgToU8(argv[2]);
	}
	if(argc >= 2) {
		address = AddressFromName(argv[1]);
	}
	if(argc) {
		if(strcasecmp(argv[0], "trip") == 0) {
			msg[0] = TRIP_MSG;
			msg[1] = value;
			ThiefMessageHandler(msg, 2, address);
		}else if(strcasecmp(argv[0], "access") == 0) {
			msg[0] = ACCESS_MSG;
			msg[1] = value;
			ThiefMessageHandler(msg, 2, address);
		}else if(strcasecmp(argv[0], "arm") == 0) {
			Thief_Arm(value, address);
		}else if(strcasecmp(argv[0], "disarm") == 0) {
			Thief_Disarm(value, address);
		}else if(strcasecmp(argv[0], "difficulty") == 0) {
			Thief_Difficulty(value, address);
		}else {
			LogMsg(sys_id, "Unknown command: %s", argv[0]);
		}
	}else {
		LogMsg(sys_id, "Missing command argument");
	}
}
