#include <string.h>
#include "project_settings.h"
#include "nrf24network.h"
#include "task.h"
#include "subsystem.h"
#include "hal_general.h"
#include "uart.h"
#include <strings.h>

#ifndef THIS_NODE
//#error "Please add #define THIS_NODE <ADDRESS> in system.h where ADDRESS is your nrf24_address"
#else
#if THIS_NODE > 0x36
//#error "Invalid node address for THIS_NODE"
#endif
#endif

#ifndef SUBSYS_UART
#error "You must define what UART number the terminal is using in system.h: #define SUBSYS_UART num"
#endif

#define GET_THIS_BRANCH(x) (x>>3)

#define ADDRESS_TO_INDEX(x) (x)//(((x?(x-1):0) >> 3) * 7 + (x % 8) + 1)
#define INDEX_TO_ADDRESS(x) (x)//((((x-1)/7) << 3) + ((x-1)%7))

char names[ADDRESS_TO_INDEX(LAST_ADDRESS)][5] = {
		{"Mast"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"Spun"},
		{"Btrp"},
		{"Tpad"},
		{"Nood"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"Mtrp"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"USon"},
		{"Dan_"},
		{"Cam_"},
		{"Jake"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"Supr"},
		{"Bot_"},
		{"Snek"},
		{"Lasr"},
		{"Simo"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"OGLA"},
		{"OGL2"},
		{"LkPk"},
		{"PIEZ"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"Mike"},
		{"SRCE"},
		{"Maln"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},
		{"xxxx"},		
		{"Muhl"},
};

#define NRFNET_PARENT_PIPE 0x07
#define NRFNET_RX_PIPE 1

nrfnet_t default_net;
nrfnet_t * second_net_ptr = 0;

// CALLBACKS FOR nRF24 LAYER
// if TX_DS and RX_DR are both set then call ProcessAck before ProcessAckPayload
static void TxAckCallback(void); // should be called when in PTX mode and TX_DS flag is set
static void TxAckPayloadCallback(uint8_t * data, uint8_t length);
static void MaxRetriesCallback(void);
static void ProcessPayloadCallback(uint8_t * data, uint8_t length);
static void AckPayloadSentCallback(void);
static void TxAckCallback2(void); // should be called when in PTX mode and TX_DS flag is set
static void TxAckPayloadCallback2(uint8_t * data, uint8_t length);
static void MaxRetriesCallback2(void);
static void ProcessPayloadCallback2(uint8_t * data, uint8_t length);
static void AckPayloadSentCallback2(void);
static void TxAckCallbackN(nrfnet_t * net); // should be called when in PTX mode and TX_DS flag is set
//static void TxAckPayloadCallbackN(nrfnet_t * net, uint8_t * data, uint8_t length);
static void MaxRetriesCallbackN(nrfnet_t * net);
static void ProcessPayloadCallbackN(nrfnet_t * net, uint8_t * data, uint8_t length);
static void AckPayloadSentCallbackN(nrfnet_t * net);
static void TxToNextChild(nrfnet_t * net);

static void NetworkTick(nrfnet_t * net);
static void ProcessMessage(nrfnet_t * net, uint8_t * data, uint8_t length);
static void QueueMsgToChild(nrfnet_t * net, uint8_t child, uint8_t * data, uint8_t length);
static void QueueMsgToParent(nrfnet_t * net, uint8_t * data, uint8_t length);
static void CheckAckMessage(nrfnet_t * net);
static void TxEmptyPacket(nrfnet_t * net);
static void ProcessReceivedMsg(nrfnet_t * net, uint8_t * data, uint8_t length);
static void ConfigureNetwork(nrfnet_t * net);

static void SystemHandler(nrfnet_t * net, uint8_t * data, uint8_t len, uint8_t from);
static void ControlHandler(nrfnet_t * net, uint8_t * data, uint8_t len, uint8_t from);
static void SensorHandler(nrfnet_t * net, uint8_t * data, uint8_t len, uint8_t from);
static void ChatHandler(nrfnet_t * net, uint8_t * data, uint8_t len, uint8_t from);

// buffer helpers
static nrfnet_msg_t * GetMsgForChild(nrfnet_t * net, uint8_t pipe);
static void AddDataForPipe(nrfnet_t * net, uint8_t pipe, uint8_t * data, uint8_t length);
static void FreeMsg(nrfnet_msg_t * msg);

static void SubsysCallback(int argc, char *argv[]);
static void UART_Receiver(uint8_t c);

