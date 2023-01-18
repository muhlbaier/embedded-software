/*
 * subsys.c
 *
 *  Created on: Mar 12, 2014
 *      Author: Michael M
 */
#include "system.h"
#include "macros.h"
#include <string.h>
#include "uart.h"

//#include "strings.h"

#ifndef SUBSYS_UART
#error "You must define what UART number the terminal is using in system.h: #define SUBSYS_UART num"
#endif

#ifndef _SUBSYS_H_
#error "Please include subsys.h in system.h"
#endif

//if(sys.echo) UART_WriteByte(0, c);
#ifndef MAX_SUBSYSTEMS
#define MAX_SUBSYSTEMS 10
#endif

#define NUM_ADDRESS

void ReceiveChar(char c);
void ProcessMsg(void);
void LogLevels(void);
void LogVersions(void);
void LogSubsystem(int16_t s);
static void SystemCallback(int argc, char *argv[]);

/** Subsystem struct to organize name, level, and version for each subsystem.
 *
 * Note: this struct is kept local to this file since the user should use the
 * following methods to set and update the subsystem information:
 * @see SubsystemLogPriority()
 * @see SubsystemInit()
 *
 */
typedef struct {
    char * name; /**< pointer to name of subsystem/module*/
    uint8_t log; /**< log setting of subsystem/module*/
    version_t version; /**< software version of module*/
    void (*callback)(int argc, char *argv[]); ///< callback function to process commands
} subsystem_t;

/// system module structure
typedef struct {
    int last_subsys; /**< index of the last subsytem in the array */
    struct {
        uint8_t echo:1;
        uint8_t mute:1;
        uint8_t :6;
    } settings; /**< binary settings for subsystem management*/
    int id; /**< subsystem ID of the subsystem management module*/
    subsystem_t subsystem[MAX_SUBSYSTEMS]; /**< subsystem array */
} system_t;
// array of subsystem information
system_t sys;

char *GetSubsystemName(uint8_t subsystem_id) {
    return (sys.subsystem[subsystem_id].name == 0) ? "uninitialized" : sys.subsystem[subsystem_id].name;
}

static void Subsystem_SystemInit(void) {
    version_t v;  
  	static uint8_t init_flag = 0;
    if (init_flag) return;
		
    init_flag = 1;
    sys.last_subsys = -1;
    sys.settings.echo = 0;
    sys.settings.mute = 0;
    // Register the module with the system and give it the name "SYS"
	v.word = SYSTEM_VERSION;
    sys.id = Subsystem_Init("SYS", v, SystemCallback);
    // Register a receiver for the subsystem module
#ifndef SUBSYS_DISABLE_RECEIVER
    UART_RegisterReceiver(SUBSYS_UART, (receiver_callback_t)ReceiveChar);
#endif
}

void Subsystem_SystemDisable(void) {
    UART_UnregisterReceiver(SUBSYS_UART, (receiver_callback_t)ReceiveChar);
}

void Subsystem_SystemEnable(void) {
    UART_RegisterReceiver(SUBSYS_UART, (receiver_callback_t)ReceiveChar);
}

uint8_t Subsystem_Init(char * name, version_t version, void (*callback)(int argc, char *argv[])) {
    Subsystem_SystemInit();
    sys.last_subsys++;
    if (sys.last_subsys < 0 || sys.last_subsys >= MAX_SUBSYSTEMS) return 0xFF;
    sys.subsystem[sys.last_subsys].log = 1;
    sys.subsystem[sys.last_subsys].name = name;
    sys.subsystem[sys.last_subsys].version.word = version.word;
    sys.subsystem[sys.last_subsys].callback = callback;
    return sys.last_subsys;
}

void Subsystem_RegisterCallback(uint8_t subsystem_id, void (*callback)(int argc, char *argv[])) {
    sys.subsystem[subsystem_id].callback = callback;
}

enum com_state {
    IDLE, WRITING
};

char receive_buffer[RECEIVE_MAX_LENGTH];

void ReceiveChar(char c) {
    //static char receive_buffer[LOG_RECEIVE_MAX_LENGTH];
    static enum com_state receive_state = IDLE;
    static int length;
    if(sys.settings.echo) {
        UART_WriteByte(SUBSYS_UART, c);
        if(c == RECEIVE_STOP_CHAR) UART_WriteByte(SUBSYS_UART, '\n');
    }
    if (receive_state == IDLE) {
        if (c == RECEIVE_START_CHAR) {
            receive_state = WRITING;
            length = 0;
        }
    } else {
        receive_buffer[length] = c;
        if (c == RECEIVE_STOP_CHAR) {
            // process received message
            ProcessMsg();
            receive_state = IDLE;
        } else {
            if (c == ASCII_BACKSPACE || c == ASCII_DEL) {
                if (length) length--;
                else receive_state = IDLE;
            } else {
                length++;
                if (length >= RECEIVE_MAX_LENGTH) receive_state = IDLE;
            }
        }
    }
}

void ProcessMsg(void) {
    volatile int16_t i = 0;
    char *name;
    char *argv[RECEIVE_MAX_ARGC];
    int argc = 0;
    // get the name
    name = &receive_buffer[0];

    while (receive_buffer[i] != RECEIVE_STOP_CHAR) {
        // if the index is greater than length then break;
        if (i >= RECEIVE_MAX_LENGTH) break;
        if (receive_buffer[i] == ' ') {
            receive_buffer[i] = 0;
            break;
        }
        i++;
    }
    // receive the arguments
    argv[argc] = &receive_buffer[++i];
    while (receive_buffer[i] != RECEIVE_STOP_CHAR) {
        // if the index is greater than length then break;
        if (i >= RECEIVE_MAX_LENGTH) break;
        i++;
        if (receive_buffer[i] == ' '  || receive_buffer[i] == RECEIVE_STOP_CHAR) {
            argc++;
            if(receive_buffer[i] == RECEIVE_STOP_CHAR) {
                receive_buffer[i] = 0;
                break;
            }else {
                receive_buffer[i] = 0;
                argv[argc] = &receive_buffer[++i];
            }
        }
    }

    for (i = 0; i <= sys.last_subsys; i++) {
        if (strcasecmp(sys.subsystem[i].name, name) == 0) {
            if (sys.subsystem[i].callback) {
                sys.subsystem[i].callback(argc, argv);
            }
        }
    }
}

