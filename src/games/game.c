#include <strings.h>

#include "project_settings.h"
#include "game.h"
#include "uart.h"
#include "subsystem.h"
#include "timing.h"
#ifdef USE_MODULE_GAME_CONTROLLER
#include "game_controller.h"
#include "game_controller_host.h"

// button press callback functions for controller input
static void StartOrA_Pressed(controller_buttons_t buttons, void * ptr);
static void B_Pressed(controller_buttons_t buttons, void * ptr);
static void D_PadPressed(controller_buttons_t buttons, void * ptr);

static int16_t game_selected = -1;
#endif

#ifndef PLAYER1_UART
#ifndef SUBSYSTEM_UART
#error "You must define what UART number the terminal is using in project_settings.h: #define SUBSYS_UART num"
#endif
#define PLAYER1_UART SUBSYSTEM_UART
#endif
// if you try to use a 2 player game without PLAYER2_UART defined in project_settings.h
// then just double print to player1's terminal
#ifndef PLAYER2_UART
#define PLAYER2_UART PLAYER1_UART
#endif

#define MAX_GAMES 30

/// game structure
typedef struct {
    void (*callback)(int argc, char *argv[]); ///< callback (optional)
    void (*play)(void); ///< funtion to initiate playing the game
    void (*help)(void); ///< function to display help info
    void (*score)(void); ///< function to display scores (optional)
    char * name; ///< name of the game
    char * description; ///< description of the game
    uint8_t num_players; ///< number of players (1 by default)
} game_t;

game_t games[MAX_GAMES];

#ifdef USE_MODULE_GAME_NFR24
struct {
    void (*network_input)(game_network_payload_t *);
    uint8_t (*host_packetizer)(uint8_t *);
    uint8_t channel;
    nrf24_t * nrf_ptr;
    uint8_t clients;
    uint8_t rx_from;
    uint8_t ack_rx;
    uint8_t next_client;
    uint8_t length;
    game_network_payload_t payload;
    uint64_t address;
    uint32_t last_time;
    controller_buttons_t buttons;
    uint8_t host_flg;
    uint8_t start_flg;
    uint8_t game_id;
} game_network;

struct {
    uint8_t type;
    uint8_t index;
    controller_buttons_t data;
} game_network_controller_msg;

enum {
    GAME_NRF_MSG_HOST_PAYLOAD = 0x01,
    GAME_NRF_MSG_CONTROLLER_PAYLOAD = 0x02,
	GAME_NRF_MSG_PLAY_GAME = 0x03,
	GAME_NRF_MSG_END_GAME = 0x04,
    GAME_NRF_MSG_JOIN_REQUEST = 0x55,
    GAME_NRF_MSG_JOIN_RESPONSE = 0x66,
};

#define GAME_NRF_MAX_CLIENTS 3
#define GAME_NRF_TIMEOUT 1000

static void ControllerTransmitter(void);
static void ControllerCallback(controller_buttons_t btn, void * ptr);
static void Game_AllStart(game_network_payload_t * input);
#endif
    
uint8_t game_last_index = 0xFF;
uint8_t game_id;
uint8_t playing_id = 0;

// callback for subsystem module
static void Game_Callback(int argc, char *argv[]);
// play a game
// this will remove the controller callbacks if a controller is being used
static void Play(void (*play)(void));

void Game_Init(void) {
    static uint8_t init_flag = 0;
    if (init_flag) return;
    init_flag = 1;
    version_t game_version;
    game_version.word = 0x01010001U;
    game_id = Subsystem_Init("game", game_version, Game_Callback);
#ifdef USE_MODULE_GAME_CONTROLLER
    // if the controller module exists then add button callbacks for selecting
    // games (this is done in Game_GameOver() so just call that)
    Game_GameOver();
    LogMsg(game_id, "Game Management Module Initialize, press Start or A to select a game");
#endif
}

uint8_t Game_Register(char * name, char * description,
        void (*play)(void),
        void (*help)(void)) {
    game_t * g;
    Game_Init();
    game_last_index++;
    if (game_last_index >= MAX_GAMES) return 0xFF;
    g = &games[game_last_index];
    g->help = help;
    g->play = play;
    g->name = name;
    g->description = description;
    // default num_players is 1, score callback is null, callback is null
    g->num_players = 1;
    g->score = 0;
    g->callback = 0;
    return game_last_index;
}