void nrf24_NetworkInit(void (*ce)(uint8_t), void (*csn)(uint8_t), uint8_t spi_channel) {
    nrf24_NetworkInitN(&default_net, ce, csn, spi_channel, THIS_NODE);
}
void nrf24_NetworkInitN(nrfnet_t * net, void (*ce)(uint8_t), void (*csn)(uint8_t), uint8_t spi_channel, uint8_t node) {
    static uint8_t net_index = 1;
    uint8_t i;
    version_t v;
    v.word = 0x0000000F;
    net->name[0] = 'N'; net->name[1] = 'e'; net->name[2] = 't'; net->name[3] = '0'+net_index++; net->name[4] = 0;
    net->sys_id = Subsystem_Init(&net->name[0], v, SubsysCallback);
    // initialize task module if it hasn't been done already
    Task_Init();
    net->state = NRFNET_INITIALIZED;
    for(i = 0; i < 6; i++) {
        net->child_time[i] = 0;
        net->child_status[i] = CHILD_INITIALIZED;
    }
    net->channel[0] = NRF24_CHANNEL_BRANCH_1;
    net->channel[1] = NRF24_CHANNEL_BRANCH_2;
    net->channel[2] = NRF24_CHANNEL_BRANCH_3;
    net->channel[3] = NRF24_CHANNEL_BRANCH_4;
    net->channel[4] = NRF24_CHANNEL_BRANCH_5;
    net->channel[5] = NRF24_CHANNEL_BRANCH_6;
    // clear all messages
    for(i = 0; i < NRFNET_MSG_BUFFER_LENGTH; i++) net->msg_buf[i].child = 0xFF;
    for(i = 0; i < LAST_MSG_ID; i++) net->handler[i] = 0;
    for(i = 0; i < LAST_CONTROL_ID; i++) net->control_handler[i] = 0;
    for(i = 0; i < LAST_SENSOR_ID; i++) net->sensor_handler[i] = 0;
    // initialize nRF24L01+ module (which should init SPI channel)
    net->handler[SYSTEM_MSG] = (nrf24_handler_fn_t)SystemHandler;
    net->handler[CONTROL_MSG] = (nrf24_handler_fn_t)ControlHandler;
    net->handler[SENSOR_MSG] = (nrf24_handler_fn_t)SensorHandler;
    net->handler[CHAT_MSG] = (nrf24_handler_fn_t)ChatHandler;
    if(net == &default_net) {
		net->radio.AckReceived = TxAckCallback;
		net->radio.AckPayloadReceived = TxAckPayloadCallback;
		net->radio.MaxRetriesHit = MaxRetriesCallback;
		net->radio.ReceivedPayload = ProcessPayloadCallback;
		net->radio.AckPayloadSent = AckPayloadSentCallback;
		// register char receiver for chat
		UART_RegisterReceiver(SUBSYS_UART, UART_Receiver);
    }else{
        net->radio.AckReceived = TxAckCallback2;
        net->radio.AckPayloadReceived = TxAckPayloadCallback2;
        net->radio.MaxRetriesHit = MaxRetriesCallback2;
        net->radio.ReceivedPayload = ProcessPayloadCallback2;
        net->radio.AckPayloadSent = AckPayloadSentCallback2;
        second_net_ptr = net;
    }
    net->radio.spi_channel = spi_channel;
    net->radio.ce = ce;
    net->radio.csn = csn;
    nRF24_Init(&net->radio);
    nRF24_SetRetries(&net->radio, 1, 5);
//    nRF24_SetRetries(&net->radio, 0xF, 0x01);
    //TODO set address mode
    // if this is the default net then set it up. Otherwise wait til the user
    // sets the node address
	net->node = node;
	// configure network will open a TX pipe on channel 1 for master and send a blank message to get things going
	// or open an RX pipe for the appropriate branch if not master
	ConfigureNetwork(net);
    // schedule task to keep network timing on point
    Task_Schedule((task_t)NetworkTick, net, NRF24_TICK_MS, NRF24_TICK_MS);
    //Log_MuteSys(net->sys_id);
}

void nrf24_RegisterMsgHandler(enum nrf24_msg_id msg_id, nrf24_handler_fn_t fn_ptr) {
    nrf24_RegisterMsgHandlerN(&default_net, msg_id, fn_ptr);
}
void nrf24_RegisterMsgHandlerN(nrfnet_t * net, enum nrf24_msg_id msg_id, nrf24_handler_fn_t fn_ptr) {
    if(msg_id >= LAST_MSG_ID) {
        LogMsg(net->sys_id, "Invalid msg_id: %d", msg_id);
    }else {
        if(msg_id <= CHAT_MSG) {
            LogMsg(net->sys_id, "Reserved ID: %d", msg_id);
        }else net->handler[msg_id] = fn_ptr;
    }
}

