/**
 * @file   nrf24network.h
 * @author Michael Muhlbaier
 *
 * @defgroup nrf24network nRF24L01+ Network (geared for class projects)
 *
 * @version 0.1
 * 
 * Dependencies:
 * nrf24.h
 * spi.h
 * task.h
 * timing.h
 * subsys.h
 * uart.h
 *
 * Default Pinouts:
 * 
 * MSP430F5529:
 * - MISO P3.1
 * - MOSI P3.0
 * - SCK  P3.2
 * - CS   P2.3
 * - CE   P2.4
 * - IRQ  P2.5
 * 
 * PIC32MX250F128B:
 * - MISO pin3/A1
 * - MOSI pin24/B13
 * - SCK  pin25/B14
 * - CS   pin7/B3
 * - CE   pin6/B2
 * - IRQ  pin9/A2
 *
 * Created on April 7, 2015, 6:24 PM
 *
 * @{
 */

#ifndef NRF24NETWORK_H
#define	NRF24NETWORK_H

#include "project_settings.h"
#include <stdint.h>
#include "timing.h"
#include "nrf24.h"

/** function pointer type for consuming certain msg_id's */
typedef void (*nrf24_handler_fn_t)(uint8_t * data, uint8_t len, uint8_t from);

/** Helper union for the first two bytes of each payload which
 * includes to and from addresses.
 */
typedef union {
    uint16_t word; 
    uint8_t b[2];
    struct {
        uint16_t to_leaf: 3;
        uint16_t to_branch: 3;
        uint16_t from_leaf: 3;
        uint16_t from_branch: 3;
        uint16_t multi_msg: 1;
        uint16_t msg_len: 3; ///< 0 = 2 bytes, 1 = 4 bytes, ... 7 = 16 bytes
    };
    struct {
        uint16_t to: 6;
        uint16_t from: 6;
        uint16_t reserved: 4;
    };
} nrfnet_address_t;

#define NRF24_CHANNEL_BRANCH_1 30
#define NRF24_CHANNEL_BRANCH_2 34
#define NRF24_CHANNEL_BRANCH_3 38
#define NRF24_CHANNEL_BRANCH_4 42
#define NRF24_CHANNEL_BRANCH_5 46
#define NRF24_CHANNEL_BRANCH_6 50

#define NRF24_ADDRESS_MASK 0x6C6C6C7800

#define NRF24_MIN_WINDOW_MS 100 //TODO change this to 4
#define NRF24_BRANCH_WINDOW_MS 80 //TODO change this to 4
#define NRF24_TICK_MS 2 //TODO change this to 1
#define NRF24_MISSING_NODE_TIMEOUT 0

// The third payload byte (after the two address bytes) will be the message ID
enum nrf24_msg_id {
    SYSTEM_MSG = 0, ///< built-in / MM
    CONTROL_MSG, ///< general control msg / max payload 5+ bytes
    SENSOR_MSG, ///< general sensor msg / max payload 4+ bytes
    CHAT_MSG, ///< built-in / Jon W.
    TEST_MSG, ///<Used for testing the network
    KILL_MSG, ///< Used to remotely kill (reset) a node on the network
    THIEF_MSG, ///< Used to support the Theif class project
    LAST_MSG_ID ///< reserved name to determine number of message IDs supported
};

enum nrf24_system_msg {
    NULL_MSG = 0,
    PING_MSG,
    PING_RESPONSE_MSG,
    SPAM_MSG  
};

// The fourth payload byte of control messages will be a control message sub-ID
enum nrf24_control_msg {
    ONBOARD_LED,
    LINEAR_ACTUATOR,
    LAST_CONTROL_ID ///< reserved name to determine number of control msgs supported
};

// The fourth payload byte of sensor messages will be a sensor message sub-ID
enum nrf24_sensor_msg {
    TEMPERATURE_SENSOR,
    LAST_SENSOR_ID ///< reserved name to determine number of sensors supported
};

enum nrf24_thief_msg {
    ARM_MSG, ///< data is difficulty
    DISARM_MSG, ///< data is difficulty
    TRIP_MSG, ///< data is score (1 barely tripped)
    ACCESS_MSG, ///< data is score (1 great job)
    ERROR_MSG, ///< data is error code (TBD)
    DIFFICULTY_MSG, ///< data is difficulty
    LAST_THIEF_ID ///< reserved name to determine number of messages supported
};

enum nrf24_address {
    // assign fixed node addresses
    MASTER = 0x00, ///< Brainframe / Scrumshank Redemption
    SCRUM_PUN = 0x08, ///< Branch 1 - ScrumPun
    BALL_TRAP,
    TOMS_PAD,
    NOODLE,
    MOUSE_TRAP = 0x10, ///< Branch 2 - RedBulls
    ULTRASONIC = 0x18, ///< Branch 3 - SmartHomies
    DAN,
    CAM,
    JAKE,
    SUPREME = 0x20, ///< Branch 4 - TeamSupreme
    BOT,
    SNEAK,
    LASER,
    SIMON,
    OGLAZERBOIZ = 0x28, ///< Branch 5 - Muhlbaier's Fav. Clinic
    OGLAZ2,
    LOCK_PICK,
    PIEZO,
    MIKE = 0x30, ///< Branch 6 - Scrumshank Redemption
    SORCE,
    MALONEY,
	MUHLBAIER,
	LAST_ADDRESS,
    ALL_ALL = 0x3F
};

enum nrfstate_e {
        NRFNET_INITIALIZED = 0x00,
        NRFNET_NORMAL_STATE = 0x00,
        NRFNET_WAITING_FOR_MIN_WINDOW
    };
    
enum nrfrole_e {ROLE_MASTER, ROLE_BRANCH, ROLE_LEAF};

