/** @defgroup spi SPI Module
 *
 * File:   spi.h
 * Author: Anthony Merlino
 *
 * Transactional Master SPI module capable of bi-directional data transfer in a 
 * single transaction. Support blocking and non-blocking modes.
 * 
 * Currently supports 8-bit mode only.
 *
 * Created on February 24, 2015, 11:03 PM
 * @{
 */

#ifndef _SPI_H_
#define	_SPI_H_

#include <stdint.h>

#include "project_settings.h"

#include "hal_spi.h"

#ifndef SPI_MAX_SIZE
#define SPI_MAX_SIZE 8 ///< max length
#endif

#ifndef SPI_MAX_TRANSACTIONS
#define SPI_MAX_TRANSACTIONS 4 ///< max queued transactions
#endif

/// spi settings structure
typedef struct spi_settings_t {
    uint8_t channel; ///< channel number of module to be configured
    uint32_t bit_rate; ///< bit_rate of SPI clock, not guaranteed to be accurate,
    /// depends on processor clock settings and SPI peripheral
    uint8_t mode; ///< SPI mode select
    /// - Mode 0 idle clock is low, data changes on high to low clock edge
    /// - Mode 1 idle clock is low, data changes on low to high clock edge
    /// - Mode 2 idle clock is high, data changes on low to high clock edge
    /// - Mode 3 idle clock is high, data changes on high to low clock edge
    hal_spi_settings_t hal_settings; ///< processor specific settings
} spi_settings_t;

/// spi transaction structure
struct spi_transaction {
    uint8_t writeLength; ///< write length
    uint8_t readLength; ///< read length
    uint8_t readDelay; ///< read delay
    struct {
        volatile uint8_t blocking : 1; ///< blocking flag
        volatile uint8_t finished : 1; ///< transaction finished flag
        volatile uint8_t channel : 3; ///< channel setting
        volatile uint8_t overrun : 1; ///< overrun error occured
        volatile uint8_t unused : 2; ///< unused
    } flags;
    void (*callback)(struct spi_transaction *); ///< callback function
    void (*cs_control)(uint8_t); ///< cs_control
    uint8_t data[SPI_MAX_SIZE]; ///< data (write data followed by read data)
};

/// spi transaction typedef
typedef struct spi_transaction spi_transaction_t;

/** @brief initialize a SPI channel
 * 
 * To initialize a SPI module a spi_setting_t struct must be configured with the
 * desired settings and then passed in by reference to this function.
 * 
 * Example usage:
@code
spi_settings_t spi_settings;
spi_settings.channel = 1;
spi_settings.bit_rate = 100000;
// configure spi_settings.hal_settings as required for the target processor
spi_settings.mode = 0;
SPI_Init(&spi_settings);
@endcode
 *
 * @param spi_settings pointer to spi_settings_t struct.
 */
void SPI_Init(spi_settings_t* spi_settings);

/** @brief send a SPI transaction
 *
 * When configuring the SPI transaction the user must specify:
 * - transaction.flags.channel The SPI channel to use
 * - transaction.flags.blocking Whether or not to use blocking mode
 * - transaction.cs_control A pointer to a chip select function
 * - transaction.writeLength How many bytes to write
 * - transaction.readDelay How many bytes to delay before saving read data
 * - transaction.readLength How many bytes to read
 * - transaction.data Initialize data in array to write (if reading and writing 
 *   simultaneously then write data will be overwritten with read data after it 
 *   is written).
 * 
 * Usage Example:
@code
void Channel1ChipSelect(uint8_t cs) {
    // assuming CS_PIN_1 is defined as an output register somewhere
    CS_PIN_1 = cs ? 1 : 0;
}
//...
// write two bytes, receive the second byte through the fifth byte
spi_transaction_t transaction;
transaction.flags.channel = 1;
transaction.flags.blocking = 1;
transaction.cs_control = Channel1ChipSelect;
transaction.writeLength = 2;
transaction.readLength = 4;
transaction.readDelay = 1;
transaction.data[0] = 0x20;
transaction.data[1] = 0x85;
SPI_Transact(&transaction);
// at this point transaction.data[1] through transaction.data[4] contain the data read
@endcode
 * @param transaction pointer to spi_transaction_t structure
 *
 * @warning Not interrupt safe
 */
