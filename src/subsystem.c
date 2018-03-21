/*
 * subsystem.c
 *
 *  Created on: Mar 12, 2014
 *      Author: Michael M
 */
#include <string.h>
#include "strings.h"
#include "project_settings.h"
#include "subsystem.h"

#include "timing.h"

#ifndef MAX_SUBSYSTEMS
#define MAX_SUBSYSTEMS 10
#endif

#if SUBSYSTEM_IO == SUBSYSTEM_IO_NONE
#warning "You have included subsystem.c in your project, but don't use it. Either set SUBSYSTEM_IO in project_settings.h or remove subsystem.c from your project"
#endif

static void Receiver(uint8_t c);
static void ProcessMsg(void);
static void SystemCallback(int argc, char *argv[]);

/** Subsystem struct to organize name, level, and version for each subsystem.
 *
 * Note: this struct is kept local to this file since the user should use the
 * following methods to set and update the subsystem information:
 * @see Subsystem_Init()
 *
 */
typedef struct {
    char * name; /**< pointer to name of subsystem/module*/
    enum log_level level; /**< log setting of subsystem/module*/
    version_t version; /**< software version of module*/
    void (*callback)(int argc, char *argv[]); ///< callback function to process commands
} subsystem_t;

/// system module structure
typedef struct {
    int last_subsys; /**< index of the last subsytem in the array */
    struct {
        uint8_t echo:1;
        uint8_t mute:1; // Global log level
        uint8_t :6;
    } settings; /**< binary settings for subsystem management*/
    int id; /**< subsystem ID of the subsystem management module*/
    subsystem_t subsystem[MAX_SUBSYSTEMS]; /**< subsystem array */
} system_t;
// array of subsystem information
system_t sys;

char *Subsystem_GetName(uint8_t subsystem_id) {
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
#ifndef SUBSYSTEM_DISABLE_RECEIVER
    Subsystem_RegisterReceiver(Receiver);
#endif
}

void Subsystem_SystemDisable(void) {
    UART_UnregisterReceiver(SUBSYSTEM_UART, Receiver);
}

void Subsystem_SystemEnable(void) {
    UART_RegisterReceiver(SUBSYSTEM_UART, Receiver);
}

uint8_t Subsystem_Init(char * name, version_t version, void (*callback)(int argc, char *argv[])) {
    // make sure subsystem module has been initialized first (does nothing if it has)
    Subsystem_SystemInit();
    sys.last_subsys++;
    if (sys.last_subsys < 0 || sys.last_subsys >= MAX_SUBSYSTEMS) {
        sys.last_subsys--;
        return 0xFF;
    }
    sys.subsystem[sys.last_subsys].level = LOG_MESSAGE;
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

static uint8_t receive_buffer[RECEIVE_MAX_LENGTH];

static void Receiver(uint8_t c) {
    //static char receive_buffer[LOG_RECEIVE_MAX_LENGTH];
    static enum com_state receive_state = IDLE;
    static int length;
    if(sys.settings.echo) {
        Subsystem_PrintChar(c);
        if(c == RECEIVE_STOP_CHAR) Subsystem_PrintChar('\n');
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

static void ProcessMsg(void) {
    volatile int16_t i = 0;
    char *name;
    char *argv[RECEIVE_MAX_ARGC];
    int argc = 0;
    // get the name
    name = (char*)&receive_buffer[0]; // convert to char* from unt8_t*

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
    argv[argc] = (char*)&receive_buffer[++i]; // convert to char* from unt8_t*
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
                argv[argc] = (char*)&receive_buffer[++i]; // convert to char* from unt8_t*
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

void LogStr(char * str, ...) {
    // variable argument list type
    va_list vars;
    // initialize the variable argument list pointer by specifying the
    // input argument immediately preceding the variable list
    va_start(vars, str);
    // use Push_vprintf to log to LOG_BUF
    Subsystem_vprintf(str, vars);
    va_end(vars);
}

void Subsystem_Write(uint8_t subsystem_id, enum log_level level, char * str, ...) {
    // variable argument list type
    va_list vars;
    if(subsystem_id > sys.last_subsys) return;

    // Check if global level allows this to pass through
    if(!sys.settings.mute) {
        // Check if subsytem specific level allows for pass through
        if (sys.subsystem[subsystem_id].level >= level) {
            // output header
            Log_Header(subsystem_id, level);
            // initialize the variable argument list pointer by specifying the
            // input argument immediately preceding the variable list
            va_start(vars, str);
            // use Push_vprintf to log to LOG_BUF
            Subsystem_vprintf(str, vars);
            va_end(vars);
            // new line
            Subsystem_PrintChar('\r');
            Subsystem_PrintChar('\n');
        }
    }
}

static const char* log_level_str[] = {"", "[ERROR] ", "[WARNING] ", "", "[DEBUG] "};

void Log_Header(uint8_t subsystem_id, enum log_level level) {
    // format header as "<ms timestamp>_<subsystem name> "
    Subsystem_printf("%lu_%s: %s",TimeNow(), Subsystem_GetName(subsystem_id), log_level_str[level]);
}

static void SystemCallback(int argc, char *argv[]) {
    volatile uint8_t i;
    if(argc) {
        if(strcasecmp(argv[0],"list") == 0) {
            for(i = 0; i < MAX_SUBSYSTEMS; i++) {
                if(sys.subsystem[i].name) LogStr("%d: %s\r\n", i, sys.subsystem[i].name);
            }
        }else if(strcasecmp(argv[0],"mute") == 0) {
            LogMsg(sys.id, "muted");
            sys.settings.mute = 1;
        }else if(strcasecmp(argv[0],"unmute") == 0) {
            sys.settings.mute = 0;
            if(argc >= 2 && strcasecmp(argv[1], "all") == 0) {
                for(i = 0; i <= sys.last_subsys; i++) sys.subsystem[i].level = LOG_MESSAGE;
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
                            sys.subsystem[i].level = LOG_OFF;
                        }else if(strcasecmp(argv[1],"unmute") == 0) {
                            sys.subsystem[i].level = LOG_MESSAGE;
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

void Log_SetLevel(uint8_t sys_id, enum log_level level) {
    if(sys_id <= sys.last_subsys) sys.subsystem[sys_id].level = level;
}

void Log_MuteAll(void) {
    sys.settings.mute = 1;
}

void Log_UnmuteAll(void) {
    sys.settings.mute = 0;
}

void Log_MuteSys(uint8_t sys_id) {
    if(sys_id <= sys.last_subsys) sys.subsystem[sys_id].level = LOG_OFF;
}

void Log_UnmuteSys(uint8_t sys_id) {
    if(sys_id <= sys.last_subsys) sys.subsystem[sys_id].level = LOG_MESSAGE;
}