enum pipe_status_e {
    CHILD_INITIALIZED,
    CHILD_ACTIVE,
    CHILD_MISSING
};

typedef struct {
    uint8_t length; 
    uint8_t child; // child 0xFF means unused buffer slot
    uint8_t age; // how long this msg has been on the buffer (relative, 0 is newest)
    uint8_t data[32];
} nrfnet_msg_t;

#define NRFNET_MSG_BUFFER_LENGTH 16
typedef struct nrfnet_s {
    nrf24_t radio;
    uint8_t sys_id;
    uint8_t node;
    char name[5];
    enum nrfrole_e role;
    enum nrfstate_e state;
    volatile uint8_t current_child;
    uint32_t child_time[6];
    uint32_t switch_to_tx_time;
    enum pipe_status_e child_status[6];
    uint8_t channel[6];
    nrfnet_msg_t msg_buf[NRFNET_MSG_BUFFER_LENGTH];
    nrf24_handler_fn_t handler[LAST_MSG_ID];
    nrf24_handler_fn_t control_handler[LAST_CONTROL_ID];
    nrf24_handler_fn_t sensor_handler[LAST_SENSOR_ID];
} nrfnet_t;

/** @brief Initialize nRF24 Network Module
 * 
 * @param ce Function pointer to Chip Enable signal function. If input to the 
 * function is 0 the CE line should output GND. If the input to the function is
 * 1 the CE pin should output 3.3V
 * @param csn Function pointer to Not Chip Select signal function. If input to the 
 * function is 0 the CSN line should output GND. If the input to the function is
 * 1 the CSN pin should output 3.3V
 * @param spi_channel SPI channel number to be passed to the @ref spi
 */
void nrf24_NetworkInit(void (*ce)(uint8_t), void (*csn)(uint8_t), uint8_t spi_channel);
void nrf24_NetworkInitN(nrfnet_t * net, void (*ce)(uint8_t), void (*csn)(uint8_t), uint8_t spi_channel, uint8_t node);

/** Register a message Handler for a specific message ID
 * 
 * When a message with a message ID that matches @c msg_id is received the
 * Handler function will be called with a data pointer to the remaining payload
 * (data), the length of the remaining payload (len), and the from address (from).
 * 
 * Only one handler per message ID may be registered. 
 * 
 * Handlers for SYSTEM_MSG, CONTROL_MSG, SENSOR_MSG, and CHAT_MSG are built-in
 * but can be overridden.
 * 
 * Example:
@code
void PongHandler(uint8_t * data, uint8_t len, uint8_t from) {
    // implement pong handler
}
nrf24_RegisterMsgHandler(PONG_MSG, PongHandler);
@endcode
 * 
 * @param msg_id Message ID to register Handler for
 * @param fn_ptr Pointer to Handler function
 */
void nrf24_RegisterMsgHandler(enum nrf24_msg_id msg_id, nrf24_handler_fn_t fn_ptr);
void nrf24_RegisterMsgHandlerN(nrfnet_t * net, enum nrf24_msg_id msg_id, nrf24_handler_fn_t fn_ptr);

/** Send a message over the nRF24 network
 *
 * @param to address to send message to (use enum nrf24_address for now)
 * @param msg_id Message ID
 * @param data pointer to data payload (address bytes and message ID will be
 *          generated automatically, this pointer should point to the fourth
 *          byte of the payload).
 * @param len length to the data payload (min 0, max 29)
 */
void nrf24_SendMsg(uint8_t to, enum nrf24_msg_id msg_id, uint8_t * data, uint8_t len);
void nrf24_SendMsgN(nrfnet_t * net, uint8_t to, enum nrf24_msg_id msg_id, uint8_t * data, uint8_t len);

/**  Register a control message Handler for a specific control ID
 *
 * Same as nrf24_RegisterMsgHandler() except the Handler will be called when
 * the message ID is CONTROL_MSG and the control_id (byte after msg ID,
 * i.e. byte 4) matches @c control_id.
 *
 * @param control_id Control ID to register Handler for
 * @param fn_ptr Pointer to Handler function
 */
void nrf24_RegisterControlHandler(enum nrf24_control_msg control_id, nrf24_handler_fn_t fn_ptr);
void nrf24_RegisterControlHandlerN(nrfnet_t * net, enum nrf24_control_msg control_id, nrf24_handler_fn_t fn_ptr);

/**  Register a sensor message Handler for a specific sensor ID
 *
 * Same as nrf24_RegisterMsgHandler() except the Handler will be called when
 * the message ID is SENSOR_MSG and the sensor_id (byte after msg ID,
 * i.e. byte 4) matches @c sensor_id.
 *
 * @param sensor_id Sensor ID to register Handler for
 * @param fn_ptr Pointer to Handler function
 */
void nrf24_RegisterSensorHandler(enum nrf24_sensor_msg sensor_id, nrf24_handler_fn_t fn_ptr);
void nrf24_RegisterSensorHandlerN(nrfnet_t * net, enum nrf24_sensor_msg sensor_id, nrf24_handler_fn_t fn_ptr);

void nrf24_NetworkISRHandler(void);
#define nrf24_NetworkISRHandlerN(net) nRF24_ISR(&net.radio)

void nrf24_Ping(uint8_t to);

/** Convert network address to name
 *
 * @param address address to convert to name
 * @return string pointer to name
 */
char * NameFromAddress(uint8_t address);

/**
 *
 * @param name pointer to name to lookup
 * @return address network address
 */
uint8_t AddressFromName(char * name);

 /** @} */
 
// possible additions:
// - add method to send a control message
// - add method to request sensor data
// - add method to support automatic response of sensor data request
// - add network level nack for undeliverable packets

#endif	/* NRF24NETWORK_H */