void Game_RegisterCallback(uint8_t game_id,
        void (*callback)(int argc, char *argv[])) {
    if(game_id >= MAX_GAMES) return; // prevent out of bounds access
    games[game_id].callback = callback;
}

void Game_RegisterHighscoreCallback(uint8_t game_id,
        void (*callback)(void)) {
    if(game_id >= MAX_GAMES) return; // prevent out of bounds access
    games[game_id].score = callback;
}

void Game_Log(uint8_t id, char * str, ...) {
    Log_Header(id, LOG_MESSAGE);
    LogStr(" %s ", games[id].name);
    va_list vars;
    va_start(vars, str);
    UART_vprintf(SUBSYSTEM_UART, str, vars);
    va_end(vars);
}

void Game_EnableMultiPlayer(uint8_t id, uint8_t num) {
    if(id >= MAX_GAMES) return; // prevent out of bounds access
    games[id].num_players = num;
}

void Game_RegisterPlayer1Receiver(void(*rx)(uint8_t)) {
    UART_RegisterReceiver(PLAYER1_UART, rx);
}

void Game_RegisterPlayer2Receiver(void(*rx)(uint8_t)) {
    UART_RegisterReceiver(PLAYER2_UART, rx);
}

void Game_UnregisterPlayer1Receiver(void(*rx)(uint8_t)) {
    UART_UnregisterReceiver(PLAYER1_UART, rx);
}

void Game_UnregisterPlayer2Receiver(void(*rx)(uint8_t)) {
    UART_UnregisterReceiver(PLAYER2_UART, rx);
}

void Game_Callback(int argc, char *argv[]) {
    volatile uint8_t i;
    if (argc) {
        // game management commands are "help" "list" "player1" "player2"
        if (strcasecmp(argv[0], "help") == 0) {
            Game_Printf("Type '$game list' for a list of games\r\n"
                    "type '$game name play' to play\r\n"
                    "type '$game name help' for help with that game\r\n");
            return;
        }
        if (strcasecmp(argv[0], "list") == 0) {
            if(game_last_index == 0xFF) Game_Printf("No games registered\r\n");
            else {
                Game_Printf("id: name - description\r\n");
                for (i = 0; i <= game_last_index; i++) {
                    Game_Printf("%d: %s - %s\r\n", i, games[i].name, games[i].description);
                }
            }
            return;
        }
#ifdef USE_MODULE_GAME_NFR24
        if(strcasecmp(argv[0],"host") == 0) {
            if(argc == 2) {
                game_network.host_flg = 0x01;
                Game_Host(ArgToU8(argv[1]));
            }
        }
        if(strcasecmp(argv[0],"join") == 0) {
            if(argc == 2) {
                game_network.host_flg = 0x10;
                Game_Join(ArgToU8(argv[1]));
            }
        }
        if(strcasecmp(argv[0],"ec") == 0) {
        	GameControllerHost_EnableTerminalController(PLAYER1_UART);
        }
#endif
        if (strcasecmp(argv[0], "player1") == 0) {
            Game_Printf("command yet to be implemented\r\n");
            return;
        }
        if (strcasecmp(argv[0], "player2") == 0) {
            Game_Printf("command yet to be implemented\r\n");
            return;
        }
        if(game_last_index == 0xFF) return;
        for (i = 0; i <= game_last_index; i++) {
            if (strcasecmp(argv[0], games[i].name) == 0) {
                if (argc >= 2) {
                    if (strcasecmp(argv[1], "play") == 0) {
                        playing_id = i;
#ifndef USE_MODULE_GAME_NFR24
                        Play(games[i].play);
#else
                        if(game_network.host_flg == 0x01)
                        {
                           game_network.start_flg = 1;
                           game_network.game_id = i;
                           
                        }
                        else if(game_network.host_flg == 0x10) {
                        	 Game_Printf("Silly Client, games are for hosts!\r\n");
                        }
                        else
                        {
                            Play(games[i].play);
                        }
#endif
                        return;
                    }
                    if (strcasecmp(argv[1], "help") == 0) {
                        games[i].help();
                        return;
                    }
                    if (strcasecmp(argv[1], "score") == 0) {
                        if (games[i].score) games[i].score();
                        return;
                    }
                    if (argc >= 2)
                    {
                        if (games[i].callback)
                        {
                            games[i].callback(argc - 1, &argv[1]);
                        }
                    }
                }
            }
        }
    }
}

