#include <msp430.h>
#include <stdint.h>
#include "system.h"
#include "nrf24.h"
#include "timing.h"
#include "task.h"
#include "uart.h"
#include "spi.h"
#include "hal_general.h"

#define CHIP1_AND_2

// add the following to system.h:
// #define USE_SPI2
// #define SPI_MAX_SIZE 33

void SetClk24MHz(void);
void SetVcoreUp (unsigned int level);

#define RF_SPI_CH SPI_B0

void RF1_CE(uint8_t out);
void RF1_CSN(uint8_t out);
void RF1_PollIRQ(void);

void RF2_CE(uint8_t out);
void RF2_CSN(uint8_t out);
void RF2_PollIRQ(void);

void RF1_Init(void);
void RF2_Init(void);

void RF1_RxPayloadHandler(uint8_t * data, uint8_t length);
void RF1_AckPayloadHandler(uint8_t * data, uint8_t length);
void RF1_AckReceivedHandler(void);
void RF1_MaxRetriesHandler(void);

void RF2_RxPayloadHandler(uint8_t * data, uint8_t length);
void RF2_AckPayloadHandler(uint8_t * data, uint8_t length);
void RF2_AckReceivedHandler(void);
void RF2_MaxRetriesHandler(void);
void RF2_AckPayloadSent(void);
#ifdef CHIP1_AND_2
void RF_Test(void);
void WriteAcks(void);
#else
void RF_TX1(void);
void RF_TX2(void);
#endif

#define RF_ADDRESS 0x6C6C6C7828

nrf24_t RF1;
nrf24_t RF2;

uint8_t tx_buf[13] = {"1Hello RF2\r\n"};
uint8_t tx1_buf[13] = {"2Hello RF2\r\n"};
uint8_t tx2_buf[13] = {"3Hello RF2\r\n"};
uint8_t retry_buf[15] = {"Hello Retry!\r\n"};
uint8_t ack_buf[10] = {"Hi RF1!\r\n"};

static uint8_t rx_from = 0;
/*
 * main.c
 */
uint32_t payload1_rx = 0, ack1_rx = 0;
uint32_t payload2_rx = 0;
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    SetClk24MHz();

    DisableInterrupts();

    Timing_Init();
    Task_Init();
    UART_Init(1);

    //Setup IRQ, CE, CSN for RF2
	P7DIR |= BIT0; // CE as output
	P2DIR |= BIT2; // CSN as output
	P6DIR &= ~BIT4; // IRQ as input

    //Setup IRQ, CE, CSN for RF1
    P2DIR |= BIT3 | BIT4; // CE, CSN as output
    P2DIR &= ~BIT5; // IRQ as input

    spi_settings_t spi_settings;
    spi_settings.channel = RF_SPI_CH;
    spi_settings.bit_rate = 200000;
    spi_settings.hal_settings.char7bit = 0;
    spi_settings.hal_settings.msb_first = 1;
    spi_settings.mode = 0;
    SPI_Init(&spi_settings);

    EnableInterrupts();
    RF1_CSN(1);
    RF2_CSN(1);

#ifdef CHIP1_AND_2
    RF1_Init();
	nRF24_OpenTxPipe(&RF1, RF_ADDRESS);
	RF2_Init();
	nRF24_OpenRxPipe(&RF2, 1, RF_ADDRESS);
	nRF24_OpenRxPipe(&RF2, 2, RF_ADDRESS+20);
	nRF24_OpenRxPipe(&RF2, 3, RF_ADDRESS+40);
	//nRF24_WriteReg(&RF2, EN_AA, 0x0E);
	nRF24_StartListening(&RF2);
	// setup first ACK
	nRF24_WriteAck(&RF2, &ack_buf[0], 9, 1);
	Task_Schedule(RF_Test, 0, 33, 40);
	UART_Write(UART1, "I'm 1/2\r\n", 9);
	Task_Schedule(WriteAcks, 0, 15,40);
#else
    RF1_Init();
    RF2_Init();
    nRF24_OpenTxPipe(&RF1, RF_ADDRESS+40);
    nRF24_OpenTxPipe(&RF2, RF_ADDRESS+20);
    Task_Schedule(RF_TX1, 0, 44, 40);
    Task_Schedule(RF_TX2, 0, 55, 40);
    UART_Write(UART1, "I'm 3/4\r\n", 9);