void nrf24_SendMsg(uint8_t to, enum nrf24_msg_id msg_id, uint8_t * data, uint8_t len) {
	nrf24_SendMsgN(&default_net, to, msg_id, data, len);
}

void nrf24_SendMsgN(nrfnet_t * net, uint8_t to, enum nrf24_msg_id msg_id, uint8_t * data, uint8_t len) {
    // create the message
    uint8_t msg[32];
    nrfnet_address_t address;
    address.from = net->node;
    address.to = to;
    msg[0] = address.b[1];
    msg[1] = address.b[0];
    msg[2] = msg_id;
    if(len > 29) len = 29;
    uint8_t i;
    len += 3;
    for(i = 3; i < len; i++) msg[i] = *data++;
    //memcpy(&msg[3], data, len);
    switch(net->role) {
        case ROLE_MASTER:
            // check if the message is to everyone
            if(to == ALL_ALL) {
                // queue to send to all children
                uint8_t i;
                for(i = 1; i < 6; i++) {
                    // skip sending to who it was from
                    if(i == address.from_branch) continue;
                    QueueMsgToChild(net, i-1, &msg[0], len);
                }
            }else {
                // if not to everyone then queue to send to child
                QueueMsgToChild(net, address.to_branch-1, &msg[0], len);
            }
            break;
        case ROLE_BRANCH:
            // if the message is to everyone then send to all children and to parent
            if(to == ALL_ALL) {
                uint8_t i;
                for(i = 1; i < 6; i++) {
                    QueueMsgToChild(net, i-1, &msg[0], len);
                }
                QueueMsgToParent(net, &msg[0], len);
            }else if(address.to_branch == GET_THIS_BRANCH(net->node)) {
                // if the message is to a child of this branch
                QueueMsgToChild(net, address.to_leaf-1, &msg[0], len);
            }else {
                // otherwise send the message to our parent
                QueueMsgToParent(net, &msg[0], len);
            }
            break;
        case ROLE_LEAF:
            QueueMsgToParent(net, &msg[0], len);
            break;
        default:
            LogMsg(net->sys_id, "Corrupt role: %d", net->role);
            break;
    }
}

void nrf24_RegisterControlHandler(enum nrf24_control_msg control_id, nrf24_handler_fn_t fn_ptr) {
    nrf24_RegisterControlHandlerN(&default_net, control_id, fn_ptr);
}
void nrf24_RegisterControlHandlerN(nrfnet_t * net, enum nrf24_control_msg control_id, nrf24_handler_fn_t fn_ptr) {
    if(control_id >= LAST_CONTROL_ID) {
        LogMsg(net->sys_id, "Invalid control_id: %d", control_id);
    }else net->control_handler[control_id] = fn_ptr;
}

void nrf24_RegisterSensorHandler(enum nrf24_sensor_msg sensor_id, nrf24_handler_fn_t fn_ptr) {
    nrf24_RegisterSensorHandlerN(&default_net, sensor_id, fn_ptr);
}
void nrf24_RegisterSensorHandlerN(nrfnet_t * net, enum nrf24_sensor_msg sensor_id, nrf24_handler_fn_t fn_ptr) {
    if(sensor_id >= LAST_SENSOR_ID) {
        LogMsg(net->sys_id, "Invalid sensor_id: %d", sensor_id);
    }else net->sensor_handler[sensor_id] = fn_ptr;
}

void ConfigureNetwork(nrfnet_t * net) {
    if(net->node == MASTER) net->role = ROLE_MASTER;
    else if((net->node & 0x07) == 0) net->role = ROLE_BRANCH;
    else net->role = ROLE_LEAF;
    
    if(net->role == ROLE_MASTER) {
        nRF24_SetChannel(&net->radio, NRF24_CHANNEL_BRANCH_1);
        // open TX pipe for branch 1
        nRF24_OpenTxPipe(&net->radio, (NRF24_ADDRESS_MASK | 0x08));
        net->current_child = 0;
        // start communication by sending an empty packet to branch 1
        TxEmptyPacket(net);
    }else {
        nRF24_SetChannel(&net->radio, net->channel[GET_THIS_BRANCH(net->node)-1]);
        nRF24_OpenRxPipe(&net->radio, NRFNET_RX_PIPE, (NRF24_ADDRESS_MASK | net->node));
        nRF24_StartListening(&net->radio);
    }
}