void Game_CharXY(char c, char x, char y) {
    Terminal_CharXY(PLAYER1_UART, c, x, y);
    if (games[playing_id].num_players >= 2)
        Terminal_CharXY(PLAYER2_UART, c, x, y);
}

void Game_Player1CharXY(char c, char x, char y) {
    Terminal_CharXY(PLAYER1_UART, c, x, y);
}

void Game_Player2CharXY(char c, char x, char y) {
    Terminal_CharXY(PLAYER2_UART, c, x, y);
}

void Game_Printf(char * str, ...) {
    // variable argument list type
    va_list vars;
    // initialize the variable argument list pointer by specifying the
    // input argument immediately preceding the variable list
    va_start(vars, str);
    UART_vprintf(PLAYER1_UART, str, vars);
    va_end(vars);
    if(games[playing_id].num_players >= 2) {
        va_start(vars, str);
        UART_vprintf(PLAYER2_UART, str, vars);
        va_end(vars);
    }
}

void Game_Player1Printf(char * str, ...) {
    va_list vars;
    va_start(vars, str);
    UART_vprintf(PLAYER1_UART, str, vars);
    va_end(vars);
}

void Game_Player2Printf(char * str, ...) {
    va_list vars;
    va_start(vars, str);
    UART_vprintf(PLAYER2_UART, str, vars);
    va_end(vars);
}

static void Cursor(uint8_t show) {
    static uint8_t echo = 0;
    if(show == 0) {
        Terminal_HideCursor(PLAYER1_UART);
        if(games[playing_id].num_players >= 2) Terminal_HideCursor(PLAYER2_UART);
        echo = Log_GetEcho();
        Log_EchoOff();
    }else {
        Terminal_ShowCursor(PLAYER1_UART);
        if(games[playing_id].num_players >= 2) Terminal_ShowCursor(PLAYER2_UART);
        if(echo) Log_EchoOn();
    }
}

void Game_HideCursor(void) {
    Cursor(0);
}

void Game_ShowCursor(void) {
    Cursor(1);
}

void Game_SetColor(enum term_color color) {
    Terminal_SetColor(PLAYER1_UART, color);
    if(games[playing_id].num_players >= 2) Terminal_SetColor(PLAYER2_UART, color);
}

void Game_DrawTile(char *tile[], char x, char y) {
    // support for "transparent" characters not implemented yet
    volatile uint8_t i = 0;
    while(tile[i]) {
        Game_CharXY(tile[i][0], x, y++);
        Game_Printf(tile[i]+1);
        i++;
    }
}

void Game_ClearScreen(void) {
    Game_Printf("\f");
}

void Game_DrawRect(char x_min, char y_min, char x_max, char y_max) {
    char x, y;
    Game_CharXY(201, x_min, y_min); //
    for (x = x_min + 1; x < x_max; x++) Game_CharXY(205, x, y_min); //
    Game_CharXY(187, x_max, y_min); //
    for (y = y_min + 1; y < y_max; y++) Game_CharXY(186, x_max, y); //
    // wait to transmit all the chars before continuing
    // (this could be removed if the TX buffer size is increased)
    // another way around this would be to schedule the rest of the printout
    // to happen several ms from now
    while (Game_IsTransmitting()) DelayMs(2);
    Game_CharXY(188, x_max, y_max);
    for (x = x_min + 1; x < x_max; x++) Game_CharXY(205, x, y_max); //
    Game_CharXY(200, x_min, y_max); //
    for (y = y_min + 1; y < y_max; y++) Game_CharXY(186, x_min, y); //
}

void Game_FillRect(char c, char x_min, char y_min, char x_max, char y_max) {
    volatile uint8_t x, y;
    for(y = y_min; y < y_max; y++) {
        Game_CharXY(c, x_min, y);
        for(x = x_min + 1; x < x_max; x++) {
            UART_WriteByte(PLAYER1_UART, c);
            if(games[playing_id].num_players >= 2) UART_WriteByte(PLAYER2_UART, c);
        }
    }
}

void Game_ScrollDown(void) {
    Game_Printf("%cD", ASCII_ESC);
}

void Game_ScrollUp(void) {
    Game_Printf("%cM", ASCII_ESC);
}

void Game_Bell(void) {
    Game_Printf("\a");
}