#endif

	uint32_t time = TimeNow();
	while(1){
		SystemTick();
		RF1_PollIRQ();
		RF2_PollIRQ();
		if(TimeSince(time)>10000) {
			UART_Printf(UART1, "rx2: %l, rx1: %l, ack1: %l\r\n", payload2_rx, payload1_rx, ack1_rx);
			time = TimeNow();
			payload2_rx = 0;
			payload1_rx = 0;
			ack1_rx = 0;
		}
	}
}


/* MSP430F5529 RF1:
* - MISO P3.1
* - MOSI P3.0
* - SCK  P3.2
* - CS   P2.3
* - CE   P2.4
* - IRQ  P2.5
*/

void RF1_Init(void){
    RF1.ce = RF1_CE;
    RF1.csn = RF1_CSN;
    RF1.spi_channel = RF_SPI_CH;
    RF1.ReceivedPayload = RF1_RxPayloadHandler;
    RF1.AckPayloadReceived = RF1_AckPayloadHandler;
    RF1.AckReceived = RF1_AckReceivedHandler;
    RF1.MaxRetriesHit = RF1_MaxRetriesHandler;

    nRF24_Init(&RF1);
#ifdef CHIP1_AND_2
    nRF24_SetRetries(&RF1, 1, 8);
#else
    nRF24_SetRetries(&RF1, 3, 8);
#endif
    nRF24_SetChannel(&RF1, 46);
}

// This function is provided to the network layer in the init function and is used to control the
// Chip Enable pin on the radio
void RF1_CE(uint8_t out){
	P2OUT = (P2OUT & ~BIT4) | (out << 4);
}

// This function is provided to the network layer in the init function and is used to control the
// Chip Select pin on the radio
void RF1_CSN(uint8_t out){
	P2OUT = (P2OUT & ~BIT3) | (out << 3);
}

// Method to poll the interrupt pin and see if an interrupt has occured
void RF1_PollIRQ(void){
	static uint8_t pin_state = 1;
	static uint32_t last = 0;
	uint8_t new_state = (P2IN & BIT5) >> 5;

	if( (new_state != pin_state) && !new_state) {
		last = TimeNow();
		nRF24_ISR(&RF1);
	}else if(!new_state && TimeSince(last)>5) {
		last = TimeNow();
		nRF24_ISR(&RF1);
	}
	pin_state = new_state;
}

// This function is provided to the network layer in the init function and is used to control the
// Chip Enable pin on the radio
void RF2_CE(uint8_t out){
	P7OUT = (P7OUT & ~BIT0) | out;
}

// This function is provided to the network layer in the init function and is used to control the
// Chip Select pin on the radio
void RF2_CSN(uint8_t out){
	P2OUT = (P2OUT & ~BIT2) | (out << 2);
}

// Method to poll the interrupt pin and see if an interrupt has occured
void RF2_PollIRQ(void){
	static uint8_t pin_state = 1;
	static uint32_t last = 0;
	uint8_t new_state = (P6IN & BIT4) >> 4;

	if( (new_state != pin_state) && !new_state) {
		last = TimeNow();
		nRF24_ISR(&RF2);
	}else if(!new_state && TimeSince(last)>5) {
		last = TimeNow();
		nRF24_ISR(&RF2);
	}
	pin_state = new_state;
}

/* MSP430F5529 RF2:
* - MISO P3.1
* - MOSI P3.0
* - SCK  P3.2
* - CS   P2.2
* - CE   P7.0
* - IRQ  P6.4
*/
void RF2_Init(void){
    RF2.ce = RF2_CE;
    RF2.csn = RF2_CSN;
    RF2.spi_channel = RF_SPI_CH;
    RF2.ReceivedPayload = RF2_RxPayloadHandler;
    RF2.AckPayloadReceived = RF2_AckPayloadHandler;
    RF2.AckPayloadSent = RF2_AckPayloadSent;
    RF2.AckReceived = RF2_AckReceivedHandler;
    RF2.MaxRetriesHit = RF2_MaxRetriesHandler;

    nRF24_Init(&RF2);
#ifdef CHIP1_AND_2
    //nRF24_SetRetries(&RF2, 5, 7);
#else
    nRF24_SetRetries(&RF2, 5, 8);
#endif
    nRF24_SetChannel(&RF2, 46);
}

