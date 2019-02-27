/**
 * @defgroup hal_doc Hardware Abstraction Layer
 *
 * This library is carefully designed to make it simple for a user to port
 * the library to a new target processor.
 *
 * When porting this library the user should refer to the documentation for the
 * following modules:
 * - @ref hal_general HAL General Functions, used by multiple modules
 * - @ref hal_spi HAL for SPI Module
 * - @ref hal_uart HAL for UART Module
 * - @ref hal_timing HAL for Timing Module
 *
 * Once complete microcontroller specific documentation should be created.
 * Refer to @ref hal_cpu for complete instructions.
 *
 */

/**
 * @ingroup hal_doc
 * @defgroup hal_cpu Microcontroller Specific Documentation
 *
 * A documentation file should also be created in the documentation folder and
 * be named hal_[MCU_NAME].h, for example, hal_msp430f5529.h. This file should
 * include notes about using the library with the given processor. For example,
 * with the PIC32MX or PIC32MZ series a user can let the library handle the
 * peripheral pin select features with macros:
 * @code
 * #define UART1_TX_PIN RPA7
 * #define UART1_RX_PIN RPA8
 * @endcode
 * The processor specific documentation should also include warnings for any
 * unsupported modules.
 *
 */

/**
 * @ingroup hal_doc
 * @defgroup hal_clock Clock Configuration
 *
 * @ref hal_clock_pic32mx250f128b
 * 
 */

/**
 * @ingroup hal_doc
 * @defgroup hal_general Hardware Abstraction Layer General Fucntions
 *
 * The hal_general.h file is processor specific and should be located in each
 * processors directory within hal / processor family.
 *
 * Each of the following function prototypes are typically implemented with
 * macros are not actual functions.
 * @{
 */

/** @brief No oporation
 *
 * Note: this may already be implemented for some microcontrollers (i.e. PIC).
 */
void Nop(void);

/** @brief Disable global interrupts
 *
 * Disable global interrupts. This can destroy the interrupt status of the CPU
 * and should never be called from an interrupt.
 */
void DisableInterrupts(void);

/** @brief Configure and enable global interrupts
 *
 * Configures multi vectored interrupts where available. Will not restore the
 * interrupt level. See BlockInterrupts() and RestoreInterrupts()
 */
void EnableInterrupts(void);

/** @brief Backup the interrupt status (i.e. IPL) and block interrupts.
 *
 * A interrupt safe method to block interrupts and restore to the appropriate
 * state using the matching RestoreInterrupts function.
 */
void BlockInterrupts(void);

/** @brief RestoreInterrupts after blocking interrupts (BlockInterrupts())
 *
 * Restores the interrupt status that was backed up by BlockInterrupts() and
 * enables interrupts.
 *
 * @warning RestoreInterrupts() can only be called when BlockInterrupts() was
 * called earlier in the same context (function).
 */
void RestoreInterrupts(void);

/**@}*/