void LogVersion(uint8_t subsys_id) {
    char * ptr;
    version_mmb_t * v;
	ptr = sys.subsystem[subsys_id].name;
	if (ptr) {
		v = &sys.subsystem[subsys_id].version.v;
		LogStr("%s %u.%u.%u\r\n", ptr, v->major, v->minor, v->build);
    }
}

#define LogChar(c) UART_WriteByte(SUBSYS_UART,c)

void LogHeader(uint8_t subsystem_id);

void LogStr(char * str, ...) {
    // variable argument list type
    va_list vars;
    // initialize the variable argument list pointer by specifying the
    // input argument immediately preceding the variable list
    va_start(vars, str);
    // use Push_vprintf to log to LOG_BUF
    UART_vprintf(SUBSYS_UART, str, vars);
    va_end(vars);
}

void LogMsg(uint8_t subsystem_id, char * str, ...) {
    // variable argument list type
    va_list vars;
    if(subsystem_id > sys.last_subsys) return;
    if (sys.subsystem[subsystem_id].log && sys.settings.mute == 0) {
        // output header
        Log_Header(subsystem_id);
        // initialize the variable argument list pointer by specifying the
        // input argument immediately preceding the variable list
        va_start(vars, str);
        // use Push_vprintf to log to LOG_BUF
        UART_vprintf(SUBSYS_UART, str, vars);
        va_end(vars);
        // new line
        LogChar('\r');
        LogChar('\n');
    }
}

void Log_Header(uint8_t subsystem_id) {
    // format header as "<ms timestamp>_<subsystem name> "
    UART_Printf(SUBSYS_UART, "%l_%s: ",TimeNow(), GetSubsystemName(subsystem_id));
}

static void SystemCallback(int argc, char *argv[]) {
    volatile uint8_t i;
    if(argc) {
        if(strcasecmp(argv[0],"list") == 0) {
            for(i = 0; i < MAX_SUBSYSTEMS; i++) {
                if(sys.subsystem[i].name) LogStr("%d: %s\tversion: %08x\r\n", i, sys.subsystem[i].name, sys.subsystem[i].version);
            }
        }else if(strcasecmp(argv[0],"mute") == 0) {
            LogMsg(sys.id, "muted");
            sys.settings.mute = 1;
        }else if(strcasecmp(argv[0],"unmute") == 0) {
            sys.settings.mute = 0;
            if(argc >= 2 && strcasecmp(argv[1], "all") == 0) {
                for(i = 0; i <= sys.last_subsys; i++) sys.subsystem[i].log = 1;
                LogMsg(sys.id, "unmuted all");
            }else LogMsg(sys.id, "unmuted");
        }else if(strcasecmp(argv[0],"echo") == 0) {
            if(argc >= 2 && strcasecmp(argv[1],"off") == 0) {
                sys.settings.echo = 0;
                LogMsg(sys.id, "echo off");
            }else {
                sys.settings.echo = 1;
                LogMsg(sys.id, "echo on");
            }
        }else{
            //check if the command is actually a subsystem name
            for(i = 0; i <= sys.last_subsys; i++) {
                if(strcasecmp(argv[0], sys.subsystem[i].name) == 0) {
                    if(argc < 2) LogMsg(sys.id, "to few args");
                    else {
                        if(strcasecmp(argv[1],"mute") == 0) {
                            LogMsg(sys.id, "muted");
                            sys.subsystem[i].log = 0;
                        }else if(strcasecmp(argv[1],"unmute") == 0) {
                            sys.subsystem[i].log = 1;
                            LogMsg(sys.id, "unmuted");
                        }else LogMsg(sys.id, "cmd not supported");
                    }
                    return;
                }
            }
            LogMsg(sys.id, "cmd not supported");
        }
    }
}

uint32_t ArgToU32(char* arg){
	uint32_t result;
	uint32_t i;
	result = 0;
	for(i = 0; i < strlen(arg); i++){
		result = result * 10 + ( arg[i] - '0' );
	}
	return result;
}

uint16_t ArgToU16(char* arg){
	uint16_t result;
	uint8_t i;
	result = 0;
	for(i = 0; i < strlen(arg); i++){
		result = result * 10 + ( arg[i] - '0' );
	}
	return result;
}

uint8_t ArgToU8(char* arg){
	uint8_t result;
	uint8_t i;
	result = 0;
	for(i = 0; i < strlen(arg); i++){
		result = result * 10 + ( arg[i] - '0' );
	}
	return result;
}

void Log_EchoOn(void) {
    sys.settings.echo = 1;
}

void Log_EchoOff(void) {
    sys.settings.echo = 0;
}

uint8_t Log_GetEcho(void) {
    if(sys.settings.echo) return 1;
    return 0;
}

void MuteAll(void) {
	sys.settings.mute = 1;
}

void UnmuteAll(void) {
	sys.settings.mute = 0;
}

void MuteSys(uint8_t sys_id) {
	if(sys_id <= sys.last_subsys) sys.subsystem[sys_id].log = 0;
}

void UnmuteSys(uint8_t sys_id) {
	if(sys_id <= sys.last_subsys) sys.subsystem[sys_id].log = 1;
}