void ProcessReceivedMsg(nrfnet_t * net, uint8_t * data, uint8_t length) {
    //TODO add support for multiple messages in a single packet by putting a wrapper around this functionality
    //if multi message packet call the below functionality for each sub-message
    //and update the address, length, and data pointer until no messages remain
    nrfnet_address_t address;
    address.b[1] = *data;
    address.b[0] = *(data+1);
    
    switch(net->role) {
        case ROLE_MASTER:
            // check if we are the destination or if the message is to everyone
            if(address.to == ROLE_MASTER || address.to == ALL_ALL) {
                ProcessMessage(net, data, length);
                // if message is going to ALL_ALL
                if(address.to == ALL_ALL) {
                    // queue to send to all other children
                    volatile uint8_t i;
                    for(i = 1; i < 6; i++) {
                        // skip sending to who it was from
                        if(i == address.from_branch) continue;
                        QueueMsgToChild(net, i-1, data, length);
                    }
                }
            }else {
                // if not destination then queue to send to child
                QueueMsgToChild(net, address.to_branch - 1, data, length);
            }
            break;
        case ROLE_BRANCH:
            // check if we are the destination or if the message is to everyone
            if(address.to == net->node || address.to == ALL_ALL) {
                ProcessMessage(net, data, length);
                // if message is going to ALL_ALL
                if(address.to == ALL_ALL) {
                    // if it is from a child then queue to send to parent and other children
                    if(GET_THIS_BRANCH(net->node) == address.from_branch) {
                        // queue to send to all other children
                        volatile uint8_t i;
                        for(i = 1; i < 6; i++) {
                            // skip sending to who it was from
                            if(i == address.from_leaf) continue;
                            QueueMsgToChild(net, i-1, data, length);
                        }
                        QueueMsgToParent(net, data, length);
                    }else {
                        // if it is from the parent then queue to send to all children
                        volatile uint8_t i;
                        for(i = 1; i < 6; i++) {
                            QueueMsgToChild(net, i-1, data, length);
                        }
                    }
                }
            }else { // if not to us and not to ALL_ALL then forward
                // if destination is not within our branch then queue to send to parent
                if(address.to_branch != GET_THIS_BRANCH(net->node)) {
                    QueueMsgToParent(net, data, length);
                }else {
                    // if below then queue to send to child
                    QueueMsgToChild(net, address.to_leaf - 1, data, length);
                }
            }
            break;
        case ROLE_LEAF:
            // the message should be to me, double check and process the message
            if(address.to == net->node || address.to == ALL_ALL) {
                ProcessMessage(net, data, length);
            }else {
                LogMsg(net->sys_id, "RxMsg not to me? me: %d, to: %d, from: %d", net->node, address.to, address.from);
            }
            break;
        default:
            LogMsg(net->sys_id, "Corrupt role: %d", net->role);
            break;
    }
}

void ProcessMessage(nrfnet_t * net, uint8_t * data, uint8_t length) {
    nrfnet_address_t address;
    address.b[1] = *data++;
    address.b[0] = *data++;
    uint8_t id;
    id = *data++;
    if(id) LogMsg(net->sys_id, "msg rx %d", id);
    if(id < LAST_MSG_ID) {
        if(net->handler[id]) {
            if(id <= CHAT_MSG) {
                void (*handler_ptr)(nrfnet_t *, uint8_t *, uint8_t, uint8_t);
                handler_ptr = (void (*)(nrfnet_t *, uint8_t *, uint8_t, uint8_t))net->handler[id];
                handler_ptr(net, data, length-3, address.from);
            }else net->handler[id](data, length-3, address.from);
        }else LogMsg(net->sys_id, "No handler set for message ID: %d", id);
    }else LogMsg(net->sys_id, "invalid message ID: %d", id);
}

void NetworkTick(nrfnet_t * net) {
    switch(net->role) {
        case ROLE_MASTER:
            // if we are waiting on the window time then check and send if time is up
            if(net->state == NRFNET_WAITING_FOR_MIN_WINDOW) {
                if(TimeSince(net->switch_to_tx_time) > NRF24_MIN_WINDOW_MS) {
                    nrfnet_msg_t * msg;
                    net->state = NRFNET_NORMAL_STATE;
                    nRF24_SetChannel(&net->radio, net->channel[net->current_child]);
                    nRF24_OpenTxPipe(&net->radio, (NRF24_ADDRESS_MASK | ((net->current_child+1)<<3)));
                    msg = GetMsgForChild(net, net->current_child);
                    if(msg) {
                        nRF24_Write(&net->radio, &msg->data[0], msg->length);
                        FreeMsg(msg);
                        LogMsg(net->sys_id, "msg tx0 %d", msg->data[2]);
                    }else {
                        TxEmptyPacket(net);
                    }
                }
            }else if(TimeSince(net->switch_to_tx_time) > NRF24_MIN_WINDOW_MS*12) {
                LogMsg(net->sys_id, "timeout branch %d, flushing TX", net->current_child);
                nRF24_FlushTx(&net->radio);
                TxToNextChild(net);
            }
            break;
        case ROLE_BRANCH:
            break;
        case ROLE_LEAF:
            break;
        default:
            break;
    }
}

