/***********************************************************************
 * @defgroup rs485controller RS485 Controller Module
 *
 * @{ */

#ifndef _RS485CONTROLLER_H_
#define _RS485CONTROLLER_H_
 
#include "RS485Client.h"
#include "RS485Packet.h"
 
// max number of slaves
#define RS485CONTROLLER_MAX_SLAVES 4
// max number of packets to send that can be buffered
#define RS485CONTROLLER_BUFFER_MAX_LENGTH 8
// max receive callbacks
#define RS485CONTROLLER_MAX_CALLBACKS 8

#define RS485CONTROLLER_PACKET_TRANSMIT_TIME_MS 12

// default address of master node
#define RS485_MASTER_ADDRESS 0xFF

typedef enum {
    CLIENT_DEFAULT=0,
    CLIENT_SEND,
    CLIENT_RECIEVE,
    CLIENT_DISABLED
} rs485_state_t;

typedef struct rs485ctrlr* rs485ctrlr_handle_t;

struct rs485ctrlr{
    rs485_state_t state;			//current state of the controller
    rs485_state_t state_prev;	//previous state of the controller
    uint32_t tx_count;				//number of transmitted packets
    uint32_t rx_count;				//number of received packets
    float avg_pps;					//avg throughput in packets per second
    uint32_t ms_last_received[RS485CONTROLLER_MAX_SLAVES];    // timestamp of last packet received from endpoint
    uint32_t ms_last_sent; // time last message was sent, can be used to calculate round trip time
    uint32_t ms_timeout;		// timeout of when communications are considered lost
    uint32_t reset_count;			//incremented each time the controller is reset (after the first)
    rs485_packet_t packet;		//memory storage for current packet contents
    rs485_client_handle_t client;	//handle to rs485 client for data transport
    uint32_t num_slaves;			//number of slaves on the bus (0 means we are master)
    uint32_t addr_index;					//currently indexed peer (only used when master)
    uint32_t my_address;	// address of myself
    rs485_packet_t packet_buffer[RS485CONTROLLER_BUFFER_MAX_LENGTH]; // packet buffer single linked list
    uint16_t next_packet_index[RS485CONTROLLER_BUFFER_MAX_LENGTH]; // index links for single linked list
    uint16_t first_packet; // index of first item in single linked list
    uint16_t last_packet; // index of last item in single linked list
    uint16_t buffer_length; // current number of packets in the packet buffer
    struct {
        void(*callback)(rs485ctrlr_handle_t, uint8_t*, uint8_t);
        uint16_t control;
        uint16_t destination;
    } callbacks[RS485CONTROLLER_MAX_CALLBACKS]; // list of receive callbacks
    void(*packet_constructor)(rs485_packet_t *, uint8_t); // default packet constructor 
    void(*com_down_callback)(rs485ctrlr_handle_t); // callback for communication down event
};

/********************************************************************//**
 * @brief 		Return a handle for an RS485 controller instance
 * @param[in]	None
 * @return 		Handle (pointer) to controller structure
 *
 * Currently implemented as a singleton (5/12/2016)
 *********************************************************************/
rs485ctrlr_handle_t RS485Controller_Create(void);

/***********************************************************************
 * @brief initialize controller and client, register receiver callback with client
 * 
 * if num_slaves = 0 then the controller is in SLAVE mode
 * if num_slaves > 0 then the controller is in MASTER mode
 * 
 * @param[in]	controller - handle for the controller instance
 * @param[in]	num_slaves - number of slaves on the bus (0 if a slave node)
 * @param[in]	address - address of this node on the bus (required if a slave node)
 * @return		None
 *********************************************************************/
void RS485Controller_Init(rs485ctrlr_handle_t controller, uint8_t num_slaves, uint8_t address);

/***********************************************************************
 * @brief Reset the controller data and state
 *
 * @return		None
 *********************************************************************/
void RS485Controller_Reset(rs485ctrlr_handle_t controller);