int8_t SPI_Transact(spi_transaction_t * transaction);

/** @brief Interrupt service routine to be called from the HAL
 *
 * @param channel SPI channel
 */
void SPI_ISR(uint8_t channel);

/******************************************
 * HAL Functions
 * The following functions can be implemented in hal_spi.c or as
 * #define's in hal_spi.h
 *****************************************/

/** Initialize the SPI peripheral given the settings structure
 * 
 * Note: the developer of hal_spi can define hal_spi_settings_t to have whatever
 * is required for implementing SPI on the target processor.
 *
 * @param settings pointer to spi_settings_structure.
 */
#ifndef hal_SPI_Init
void hal_SPI_Init(spi_settings_t* settings);
#endif

/** Enable the SPI peripheral
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_Enable
void hal_SPI_Enable(uint8_t channel);
#endif

/** Disable the SPI peripheral
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_Disable
void hal_SPI_Disable(uint8_t channel);
#endif

/** Get space available status of SPI transmit buffer
 *
 * @param channel SPI channel number
 * @return Return 0 if no space available in transmit buffer, non-zero if space
 * is available
 */
#ifndef hal_SPI_SpaceAvailable
uint8_t hal_SPI_SpaceAvailable(uint8_t channel);
#endif

/** Get data available status of SPI receive buffer
 *
 * @param channel SPI channel number
 * @return 0 if no data is available in receive buffer, non-zero if data is
 * available
 */
#ifndef hal_SPI_DataAvailable
uint8_t hal_SPI_DataAvailable(uint8_t channel);
#endif

/** Clear SPI receive interrupt flag
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_ClearRxIF
void hal_SPI_ClearRxIF(uint8_t channel);
#endif

/** Clear SPI transmit interrupt flag
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_ClearTxIF
void hal_SPI_ClearTxIF(uint8_t channel);
#endif

/** Enable SPI receive interrupts
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_EnableRxInterrupt
void hal_SPI_EnableRxInterrupt(uint8_t channel);
#endif

/** Enable SPI transmit interrupts
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_EnableTxInterrupt
void hal_SPI_EnableTxInterrupt(uint8_t channel);
#endif

/** Disable SPI receive interrupts
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_DisableRxInterrupt
void hal_SPI_DisableRxInterrupt(uint8_t channel);
#endif

/** Disable SPI transmit interrupts
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_DisableTxInterrupt
void hal_SPI_DisableTxInterrupt(uint8_t channel);
#endif

/** Get status of SPI transmit interrupt flag
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_TxIntStatus
uint8_t hal_SPI_TxIntStatus(uint8_t channel);
#endif

/** Get status of SPI receive interrupt flag
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_RxIntStatus
uint8_t hal_SPI_RxIntStatus(uint8_t channel);
#endif

/** Get enable status of SPI transmit interrupt
 *
 * @param channel SPI channel number
 */
#ifndef hal_SPI_IsTxIntEnabled
uint8_t hal_SPI_IsTxIntEnabled(uint8_t channel);
#endif

/** Get byte from SPI receive buffer / register
 *
 * @param channel SPI channel number
 * @return data read from receive buffer / register
 */
#ifndef hal_SPI_RxByte
uint8_t hal_SPI_RxByte(uint8_t channel);
#endif

/** Write byte to SPI transmit buffer / register
 *
 * @param channel SPI channel number
 * @param byte to write
 */
#ifndef hal_SPI_TxByte
void hal_SPI_TxByte(uint8_t channel, uint8_t b);
#endif

/** Check if buffer overrrun has occured.
 *
 */
#ifndef hal_SPI_OverrunError
uint8_t hal_SPI_OverrunError(uint8_t channel);
#endif

///@}
#endif // _SPI_H_