static void SubsysCallback(int argc, char *argv[]) {
    uint8_t address;
    if (strcasecmp(argv[0], "ping") == 0) {
        if(--argc) {
            address = AddressFromName(argv[1]);
            if(address != 1) {
                nrf24_Ping(address);
            }
        }
    }else if(strcasecmp(argv[0], "spam") == 0) {
        if(--argc) {
            address = AddressFromName(argv[1]);
            if(address != 1) {
                *(argv[2]-2) = SPAM_MSG;
                *(argv[2]-1) = 255;
                uint8_t len = 0;
                while(*(argv[2]+len)) len++;
                nrf24_SendMsgN(&default_net, address, SYSTEM_MSG, (uint8_t*)argv[2]-2, len+2);
            }
        }
    }else if(strcasecmp(argv[0], "who") == 0) {
        UART_printf(SUBSYS_UART, "You are %s\r\n", NameFromAddress(default_net.node));
    }
}

uint32_t ping_time;
static void SystemHandler(nrfnet_t * net, uint8_t * data, uint8_t len, uint8_t from) {
    // System handler to be implemented in future
    // Most system features are just hard coded for now
    enum nrf24_system_msg msg;
    uint8_t i, n;
    msg = (enum nrf24_system_msg)*data++;
    switch(msg) {
        case NULL_MSG:
            break;
        case PING_MSG:
            *data = PING_RESPONSE_MSG;
            nrf24_SendMsgN(net, from, SYSTEM_MSG, data, len);
            break;
        case PING_RESPONSE_MSG:
            UART_printf(SUBSYS_UART, "time: %dms\r\n", TimeSince(ping_time));
            break;
        case SPAM_MSG:
            n = *data++;
            for(i = 0; i < n; i++) {
                UART_Write(SUBSYS_UART, data, len-2);
            }
            break;
    }
}

void nrf24_Ping(uint8_t to) {
    uint8_t data[28];
    data[0] = PING_MSG;
    ping_time = TimeNow();
    nrf24_SendMsgN(&default_net, to, SYSTEM_MSG, &data[0], 28);
    UART_printf(SUBSYS_UART, "32 bytes sent: ");
}

static void ControlHandler(nrfnet_t * net, uint8_t * data, uint8_t len, uint8_t from) {
    uint8_t id;
    id = *data++;
    if(id < LAST_CONTROL_ID) {
        if(net->control_handler[id]) {
            net->control_handler[id](data, len-1, from);
        }else LogMsg(net->sys_id, "No control handler set for control ID: %d", id);
    }else LogMsg(net->sys_id, "invalid control ID: %d", id);
}

static void SensorHandler(nrfnet_t * net, uint8_t * data, uint8_t len, uint8_t from) {
    uint8_t id;
    id = *data++;
    if(id < LAST_SENSOR_ID) {
        if(net->sensor_handler[id]) {
            net->sensor_handler[id](data, len-1, from);
        }else LogMsg(net->sys_id, "No sensor handler set for sensor ID: %d", id);
    }else LogMsg(net->sys_id, "invalid sensor ID: %d", id);    
}
char * NameFromAddress(uint8_t address) {
	uint8_t index;
	index = ADDRESS_TO_INDEX(address);
	if(index >= LAST_ADDRESS) return "crap";
	return &names[index][0];
}
#include <string.h>
uint8_t AddressFromName(char * name) {
	uint8_t i;
	for(i = 0; i < ADDRESS_TO_INDEX(LAST_ADDRESS); i++) {
		if(strncmp(name, &names[i][0], 4) == 0) {
			return INDEX_TO_ADDRESS(i);
		}
	}
	return 1;
}

void PrintNames(void) {
	uint8_t i;
	for(i = 0; i < ADDRESS_TO_INDEX(LAST_ADDRESS); i++) {
		UART_printf(SUBSYS_UART, "%s\r\n", &names[i][0]);
	}
}