/************************************************************************
 * @brief Control RS485 client based on mode (master/slave) and state
 *
 * If RS485CONTROLLER_COM_TIMEOUT ms elapse without receiving a packet
 * then the ComDownCallback will be called
 *
 * - If state is SEND
 * -- If send buffer has a packet then send it (if master get first packet for the slave address)
 * -- Else send the default packet (call the DefaultPacketConstructor)
 * -- Change state to READ (if master then increment the slave index)
 * - If state is READ then check Client for received packet. When packet is
 *   received it will be processed by the RS485Controller's internal callback
 *   which will set the state to SEND.
 *
 * @param[in]	controller - handle for the controller instance
 * @return		None
 *
 * @warning not interrupt safe
 *********************************************************************/
void RS485Controller_Run(rs485ctrlr_handle_t controller);

/************************************************************************
 * @brief Add packet to send buffer
 *
 * If sending from the master the packet will be sent in FIFO order when
 * transmitting to the destination address set in the packet.
 *
 * If sending from a slave the packet will be sent in FIFO order after
 * receiving a packet from the master.
 *
 * @param[in]	controller - handle for the controller instance 
 * @param[in]	packet - pointer to the rs485_packet_t packet to send
 * @return		None
 *
 * @warning not interrupt safe
 *********************************************************************/
void RS485Controller_Send(rs485ctrlr_handle_t controller, rs485_packet_t * packet);

/************************************************************************
 * @brief Set packet received callback for a given packet control type 
 * and destination
 *
 * Example usage:
 * @code
 * void Callback(rs485ctrlr_handle_t controller, uint8_t* buffer, uint8_t length) {
 *    // do something with the data
 *    // maybe send a response:
 *    // need to create packet here
 *    RS485Controller_Send(controller, packet);
 * }
 * //...
 * // set callback to received all packets sent to address 0x02
 * RS485Controller_SetCallback(controller, Callback, 0xFF, 0x02);
 * @endcode
 * 
 * @param[in]	controller - handle for the controller instance
 * @param[in]	callback - function pointer to callback
 * @param[in]	control - only call callback if control matches this value.
 *              Use 0xFF to match all control values.
 * @param[in]	destination - only call callback if destination matches this value.
 *              Use 0xFF to match all destination values.
 * @return		None
 *********************************************************************/
void RS485Controller_SetCallback(rs485ctrlr_handle_t controller,
    void(*callback)(rs485ctrlr_handle_t controller, uint8_t* buffer, uint8_t length),
    uint8_t control, uint8_t destination);
	
/************************************************************************
 * @brief To ensure constant communications a default packet should be sent
 * when none is available in the send buffer.
 *
 * Example usage:
 * @code
 * // packet is a pass by reference output
 * // address is the destination address for the packet
 * void MyConstructor(rs485_packet_t *packet, uint8_t address) {
 *    uint8_t data[4] = {1,2,3,4}; // dummy data
 *    uint8_t control = 0; // control field for packet
 *    RS485Packet_init(packet, address, control, &data[0], 4);
 * }
 * // ...
 * RS485Controller_SetDefaultPacketConstructor(controller, MyConstructor);
 * @endcode
 *
 * @param[in]	controller - handle for the controller instance
 * @param[in]	constructor - pointer to packet constructor function
 * @return		None
 *********************************************************************/
void RS485Controller_SetDefaultPacketConstructor(rs485ctrlr_handle_t controller, 
    void(*constructor)(rs485_packet_t *packet, uint8_t destination));

/************************************************************************
 * @brief Set callback and timeout for handling loss of communications
 *
 * Calling this function will enable the communication down event feature.
 * This feature will track the time the last packet was received and
 * trigger the event if no packet is received after timeout ms. When detected
 * the callback is called and the user can determine how to handle the event.
 *
 * @param[in]	controller - handle for the controller instance
 * @param[in]	callback - function pointer to function which handles the communication down event
 * @param[in]	timeout - timeout in ms
 * @return		None
 *********************************************************************/
void RS485Controller_SetComDownCallback(rs485ctrlr_handle_t controller, 
    void(*callback)(rs485ctrlr_handle_t controller), uint32_t timeout);
	
/** @} rs485controller */
#endif