uint8_t Game_IsTransmitting(void) {
    uint8_t ret;
    ret = UART_IsTransmitting(PLAYER1_UART);
    if(games[playing_id].num_players >= 2) ret += UART_IsTransmitting(PLAYER2_UART);
    return ret;
}

void Game_GameOver(void) {
#ifdef USE_MODULE_GAME_CONTROLLER
    // if the controller module exists then add button callbacks for selecting
    // games
    controller_buttons_t mask;
    mask.all_buttons = 0;
    mask.button.A = 1;
    mask.button.start = 1;
    GameControllerHost_RegisterPressCallback(7, StartOrA_Pressed, mask, 0);
    mask.all_buttons = 0;
    mask.button.B = 1;
    GameControllerHost_RegisterPressCallback(7, B_Pressed, mask, 0);
    mask.all_buttons = 0;
    mask.button.up = 1;
    mask.button.down = 1;
    mask.button.left = 1;
    mask.button.right = 1;
    GameControllerHost_RegisterPressCallback(7, D_PadPressed, mask, 0);
    // set the game_selected to -1 so we know nothing is selected yet
    game_selected = -1;
#endif
}


static void Play(void (*play)(void)) {
#ifdef USE_MODULE_GAME_CONTROLLER
    // remove controller callbacks so they don't get called while the user is
    // playing the game
    GameControllerHost_RemoveCallback(StartOrA_Pressed, 0);
    GameControllerHost_RemoveCallback(D_PadPressed, 0);
    GameControllerHost_RemoveCallback(B_Pressed, 0);
#endif
    // call the play routine that was passed to us
    play();
}

#ifdef USE_MODULE_GAME_CONTROLLER

static void DisplayGameMenu(void) {
    uint8_t i;
    Game_ClearScreen();
    Game_Printf("Welcome to the embedded game arcade!\r\n");
    if(game_last_index == 0xFF) {
        Game_Printf("Sorry, there are no games to select from");
    }else {
        Game_Printf("Use the D-Pad to select a game, Start/A to play, B for help\r\n");
        for(i = 0; i <= game_last_index; i++) {
            Game_Printf(" %d: %s - %s\r\n", i, games[i].name, games[i].description);
        }
        Game_Printf(" Exit game selection");
    }
}

static void StartOrA_Pressed(controller_buttons_t buttons, void * ptr) {
    if(game_selected == -1) {
        game_selected = 0;
        DisplayGameMenu();
        // select game 0
        Game_CharXY('>', 0, 2);
    }else {
        if(game_selected <= game_last_index) {
#ifdef USE_MODULE_GAME_NFR24
            if(game_network.host_flg == 0x01)
            {
               game_network.start_flg = 1;
               game_network.game_id = game_selected;

            }
            else if(game_network.host_flg == 0x10) {
                Game_CharXY(' ',0,game_last_index + 3);
                Game_Printf("Silly Client, games are for hosts\r\n");
                Game_CharXY('>',0,game_selected + 2);

            }
            else
            {
                Play(games[game_selected].play);
            }
#else
            Play(games[game_selected].play);
#endif
        }else if(game_selected == game_last_index+1) {
            // the user selected exit
            game_selected = -1;
            Game_ClearScreen();
        }
    }
}

static void B_Pressed(controller_buttons_t buttons, void * ptr) {
    static uint8_t in_help = 0;
    if(game_selected >= 0 && game_selected <= game_last_index) {
        if(in_help == 0 && games[game_selected].help) {
            // clear the screen and show the help
            Game_ClearScreen();
            Game_Printf("< Help for '%s', press B to go back >\r\n\n", games[game_selected].name);
            games[game_selected].help();
            in_help = 1;
        }else if(in_help == 1) {
            DisplayGameMenu();
            // select game 0
            Game_CharXY('>', 0, 2+game_selected);
            in_help = 0;
        }
    }
}

static void D_PadPressed(controller_buttons_t buttons, void * ptr) {
    if(game_selected>=0) {
        if(buttons.button.up || buttons.button.left) {
            if(game_selected > 0) {
                Game_CharXY(' ', 0, 2+game_selected);
                game_selected--;
                Game_CharXY('>', 0, 2+game_selected);
            }
        }
        if(buttons.button.down || buttons.button.right) {
            if(game_selected <= game_last_index) {
                Game_CharXY(' ', 0, 2+game_selected);
                game_selected++;
                Game_CharXY('>', 0, 2+game_selected);
            }
        }
    }
}
#endif

