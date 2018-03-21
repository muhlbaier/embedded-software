#ifndef _LIBRARY_H_
#define _LIBRARY_H_
 
/** @mainpage Embedded Library
 *
 * @section getting_started Getting Started With Using the Library
 * To use this library:
 * - add the desired modules (.c files) to your project and include the
 * corresponding header files (.h files) in project_settings.h (you must create project_settings.h).
 * - add the \ include directory to the compilers include directories
 * - add the \ hal \ hal_includes directory to the compilers include directories
 * - add the \ hal \ processor_family \ processor_number directory to the
 * compilers include directories
 * - enjoy
 *
 * For example to make a simple project which says "Hello World" every 10
 * seconds you would add the following c files to your project:
 * - buffer.c (used by the UART module)
 * - buffer_printf.c (optional - used by the UART module if you use UART_printf)
 * - charReceiverList.c (used by the UART module)
 * - hal_uart.c (found in the hal \ processor_family \ processor_number folder
 * - list.c (used by the task management module)
 * - task.c
 * - timing.c (used by the task management module)
 * - uart.c
 *
 * Your project_settings.h may look something like the following:
 * @code
#ifndef _PROJECT_SETTINGS_H_
#define _PROJECT_SETTINGS_H_

// include the library header
#include "library.h"
// let the system know which lower level modules are in use
// this way higher modules can selectively utilize their resources
#define USE_MODULE_TASK
#define USE_MODULE_TIMING
#define USE_MODULE_LIST
#define USE_MODULE_BUFFER
#define USE_MODULE_BUFFER_PRINTF
#define USE_MODULE_UART

//hint: the MSP430F5529 uses UART1 for the builtin MSP Application UART1 virtual COM port
#define USE_UART2

// hint: the default clock for the MSP430F5529 is 1048576
// the default clock for the PIC32MX is set by configuration bits
#define FCPU     8000000L
// if peripheral clock is slower than main clock change it here
#define PERIPHERAL_CLOCK FCPU

#endif // _SYSTEM_H_
@endcode
 *
 * The main for this project may look something like this:
@code
#include "library.h" // for useful macros and int defs
#include "project_settings.h"
#include "timing.h"
#include "task.h"
#include "uart.h"
#include "hal_general.h" // for definition of EnableInterrupts

// define which uart channel to use
#define UART_CHANNEL 2
void hello_world(void) {
    UART_Printf(UART_CHANNEL, "Hello World\r\n");
}
int32_t main(void)
{
    // do any device specific configuration here
    // SYSTEMConfig(FCPU, SYS_CFG_ALL); // config clock for PIC32
	// WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer for MSP430

    Timing_Init(); // initialize the timing module first
    Task_Init(); // initialize task management module next
    UART_Init(UART_CHANNEL);
	// enable interrupts after modules using interrupts have been initialized
	EnableInterrupts();
    Task_Schedule(hello_world, 0, 10000, 10000);
    while(1) SystemTick();
}

@endcode
 *
@section Contributors 
Major Contributors:
- Anthony Merlino
- Michael Muhlbaier

Minor Contributors:
- Mike Bucceroni
- Rob Capo
- Charlotte Cecere
- Bradley Ebinger
- James Ethridge
- Nick Felker
- Chris Fredrickson
- Josh Klodnicki
- Joshua Haas
- Jehandad Kahn
- George Lecakes
- Matthew Lizzio
- Ryan Patterson
- Jonathan Whalen
- Rowan ECE Embedded Class of 2012
- Rowan ECE Embedded Class of 2013
- Rowan ECE Embedded Class of 2014
- Rowan ECE Embedded Class of 2015
- Rowan ECE Embedded Class of 2016
- Rowan ECE Embedded Class of 2017

If you have personally contributed to this work and are not listed please email muhlbaier@rowan.edu
 */

#include "macros.h"
#include "int_def.h"

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

#endif // _LIBRARY_H_