void RF_Test(void){
	nRF24_Write(&RF1, &tx_buf[0], 12);
}

void RF_TX1(void){
	nRF24_Write(&RF1, &tx1_buf[0], 12);
}

void RF_TX2(void){
	nRF24_Write(&RF2, &tx2_buf[0], 12);
}

void WriteAcks(void) {
	if(rx_from == 0x0E) {
		rx_from = 0;
		nRF24_FlushTx(&RF2);
		nRF24_WriteAck(&RF2, &ack_buf[0], 9, 1);
		nRF24_WriteAck(&RF2, &ack_buf[0], 9, 2);
		nRF24_WriteAck(&RF2, &ack_buf[0], 9, 3);
	}else {
		Task_Schedule(WriteAcks, 0, 5, 0);
	}
}

void RF1_AckPayloadHandler(uint8_t * data, uint8_t length){
	//UART_Write(UART1, data, length);
	payload1_rx++;
}

void RF1_RxPayloadHandler(uint8_t * data, uint8_t length) {
	//UART_Write(UART1, data, length);
}

void RF1_AckReceivedHandler(void){

	ack1_rx++;
}

void RF1_MaxRetriesHandler(void){
	nRF24_FlushTx(&RF1);
	UART_Write(UART1, "max1\r\n", 6);
}

void RF2_AckPayloadHandler(uint8_t * data, uint8_t length){
	payload2_rx++;
	//UART_Write(UART1, (char*)data, length);
}

void RF2_RxPayloadHandler(uint8_t * data, uint8_t length) {


	rx_from |= 1 << RF2.last_rx_pipe;

	//UART_Write(UART1, data, length);

	//nRF24_WriteAck(&RF2, &ack_buf[0], 9, RF2.last_rx_pipe);
	payload2_rx++;
}

void RF2_AckPayloadSent(void) {
	//UART_Write(UART1, "ack2 tx\r\n", 9);
}

void RF2_AckReceivedHandler(void){
	//UART_Write(UART1, "ack2\r\n", 6);
}

void RF2_MaxRetriesHandler(void){
	nRF24_FlushTx(&RF2);
	UART_Write(UART1, "max2\r\n", 6);
}

void SetClk24MHz(){
	// Increase Vcore setting to level3 to support fsystem=25MHz
	// NOTE: Change core voltage one level at a time..
	SetVcoreUp (0x01);
	SetVcoreUp (0x02);
	SetVcoreUp (0x03);

	P5SEL |= BIT2+BIT3;
	UCSCTL6 &= ~XT2OFF; // Enable XT2
	UCSCTL6 &= ~XT2BYPASS;
	UCSCTL3 = SELREF__XT2CLK; // FLLref = XT2
	UCSCTL4 |= SELA_2 + SELS__DCOCLKDIV + SELM__DCOCLKDIV;

	UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
	// Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
	do
	{
	UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
										// Clear XT2,XT1,DCO fault flags
	SFRIFG1 &= ~OFIFG;                      // Clear fault flags
	}while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

	// Disable the FLL control loop
	__bis_SR_register(SCG0);

	// Select DCO range 24MHz operation
	UCSCTL1 = DCORSEL_7;
	/* Set DCO Multiplier for 24MHz
	(N + 1) * FLLRef = Fdco
	(5 + 1) * 4MHz = 24MHz  */
	UCSCTL2 = FLLD0 + FLLN0 + FLLN2;
	// Enable the FLL control loop
	__bic_SR_register(SCG0);

  /* Worst-case settling time for the DCO when the DCO range bits have been
	 changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
	 UG for optimization.
	 32 x 32 x 24MHz / 4MHz = 6144 = MCLK cycles for DCO to settle */
  __delay_cycles(70000);

	// Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
	do {
		// Clear XT2,XT1,DCO fault flags
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
		// Clear fault flags
		SFRIFG1 &= ~OFIFG;
	} while (SFRIFG1 & OFIFG); // Test oscillator fault flag
}

void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}