#ifdef USE_MODULE_GAME_NFR24
#include "stddef.h"
#include "string.h"

void Game_NetworkInit(nrf24_t * nrf_ptr) {
    game_network.nrf_ptr = nrf_ptr;
}

static void HostReceiver(uint8_t * data, uint8_t length) {
    union16_t u16;
    // check for a join message
    switch(data[0]) {
        case GAME_NRF_MSG_JOIN_REQUEST: // join message
            if(game_network.next_client) {
                game_network.clients |= 1 << game_network.next_client;
                Game_Printf("Client %d joined network\r\n",game_network.next_client);
                // if there can be more clients then load the next ack message
                if(game_network.next_client < GAME_NRF_MAX_CLIENTS) {
                    // join ack packet pipe, address offset, delay, retries
                    uint8_t ack[5] = {GAME_NRF_MSG_JOIN_RESPONSE, 1, 10, 1, 8};
                    game_network.next_client++;
                    ack[1] = game_network.next_client;
                    // address is offset by client number * 10
                    ack[2] = game_network.next_client*10;
                    // delay is 1, 3, 5
                    ack[3] = (game_network.next_client-1)*2 + 1;
                    // write the ack for the first person to join
                    nRF24_WriteAck(game_network.nrf_ptr, &ack[0], 5, 4);
                }else game_network.next_client = 0;
            }
            break;
        case GAME_NRF_MSG_CONTROLLER_PAYLOAD: // client controller data
            // receive button data into payload
            u16.b[0] = data[2];
            u16.b[1] = data[3];
            if(game_network.nrf_ptr->last_rx_pipe <= GAME_NRF_MAX_CLIENTS) {
                game_network.payload.controller[game_network.nrf_ptr->last_rx_pipe].all_buttons = u16.word;
                game_network.rx_from |= 1 << game_network.nrf_ptr->last_rx_pipe;
                if((nRF24_ReadReg(game_network.nrf_ptr, STATUS) & TX_FULL) == 0) {
                	nRF24_WriteAck(game_network.nrf_ptr, (uint8_t *)&game_network.payload, game_network.length, game_network.nrf_ptr->last_rx_pipe);
                }
            }else {
                Game_Printf("bad pipe %d\r\n", game_network.nrf_ptr->last_rx_pipe);
            }
            break;
    }
}

static void HostFillAckPayloads(void) {
    static uint8_t index = 0;
    game_network.length = 10;
    uint8_t i;
    if(game_network.clients) {
        // check if we have received from all clients
        if(game_network.rx_from >= game_network.clients) { // put one for gamestart message
        	game_network.rx_from = 0;
        	game_network.last_time = TimeNow();
            if(game_network.start_flg) {  //If the Start Flag is enabled
                game_network.start_flg = 0; //Removes Start Flag
                game_network.payload.type = 0x03; //Makes payload gamestart type
                game_network.payload.index = index++;
                char * ptr;
                ptr = games[game_network.game_id].name; //Pointer to the Game name on the Host System
                memcpy(&game_network.payload.user_data,ptr, strlen(ptr)+1); //Copies the name (including the void at the end)
                game_network.length += strlen(ptr)+1; //Sets the length of the packet to include the string
            }
            else {
                game_network.payload.type = 0x01 ;
                game_network.payload.index = index++;
                if(game_network.host_packetizer) {
                    game_network.length += game_network.host_packetizer(game_network.payload.user_data);
                }
            }
            // load the hosts controller data into the message
            game_network.payload.controller[0] = game_network_controller_msg.data;
            game_network_controller_msg.data = game_network.buttons;
            nRF24_FlushTx(game_network.nrf_ptr);
            for(i = 1; i <= GAME_NRF_MAX_CLIENTS; i++) {
                if(game_network.clients & (1<<i)) {
                    nRF24_WriteAck(game_network.nrf_ptr, (uint8_t *)&game_network.payload, game_network.length, i);
                }else {
                	// join ack packet pipe, address offset, delay, retries
					uint8_t ack[5] = {GAME_NRF_MSG_JOIN_RESPONSE, 1, 10, 1, 8};
					ack[1] = game_network.next_client;
					// address is offset by client number * 10
					ack[2] = game_network.next_client*10;
					// delay is 1, 3, 5
					ack[3] = (game_network.next_client-1)*2 + 1;
					// write the ack for the first person to join
					nRF24_WriteAck(game_network.nrf_ptr, &ack[0], 5, 4);
					break;
                }
            }
            // send payload to host game
            if(game_network.network_input) {
                game_network.network_input(&game_network.payload);
            }
            Task_Schedule(HostFillAckPayloads, 0, GAME_NRF_TX_PERIOD, 0);
        }else {
            Task_Schedule(HostFillAckPayloads, 0, 2, 0);
            if(TimeSince(game_network.last_time) > GAME_NRF_TIMEOUT) {
            	Game_Printf("network timeout, exp: %x, rx: %x", game_network.clients, game_network.rx_from);
            }
            game_network.last_time = TimeNow();
        }
    }else {
        Task_Schedule(HostFillAckPayloads, 0, GAME_NRF_TX_PERIOD, 0);
        // load the hosts controller data into the message
		game_network.payload.controller[0] = game_network_controller_msg.data;
		game_network_controller_msg.data = game_network.buttons;
        // send payload to host game
		if(game_network.network_input) {
			game_network.network_input(&game_network.payload);
		}
    }
}