static void ChatHandler(nrfnet_t * net, uint8_t * data, uint8_t len, uint8_t from) {
    // to be implemented by Jon W.
	UART_printf(SUBSYS_UART, "%s: %s", NameFromAddress(from), data);
}

static void UART_Receiver(uint8_t c) {
	static uint8_t state = 0;
	static uint8_t address;
	static char name[4];
	static uint8_t data[30];
	static uint8_t length;
	switch (state) {
		case 0:
			if(c == '#') {
				state = 1;
				length = 0;
			}
			break;
		case 1:
			if(c >= '0' && c <= '~') {
				name[length++] = c;
				if(length == 4) {
					address = AddressFromName(&name[0]);
					if(address == 1) state = 0;
					else state = 2;
					length = 0;
				}
			}else state = 0;
			break;
		case 2:
			data[length++] = c;
			// if carriage return then send message
			if(c == '\r') {
				data[length++] = c;
				data[length++] = '\n';
				data[length++] = 0;
				nrf24_SendMsg(address, CHAT_MSG, &data[0], length);
				state = 0;
			}
			// if message is full then send message
			if(length >= 26) {
				data[length++] = 0;
				nrf24_SendMsg(address, CHAT_MSG, &data[0], 27);
				length = 0;
			}
			break;
		default:
			state = 0;
			break;
	}
}

//////////// nRF24 Layer CALLBACKS ////////////////////////////////////////////
void TxAckCallback(void) { TxAckCallbackN(&default_net); }
void TxAckCallback2(void) { TxAckCallbackN(second_net_ptr); }
void TxAckCallbackN(nrfnet_t * net) {
    // if we get an ack then the pipe must be active
    net->child_status[net->current_child] = CHILD_ACTIVE;
    // stamp ACK time for this pipe
    net->child_time[net->current_child] = TimeNow();
    TxToNextChild(net);
}

void MaxRetriesCallback(void) { MaxRetriesCallbackN(&default_net); }
void MaxRetriesCallback2(void) { MaxRetriesCallbackN(second_net_ptr); }
void MaxRetriesCallbackN(nrfnet_t * net) {
    // set the pipe status to missing so we don't keep sending data to it
    // the missing timeout with determine how long before we try again on this pipe
    net->child_status[net->current_child] = CHILD_MISSING;
    TxToNextChild(net);
}

void TxToNextChild(nrfnet_t * net) {
    uint8_t i;
    switch(net->role) {
        case ROLE_MASTER:
            // set pipe to next pipe
            net->current_child = (net->current_child >= 5) ? 0 : (net->current_child + 1);
            // check if the pipe is missing
            i = 1;
            while(net->child_status[net->current_child] == CHILD_MISSING) {
                // if the timeout has been reached then try the missing pipe again
            	if(TimeSince(net->child_time[net->current_child]) > NRF24_MISSING_NODE_TIMEOUT){
					net->child_time[net->current_child] = TimeNow();
					break;
				}
                // otherwise go to the next pipe and check again
                net->current_child = (net->current_child >= 5) ? 0 : (net->current_child + 1);
                // if all pipes missing then just use the next pipe
                if(++i > 6) break;
            }
            // check if the minimum window has elapsed
            if(TimeSince(net->child_time[net->current_child]) > NRF24_MIN_WINDOW_MS) {
                nrfnet_msg_t * msg;
                nRF24_SetChannel(&net->radio, net->channel[net->current_child]);
                nRF24_OpenTxPipe(&net->radio, (NRF24_ADDRESS_MASK | ((net->current_child+1)<<3)));
                msg = GetMsgForChild(net, net->current_child);
                if(msg) {
                    nRF24_Write(&net->radio, &msg->data[0], msg->length);
                    FreeMsg(msg);
                    LogMsg(net->sys_id, "msg tx1 %d", msg->data[2]);
                }else {
                    TxEmptyPacket(net);
                }
            }else net->state = NRFNET_WAITING_FOR_MIN_WINDOW;
            net->switch_to_tx_time = TimeNow(); // in master mode use this to know when we last sent to the new pipe
            break;
        case ROLE_BRANCH:
            // set pipe to next pipe
            net->current_child = (net->current_child >= 5) ? 0 : (net->current_child + 1);
            // check if the pipe is missing
            i = 1;
            while(net->child_status[net->current_child] == CHILD_MISSING) {
                // if the timeout has been reached then try the missing pipe again
                if(TimeSince(net->child_time[net->current_child]) > NRF24_MISSING_NODE_TIMEOUT){
                	net->child_time[net->current_child] = TimeNow();
                	break;
                }
                // otherwise go to the next pipe and check again
                net->current_child = (net->current_child >= 5) ? 0 : (net->current_child + 1);
                // if all pipes missing then just give up
                if(++i > 6) {
                	nRF24_StartListening(&net->radio);
                    CheckAckMessage(net);
                    return;
                }
            }
            //check if there is enough time remaining and scan the next child
            if(TimeSince(net->switch_to_tx_time) < NRF24_BRANCH_WINDOW_MS) {
                nrfnet_msg_t * msg;
                nRF24_OpenTxPipe(&net->radio, (NRF24_ADDRESS_MASK | ((net->current_child+1) | net->node)));
                msg = GetMsgForChild(net, net->current_child);
                if(msg) {
                    nRF24_Write(&net->radio, &msg->data[0], msg->length);
                    FreeMsg(msg);
                    LogMsg(net->sys_id, "msg tx2 %d", msg->data[2]);
                }else {
                    TxEmptyPacket(net);
                }
            }else {
            	 nRF24_StartListening(&net->radio);
            	 CheckAckMessage(net);
            }
            break;
        case ROLE_LEAF:
            // this should not happen since a leaf never enters primary transmit mode (PTX))
            break;
        default:
            break;
    }
}

