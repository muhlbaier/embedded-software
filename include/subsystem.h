/**
 * @file subsystem.h
 *
 * @defgroup subsystem Subsystem Module
 *
 * This module is intended to assist in logging and or management of modules
 * via a command line. The user should define one of the following IO methods
 * in project_settings.h:
 * 
 * - #define SUBSYSTEM_IO SUBSYSTEM_IO_NONE dummy IO methods. Useful to disable
 * logging without removing all the log function calls
 * - #define SUBSYSTEM_IO SUBSYSTEM_IO_UART interface via the UART Module. Note: must
 * also #define SUBSYSTEM_UART X where X is the UART channel to use for this module.
 * - #define SUBSYSTEM_IO SUBSYSTEM_IO_PRINTF use standard c printf to log
 * - #define SUBSYSTEM_IO SUBSYSTEM_IO_CUSTOM allow the user to define the IO functions:
 * LogError(), LogWarning(), LogMsg(), LogDebug()
 * 
 *  Created on: Mar 12, 2014
 *      @author: Michael Muhlbaier
 *
 * @version 2014.03.26 changed SystemTick to SubsystemTick so the Task Management Module can own SystemTick()
 * @version 2.0.1 added version, setup SYSTEM subsystem, changed warning messages to priority WARNING and set default priority to ERROR to suppress messages
 * @version 3.1.1 removed log levels and redueced to mute or unmute. Moved
 *     receiver functionality to UART module. Changed callback to take argc
 *     argv inputs and added parsing to create argc and argv.
 * @version 3.1.2 fixed "$sys unmute all" command, removed unused code, added documentation
 * @version 3.1.3 changed name of SystemInit to avoid conflicts and changed how version is passed so it doesn't throw a warning on ARM compilers
 * @version 4.0.0 changed name of module from subsys to subsystem
 * @{
 */
#ifndef _SUBSYSTEM_H_
#define _SUBSYSTEM_H_

#include <stdarg.h>

#include "library.h"

// Define these before including project_settings.h so project_settings.h can see them
#define SUBSYSTEM_IO_NONE 0
#define SUBSYSTEM_IO_UART 1
#define SUBSYSTEM_IO_PRINTF 2
#define SUBSYSTEM_IO_CUSTOM 255 // if the user sets this, they are responsible for creating the macro hooks to the IO layer in their project_settings.h file

#include "project_settings.h"

enum log_level {
    LOG_OFF = 0,
    LOG_ERROR,
    LOG_WARNING,
    LOG_MESSAGE,
    LOG_DEBUG
};


/* If the subsys io isn't explicitly defined, but SUBSYS_UART is defined, set the io system to UART
 * else, disable the system */
#ifndef SUBSYSTEM_IO
#ifdef SUBSYSTEM_UART
#define SUBSYSTEM_IO SUBSYSTEM_IO_UART
#else
#define SUBSYSTEM_IO SUBSYSTEM_IO_NONE
#endif
#endif

#if SUBSYSTEM_IO != SUBSYSTEM_IO_NONE
#ifndef USE_MODULE_SUBSYSTEM
#warning "USE_MODULE_SUBSYSTEM not defined in project_settings.h. Other modules won't be able to utilize this module."
#endif
#endif

#if SUBSYSTEM_IO == SUBSYSTEM_IO_NONE
    #define LogError(sys_id, str, ...)
    #define LogWarning(sys_id, str, ...)
    #define LogMsg(sys_id, str, ...)
    #define LogDebug(sys_id, str, ...)
#else
    #define LogError(sys_id, str, ...) Subsystem_Write(sys_id, LOG_ERROR, str, ##__VA_ARGS__)
    #define LogWarning(sys_id, str, ...) Subsystem_Write(sys_id, LOG_WARNING, str, ##__VA_ARGS__)
    #define LogMsg(sys_id, str, ...) Subsystem_Write(sys_id, LOG_MESSAGE, str, ##__VA_ARGS__)
    #define LogDebug(sys_id, str, ...) Subsystem_Write(sys_id, LOG_DEBUG, str, ##__VA_ARGS__)
#endif

#if SUBSYSTEM_IO == SUBSYSTEM_IO_NONE
    #define Subsystem_printf(str, ...)
    #define Subsystem_vprintf(str, arg)
    #define Subsystem_PrintChar(c)
    #define Subsystem_RegisterReceiver(r)