void Game_Host(uint8_t channel) {
	controller_buttons_t mask;
    // join ack packet pipe, address offset, delay, retries
    uint8_t ack[5] = {GAME_NRF_MSG_JOIN_RESPONSE, 1, 10, 1, 8};
    nRF24_Init(game_network.nrf_ptr);
    nRF24_SetChannel(game_network.nrf_ptr, channel);
    nRF24_OpenRxPipe(game_network.nrf_ptr, 1, RF_ADDRESS+10);
	nRF24_OpenRxPipe(game_network.nrf_ptr, 2, RF_ADDRESS+20);
	nRF24_OpenRxPipe(game_network.nrf_ptr, 3, RF_ADDRESS+30);
    nRF24_OpenRxPipe(game_network.nrf_ptr, 4, RF_ADDRESS);
    nRF24_StartListening(game_network.nrf_ptr);
    game_network.nrf_ptr->ReceivedPayload = HostReceiver;
    // write the ack for the first person to join
    nRF24_WriteAck(game_network.nrf_ptr, &ack[0], 5, 4);
    game_network.clients = 0;
    // remove any old tasks
    Task_Remove(HostFillAckPayloads, 0);
    Task_Remove(ControllerTransmitter, 0);
    Task_Schedule(HostFillAckPayloads, 0, GAME_NRF_TX_PERIOD, 0);
    game_network.next_client = 1;
    DisplayGameMenu();
    mask.all_buttons = 0xFFFF;
    GameControllerHost_RegisterCallback(0, ControllerCallback, mask, 0);
    Game_RegisterInputCallback(Game_AllStart);  //Registers the host All Start command
}

static void ControllerTransmitter(void) {
    static uint8_t index = 0;

    game_network_controller_msg.type = GAME_NRF_MSG_CONTROLLER_PAYLOAD;
    if(game_network.ack_rx == 0) {
        //Game_Printf("missed game packet\r\n");
        if(TimeSince(game_network.last_time) > GAME_NRF_TIMEOUT) {
        	Game_Printf("game network timeout\r\n");
        	game_network.last_time = TimeNow();
        }
    }else {
        game_network_controller_msg.index = index++;
    }
    nRF24_Write(game_network.nrf_ptr, (uint8_t*)&game_network_controller_msg, 4);
    game_network_controller_msg.data = game_network.buttons;
    game_network.ack_rx = 0;
}

static void ClientAckReceiver(uint8_t * data, uint8_t length) {
	static uint8_t last_index = 255;
	game_network.last_time = TimeNow();
    game_network_payload_t payload;
    game_network.ack_rx = 1;
    if(game_network.network_input) {
        // process the game data
        memcpy(&payload, data, length);
        if(last_index != payload.index && payload.type == 1) {
        	game_network.network_input(&payload);
        }  
        if(last_index != payload.index && payload.type == 0x03) { //Enables game start me
        	game_network.network_input(&payload);              //Could probably be combined with previous if statement
        }
        last_index = payload.index;
    }
}