void TxAckPayloadCallback(uint8_t * data, uint8_t length) {
    ProcessReceivedMsg(&default_net, data, length);
}
void TxAckPayloadCallback2(uint8_t * data, uint8_t length) {
    ProcessReceivedMsg(second_net_ptr, data, length);
}

void ProcessPayloadCallback(uint8_t * data, uint8_t length) {
    ProcessPayloadCallbackN(&default_net, data, length);
}
void ProcessPayloadCallback2(uint8_t * data, uint8_t length) {
    ProcessPayloadCallbackN(second_net_ptr, data, length);
}
void ProcessPayloadCallbackN(nrfnet_t * net, uint8_t * data, uint8_t length) {
	if(length == 0) {
		LogMsg(net->sys_id, "ProcessPayload Length 0");
		return;
	}
    if(net->role == ROLE_BRANCH) {
        uint8_t i;
        //initiate scan through children
        // set pipe to next pipe
        net->current_child = (net->current_child >= 5) ? 0 : (net->current_child + 1);
        // check if the pipe is missing
        i = 1;
        while(net->child_status[net->current_child] == CHILD_MISSING) {
            // if the timeout has been reached then try the missing pipe again
        	if(TimeSince(net->child_time[net->current_child]) > NRF24_MISSING_NODE_TIMEOUT){
				net->child_time[net->current_child] = TimeNow();
				break;
			}
            // otherwise go to the next pipe and check again
            net->current_child = (net->current_child >= 5) ? 0 : (net->current_child + 1);
            // if all pipes missing then just give up
            if(++i > 6) {
            	ProcessReceivedMsg(net, data, length);
            	CheckAckMessage(net);
                return;
            }
        }
        nrfnet_msg_t * msg;
        net->switch_to_tx_time = TimeNow(); // stamp the time we are going into TX mode
        nRF24_OpenTxPipe(&net->radio, (NRF24_ADDRESS_MASK | ((net->current_child+1) | net->node )));
        msg = GetMsgForChild(net, net->current_child);
        if(msg) {
            nRF24_Write(&net->radio, &msg->data[0], msg->length);
            FreeMsg(msg);
            LogMsg(net->sys_id, "msg tx3 %d", msg->data[2]);
        }else {
            TxEmptyPacket(net);
        }
    }
    ProcessReceivedMsg(net, data, length);
}

void AckPayloadSentCallback(void) { AckPayloadSentCallbackN(&default_net); }
void AckPayloadSentCallback2(void) { AckPayloadSentCallbackN(second_net_ptr); }
void AckPayloadSentCallbackN(nrfnet_t * net) {
    if(net->role == ROLE_LEAF) {
        nrfnet_msg_t * msg;
        // check if payload is available to add for ACK
        msg = GetMsgForChild(net, NRFNET_RX_PIPE);
        if(msg) {
            nRF24_WriteAck(&net->radio, &msg->data[0], msg->length, NRFNET_RX_PIPE);
            nrfnet_address_t address;
            address.b[1] = msg->data[0];
            address.b[0] = msg->data[1];
            LogMsg(net->sys_id, "AckPayWr1, to:%d%d, from:%d%d", address.to_branch, address.to_leaf, address.from_branch, address.from_leaf);
            FreeMsg(msg);
        }
    }
}

