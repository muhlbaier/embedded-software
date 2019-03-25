# Jack's fork

# embedded-software
Embedded Software Library

[doxygen documentation](https://muhlbaier.github.io/embedded-software/)

This is a hardware agnostic library for embedded platforms.  Modules are written in a way that references a uniform Hardware Abstraction Layer (HAL).

## Coding Style Conventions ##

* All macros and constants in caps: SOME_MACRO
* Macros intended to be changed between projects start with module name underscore: UART_BAUD
* header file multi-inclusion protection defines: _FILE_NAME_H_

* Typedefs (including structs and unions) are lowercase and end with _t: list_t, union32_t
* HAL typedefs start with hal_: hal_spi_settings_t

* Functions are camelcase: GetChar()
* Public functions start with module name underscore function name: ItemBuffer_Init()
* * Minor exceptions exist for core system functionlity:
* * SystemTick()
* * TimeNow()
* * DelayMs()
* Private functions should be static.
* HAL functions start with hal_: hal_SPI_Init


* Variables, struct members, and union members are lower case underscore separated: my_var_x

* Files names are lowercase underscore separated module names: item_buffer.c, item_buffer.h
* HAL files start with hal_: hal_uart.c
* HAL header files need not exist and should only exist when hardware specific structs are required (e.g. hal_spi.h) or when a particular HAL provides additional functions not required by the module it serves (e.g. hal_UART_TxEnable() )

Note: if a module provides functions that are considered optional by another module the macro USE_MODULE_MODULE_NAME should be used as a flag to indicate to the other module that the functions are available for use. The USE_MODULE_MODULE_NAME flags should be defined in system.h. For example the UART module can automatically use the Task module to run UART_Tick when USE_MODULE_TASK is defined.

system.h is a hardware specific and project specific header that should be used to

* define what interdependent modules are in use (e.g. USE_MODULE_SUBSYS)
* define module macros that should be overridden (e.g. UART_BAUD 9600, default is 115200)
* define system clock speeds (e.g. FCPU, PERIPHERAL_CLOCK, etc.)
* optionally other project specific things appropriate for a header file


## Documentation Style ##

Doxygen style @ documentation in header files with documentation directed at the module user. 
Each module uses @ defgroup module_name Module Description. If a subset of a larger module or module collection use @ ingroup.
Include usage examples where appropriate using @ code and @ endcode.

Non-doxygen style documentation in source files with documentation directed at the maintainer of the source code (future you). Should explain anything that is not immediately apparent to an experienced C programmer.

## Other Programming Methodology Notes ##

* Emphasis is put on making software reusable by principles of encapsulation, portability, and modularity. 
* Any non HAL software should be written to work on any MCU with any C compiler. 
* Callbacks are preferred over polling to support safe event driven modules.
