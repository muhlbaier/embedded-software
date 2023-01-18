/**
 * @file subsys.h
 *
 * @defgroup subsys Subsystem Module
 *
 *  Created on: Mar 12, 2014
 *      @author: Michael
 *
 * @version 2014.03.26 changed SystemTick to SubsystemTick so the Task Management Module can own SystemTick()
 * @version 2.0.1 added version, setup SYSTEM subsystem, changed warning messages to priority WARNING and set default priority to ERROR to suppress messages
 * @version 3.1.1 removed log levels and redueced to mute or unmute. Moved
 *     receiver functionality to UART module. Changed callback to take argc
 *     argv inputs and added parsing to create argc and argv.
 * @version 3.1.2 fixed "$sys unmute all" command, removed unused code, added documentation
 * @version 3.1.3 changed name of SystemInit to avoid conflicts and changed how version is passed so it doesn't throw a warning on ARM compilers
 * @version 3.1.4 added version to "$sys list" printout
 * @{
 */
#ifndef _SUBSYS_H_
#define _SUBSYS_H_

/////////////////////////MMmmbbbb
#define SYSTEM_VERSION 0x03010004u ///< subsystem module version number

#ifndef RECEIVE_MAX_LENGTH
#define RECEIVE_MAX_LENGTH 64 ///< default length of receive line for commands
#endif
#define RECEIVE_START_CHAR '$' ///< start character for command lines
//#define RECEIVE_STOP_CHAR '\r' ///< stop character for command lines
#ifndef RECEIVE_MAX_ARGC
#define RECEIVE_MAX_ARGC 8 ///< max number of arguments after the command name
#endif

/** Version typedef to store software version of subsystem
 *
 * The version is split into three numbers:
 * major.minor.build
 * where major and minor are 0-255 and build is 0-65535
 * build should be incremented as frequently as possible (automatically on build
 * if possible with the compiler)
 */
typedef struct {
        uint16_t build; /**< build number*/
        uint8_t minor; /**< minor version*/
        uint8_t major; /**< major version*/
} version_mmb_t;
/// version union
typedef union {
    uint32_t word; /**< 32 bit version */
    version_mmb_t v; /**< major.minor.build struct*/
} version_t;

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
void LogMsg(uint8_t subsystem_id, char * str, ...);

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
 * The name returned is the one set by SubsystemInit()
 *
 * @param subsystem_id
 * @return - pointer to a null terminated string corresponding to the name of the subsystem
 */
char *GetSubsystemName(uint8_t subsystem_id);

/** Turn echo featuren on (default is off) */
void Log_EchoOn(void);

/** Turn echo feature off */
void Log_EchoOff(void);

/** Get status of echo setting
 * 
 * @return echo setting, 1 if echo is on
 */
uint8_t Log_GetEcho(void);

/**
 * Log header (timestamp and subsystem name)
 *
 * @param subsystem_id subsystem id (index)
 */
void Log_Header(uint8_t subsystem_id);


void LogVersion(uint8_t subys_id);


/**
 * Mute all log messages
 */
void MuteAll(void);

/**
 * Global unmute
 */
void UnmuteAll(void);

/**
 * Mute messages from certain sys_id
 */
void MuteSys(uint8_t sys_id);

/**
 * Unmute messages from certain sys_id
 */
void UnmuteSys(uint8_t sys_id);

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
void Subsystem_SystemDisable(void);

/** @}*/
#endif /* _SUBSYS_H_ */