#elif SUBSYSTEM_IO == SUBSYSTEM_IO_UART
    #include "uart.h"
    #ifndef USE_MODULE_BUFFER_PRINTF
        #error "This module requires optional UART functionality provided by buffer_printf. Please declare USE_MODULE_BUFFER_PRINTF in project_settings.h"
    #endif
    #ifndef SUBSYSTEM_UART
        #error "You must define what UART number the terminal is using in project_settings.h: #define SUBSYS_UART num"
    #endif
    #define Subsystem_printf(str, ...) UART_printf(SUBSYSTEM_UART, str, ##__VA_ARGS__)
    #define Subsystem_vprintf(str, arg) UART_vprintf(SUBSYSTEM_UART, str, arg)
    #define Subsystem_PrintChar(c) UART_WriteByte(SUBSYSTEM_UART, c)
    #define Subsystem_RegisterReceiver(r) UART_RegisterReceiver(SUBSYSTEM_UART, r);
#elif SUBSYSTEM_IO == SUBSYSTEM_IO_PRINTF
    #include <stdio.h>
    #define Subsystem_printf(str, ...) printf(str, ##__VA_ARGS__)
    #define Subsystem_vprintf(str, args) vprintf(str, args)
    #define Subsystem_PrintChar(c) printf("%c", c)
    #define Subsystem_RegisterReceiver(r)
#endif

/////////////////////////MMmmbbbb
#define SYSTEM_VERSION 0x04000000u ///< subsystem module version number

#ifndef RECEIVE_MAX_LENGTH
#define RECEIVE_MAX_LENGTH 64 ///< default length of receive line for commands
#endif
#define RECEIVE_START_CHAR '$' ///< start character for command lines
#define RECEIVE_STOP_CHAR '\r' ///< stop character for command lines
#ifndef RECEIVE_MAX_ARGC
#define RECEIVE_MAX_ARGC 8 ///< max number of arguments after the command name
#endif

/** @page sys_receive SubSystem module command interface
 *
 * If bi-directional communication is available (e.g. UART) then the
 * user can interface with the SubSystem module in real time using the
 * module's command interface.
 *
 * A command can be sent to the module in the following format:
 * @code
 * $name arg1 arg2 arg3 arg4
 * @endcode
 * Where the $ indicates the start of the command, name is the name of the
 * subsystem the command is sent to and it is followed by argument strings
 *
 * The Subsytem Module registers itself as "SYS" and the commands available are:
 *
 * - "$sys list" list the handles and names of all present subsystems
 * - "$sys mute" global mute of log messages
 * - "$sys unmute" global unmute of log messages
 * - "$sys unmute all" global unmute of log messages plus enable logging of every subsystem
 * - "$sys echo" echo back received characters
 * - "$sys echo off" disable echoing
 * - "$sys name mute" turn off logging for the subsystem called "name"
 * - "$sys name unmute" turn on logging for the subsystem called "name"
 *
 * If your module wishes to receive commands from the command line simply
 * register with the subsystem module and the arguments following a command
 * that starts with the name of your module will be passed to your callback.
 *
 * @code
 * static uint8_t sys_id; // handle for the subsystem
 * void MyCallback(int argc, char *argv[]) {
 *  // do something based on the arguments
 * }
 * ...
 * sys_id = Subsystem_Init("example", (version_t)0x01020003u, MyCallback);
 * // now you can use the handle to log messages with a header and to enable
 * // your messages to be muted or unmuted
 * LogMsG(sys_id, "printf style formatted %s", "message");
 * // MyCallback will be called after any command that starts with "$example "
 * @endcode
 */

/** @page subsys_init Subsystem module initialization
 *
 * Below are three example of how a subsystem could initialize itself.
 *
 * @code
 * // define the version
 * #define TASK_VERSION (version_t)0x01010014u
 * static uint8_t sys_id; // global to the file only
 * // initialize module use "task" to refer to this subsystem in output messages
 * // and don't register a callback for receiving commands
 * sys_id = Subsystem_Init("task", TASK_VERSION, 0);
 *
 * // or another way to do the same thing
 * static uint8_t sys_id; // global to the file only
 * #define TASK_VERSION_MAJOR 1
 * #define TASK_VERSION_MINOR 1
 * #define TASK_VERSION_BUILD 20
 *
 * version_t task_version;
 * task_version.major = TASK_VERSION_MAJOR;
 * task_version.minor = TASK_VERSION_MINOR;
 * task_version.build = TASK_VERSION_BUILD;
 *
 * task_id = Subsystem_Init("task", task_version, 0);
 *
 * static uint8_t sys_id; // global to the file only
 * // or to do it all in one line
 * sys_id = Subsystem_Init("task", (version_t)0x01010014u, 0);
 * LogMsg(sys_id, "Crap hit the fan");
 * @endcode
 *
 */


/** GetLogTimestamp must be defined so that it returns a integer (up to 32 bits)
 * timestamp
 */