void QueueMsgToChild(nrfnet_t * net, uint8_t child, uint8_t * data, uint8_t length) {
    AddDataForPipe(net, child, data, length);
}

void QueueMsgToParent(nrfnet_t * net, uint8_t * data, uint8_t length) {
    switch(net->role) {
        case ROLE_MASTER:
            break;
        case ROLE_BRANCH:
            AddDataForPipe(net, NRFNET_PARENT_PIPE, data, length);
            break;
        case ROLE_LEAF:
            //TODO make sure FIFO isn't full
            nRF24_WriteAck(&net->radio, data, length, NRFNET_RX_PIPE);
            nrfnet_address_t address;
			address.b[1] = *data;
			address.b[0] = *(data+1);
			LogMsg(net->sys_id, "AckPayWr2, to:%d%d, from:%d%d", address.to_branch, address.to_leaf, address.from_branch, address.from_leaf);
    }
}

void CheckAckMessage(nrfnet_t * net) {
    nrfnet_msg_t * msg;
    // check if payload is available to add for ACK
    msg = GetMsgForChild(net, NRFNET_PARENT_PIPE);
    if(msg) {
        nRF24_WriteAck(&net->radio, &msg->data[0], msg->length, NRFNET_RX_PIPE);
        nrfnet_address_t address;
		address.b[1] = msg->data[0];
		address.b[0] = msg->data[1];
		if(address.from != net->node){
			Nop();
		}
		LogMsg(net->sys_id, "AckPayWr3, to:%d%d, from:%d%d", address.to_branch, address.to_leaf, address.from_branch, address.from_leaf);
        FreeMsg(msg);
    }
}

void TxEmptyPacket(nrfnet_t * net) {
    uint8_t data[4];
    nrfnet_address_t address;
    address.word = 0;
    address.from = net->node;
    if(net->role == ROLE_BRANCH) {
        address.to = net->node;
        address.to_leaf = net->current_child+1;
    }else {
        address.to_branch = net->current_child+1;
    }
    data[0] = address.b[1];
    data[1] = address.b[0];
    data[2] = SYSTEM_MSG;
    data[3] = NULL_MSG;
    nRF24_Write(&net->radio, &data[0], 4);
}

// buffer helpers
nrfnet_msg_t * GetMsgForChild(nrfnet_t * net, uint8_t pipe) {
    volatile uint8_t i;
    uint8_t age = 0;
    uint8_t index = 0xFF;
    // find the oldest message for this pipe
    for(i = 0; i < NRFNET_MSG_BUFFER_LENGTH; i++) {
        if(pipe == net->msg_buf[i].child) {
            if(net->msg_buf[i].age >= age) {
                age = net->msg_buf[i].age;
                index = i;
            }
        }
    }
    if(index == 0xFF) return 0;
    else return &net->msg_buf[index];
}

void AddDataForPipe(nrfnet_t * net, uint8_t pipe, uint8_t * data, uint8_t length) {
    volatile uint8_t i;
    //TODO remove this diagnostic trash
    nrfnet_address_t address;
    address.b[1] = *data;
    address.b[0] = *(data+1);
    if(address.from != net->node) {
    	LogMsg(net->sys_id, "From:%d,Node:%d", address.from, net->node);
    }
    // see if there is room in any existing messages
    for(i = 0; i < NRFNET_MSG_BUFFER_LENGTH; i++) {
        if(pipe == net->msg_buf[i].child) {
            //TODO handle adding data to existing messages
            // increment the age of existing messages for this pipe
            net->msg_buf[i].age++;
        }
    }
    // if data could not be added to existing message then find an empty one
    for(i = 0; i < NRFNET_MSG_BUFFER_LENGTH; i++) {
        if(net->msg_buf[i].child == 0xFF) {
            net->msg_buf[i].child = pipe;
            net->msg_buf[i].age = 0;
            net->msg_buf[i].length = length;
            uint8_t j;
            for(j = 0; j < length; j++) net->msg_buf[i].data[j] = *data++;
            //memcpy(net->msg_buf[i].data, data, length);
            return;
        }
    }
    LogMsg(net->sys_id, "BufferOverflow, child: %d", pipe);
}

void FreeMsg(nrfnet_msg_t * msg) {
    msg->child = 0xFF;
    msg->age = 0;
}

void nrf24_NetworkISRHandler(void){
	nRF24_ISR(&default_net.radio);
}

// possible additions:
// - add method to send a control message
// - add method to request sensor data
// - add method to support automatic response of sensor data request