static void ClientEmptyAck(void) {
    // if we get an empty ack try resending to get the data with the index
    if(game_network.ack_rx == 0) {
        nRF24_Write(game_network.nrf_ptr, (uint8_t*)&game_network_controller_msg, 4);
    }
}

static void ClientMaxRetries(void) {
	//nRF24_Init(game_network.nrf_ptr);
    //nRF24_SetChannel(game_network.nrf_ptr, game_network.channel);
	//nRF24_OpenTxPipe(game_network.nrf_ptr, game_network.address);
	//nRF24_SetRetries(game_network.nrf_ptr, (game_network.next_client-1)*2 + 1, 8);
	nRF24_FlushTx(game_network.nrf_ptr);
	//Game_Printf("client max retries\r\n");
    //Task_Remove(ControllerTransmitter, 0);
    //Task_Schedule(ControllerTransmitter, 0, 10, GAME_NRF_TX_PERIOD);
}

static void JoinAckPayload(uint8_t * data, uint8_t length) {
    if(data[0] == GAME_NRF_MSG_JOIN_RESPONSE) {
        game_network.nrf_ptr->AckPayloadReceived = ClientAckReceiver;
        game_network.nrf_ptr->AckReceived = ClientEmptyAck;
        game_network.nrf_ptr->MaxRetriesHit = ClientMaxRetries;
        game_network.address = RF_ADDRESS + data[2];
        game_network.next_client = data[1];
        nRF24_OpenTxPipe(game_network.nrf_ptr, game_network.address);
        nRF24_SetRetries(game_network.nrf_ptr, data[3], data[4]);
        Game_Printf("Joined channel! Player %d, address offset: %d\r\n", data[1], data[2]);
        Task_Remove(HostFillAckPayloads, 0);
        Task_Remove(ControllerTransmitter, 0);
        game_network.last_time = TimeNow();
        Task_Schedule(ControllerTransmitter, 0, 10, GAME_NRF_TX_PERIOD);
        game_network.ack_rx = 0;
    }else {
        Game_Printf("invalid join response\r\n");
    }
}

static void JoinAck(void) {
    Game_Printf("Join failed, no room on channel\r\n");
}

static void JoinMaxRetries(void) {
    Game_Printf("Join failed, hit max retries\r\n");
}

static void ControllerCallback(controller_buttons_t btn, void * ptr) {
	controller_buttons_t act_but;
	act_but = GameControllerHost_GetButtonState(0);
	// OR equal so if multiple buttons are pressed before the paylaod is sent all button presses will be sent
	game_network_controller_msg.data.all_buttons |= act_but.all_buttons;
	game_network.buttons = act_but;
}

void Game_Join(uint8_t channel) {
	controller_buttons_t mask;
    // the second byte is not used
    uint8_t join_msg[2] = {GAME_NRF_MSG_JOIN_REQUEST, 0x44};
    nRF24_Init(game_network.nrf_ptr);
    game_network.channel = channel;
    nRF24_SetChannel(game_network.nrf_ptr, channel);
    nRF24_OpenTxPipe(game_network.nrf_ptr, RF_ADDRESS);
    game_network.nrf_ptr->AckPayloadReceived = JoinAckPayload;
    game_network.nrf_ptr->AckReceived = JoinAck;
    game_network.nrf_ptr->MaxRetriesHit = JoinMaxRetries;
    nRF24_Write(game_network.nrf_ptr, &join_msg[0], 2);
    mask.all_buttons = 0xFFFF;
    GameControllerHost_RegisterCallback(0, ControllerCallback, mask, 0);
    Game_RegisterInputCallback(Game_AllStart);  //Starts the watch for the GameStart command from the host
}

void Game_RegisterInputCallback(void(*fn)(game_network_payload_t *)) {
    game_network.network_input = fn;
}

void Game_RegisterHostPacketizer(uint8_t(*fn)(uint8_t *)) {
    game_network.host_packetizer = fn;
}

void Game_AllStart(game_network_payload_t * input) { 
    if(input->type == 0x03){ //Verifies game start flag
        volatile uint8_t i;
        for (i = 0; i <= game_last_index; i++) { //For loop to run through each of the local games
            if (strcasecmp((char *)input->user_data, games[i].name) == 0) // Compares packet data w/ game names
            {
                Play(games[i].play);
                return;
            }
        }
        Game_Printf("No game called: '%s' found\r\n", (char *)input->user_data);
    }
}

#endif