#define GetLogTimestamp() TimeNow()

// use Push_printf to log messages to the log buffer LOG_BUF
/** Logs the null terminated string at the pointer (str)
 *
 * Same as LogMsg() without the header in the beginning and without the CRLF at
 * the end.
 *
 * This function is implemented using Push_vprintf. See Push_printf() for supported
 * flags/features.
 *
 * Will log the string to the buffer defined by LOG_BUF (typically tx0)
 *
 * @param str pointer to string to log
 * @param ... variable number of replacement parameters for the str string
 *
 * Example usage:
 * @code
 *   LogStr("oops I crapped my pants %d times", num_craps);
 * @endcode
 */
void LogStr(char * str, ...);

/** Logs the message at the pointer (str) with a timestamp and subsystem name
 *
 * Before logging the message the function will check the current log setting of
 * the subsystem and to determine if the message should be logged
 *
 * This function is implemented using Push_vprintf. See Push_printf() for supported
 * flags/features.
 *
 * Will log the string to the buffer defined by SUSSYS_UART
 *
 * @param subsystem_id subsystem id
 * @param str pointer to message to log
 * @param ... variable number of replacement parameters for the str string
 *
 * Example usage:
 * @code
 *   LogMsg(sys.id, "oops I crapped my pants");
 *   LogMsg(sys.id, "System Index %d, System Name %s.", sys.id, GetSubsystemName(SYSTEM));
 * @endcode
 */
// TODO: Update documentation
void Subsystem_Write(uint8_t subsystem_id, enum log_level level,  char * str, ...);

/** Initialize settings for a subsystem - critical for proper logging and command interface
 *
 * If a module/subsystem uses logging it should call this function
 * with the appropriate inputs when the subsystem is initializing.
 *
 * @return subsystem index
 * @param name pointer to name of the subsystem (recommended to make the name
 *        8 characters or less)
 * @param version software version of subsystem, see #version_t for more info
 * @param callback callback function to be called when the user inputs a command
 * in the form of "$name var1 var2 var3...". Where name is the name passed to
 * this function. The callback will be passed the number of arguments, @c argc ,
 * and a array of pointers to the argument strings, @c argv.
 */
uint8_t Subsystem_Init(char * name, version_t version, void (*callback)(int argc, char *argv[]));

/** Register a callback function for a subsystem
 *
 * When a command is received by the logging module for the subsystem @c sys
 * the @c callback function will be executed and passed the number of
 * arguments @c argc and a array (vector) of pointer to the argument strungs
 * @c argv.
 *
 * The callback is set by Subsystem_Init(), this function can be used to update
 * the callback.
 *
 * @param subsystem_id - subsystem to register the callback for
 * @param callback - function pointer to the function to run when a command is
 * received for the subsystem.
 */
void Subsystem_RegisterCallback(uint8_t subsystem_id, void (*callback)(int argc, char *argv[]));

/** Return a pointer to a string corresponding to the name of the subsystem
 *
 * The name returned is the one set by Subsystem_Init()
 *
 * @param subsystem_id
 * @return - pointer to a null terminated string corresponding to the name of the subsystem
 */
char *Subsystem_GetName(uint8_t subsystem_id);

/** Turn echo featuren on (default is off) */
void Log_EchoOn(void);

/** Turn echo feature off */
void Log_EchoOff(void);

/** Get status of echo setting
 *
 * @return echo setting, 1 if echo is on
 */
uint8_t Log_GetEcho(void);

void Log_Version(uint8_t subys_id);

void Log_Header(uint8_t subsystem_id, enum log_level level);

/**
 * Mute all log messages
 */
void Log_MuteAll(void);

/**
 * Global unmute
 */
void Log_UnmuteAll(void);

/**
 * Mute messages from certain sys_id
 */
void Log_MuteSys(uint8_t sys_id);

/**
 * Unmute messages from certain sys_id
 */
void Log_UnmuteSys(uint8_t sys_id);

void Log_SetLevel(uint8_t sys_id, enum log_level level);

uint16_t ArgToU16(char* arg);
uint8_t ArgToU8(char* arg);
uint32_t ArgToU32(char* arg);

/** Used to disable the Subsystem Module's UART receiver so data can be transmitted
 * without being intercepted by the Subsystem Module
 */
void Subsystem_SystemDisable(void);

/** Used to re-register the Subsystem Module's UART after it has been disabled
 * by Subsystem_SystemDisable().
 * 
 * Only call this function after Subsystem_SystemDisable()
 */
void Subsystem_SystemEnable(void);

/** @}*/
#endif /* _SUBSYS_H_ */
