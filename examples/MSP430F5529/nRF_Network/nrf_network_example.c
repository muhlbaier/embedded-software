#include <msp430.h>
#include <stdint.h>
#include "project_settings.h"
#include "nrf24.h"
#include "nrf24network.h"
#include "timing.h"
#include "task.h"
#include "uart.h"
#include "spi.h"
#include "hal_general.h"

/**
 * NRF24 Network Example
 *
 * This example demonstrates very basic use of the NRF Network on the MSP430.
 * The example uses two radios both connected to a single MSP430.  Due to the size
 * of the nrf24_network layer, the required program space will exceed the code-size
 * limited license of most Code Composer Studio installations.  To use this project,
 * you must either:
 *
 * 1) Have a full featured license
 * 2) Acquire a 90-day full featured evaluation license
 * 3) Switch compilers to use the GCC compiler
 *
 * Likely the best option for most users is to switch to the GCC compiler due to the
 * expensive full license and time-limit on the evaluation license.  For information
 * on switching to the GCC compiler, refer to the other documentation in this library
 *
 * In this example, the network is configured and some simple messages are sent between the two nodes.
 * The first node is configured using the default network built into the network layer.  The second node
 * requires that the user interface with the extended functionality of the network layer.  For most use cases,
 * there will only be one node on a single uC and therefore, the extended functionality is not needed.  See documentation
 * for the nrf network layer and inline comments below for more information about the extended functionality.
 *
 * The first node, using the default network built into the network layer, is configured using the THIS_NODE macro found in
 * system.h  You must always define this macro in the system.h file to use the network layer. In this example, we want the
 * first node to be a branch (Meaning it has a parent (the THIS_NODE2) and children). Most users on a network will be configured
 * as either a leaf or a branch, since only one user can be configured as a THIS_NODE2.
 *
 * The second node, using the extended functionality, is setup to be the THIS_NODE2 of the network.
 *
 * The first node (the branch) sets up a message handler to listen for TEST_MSG's, while the second node (the THIS_NODE2) sets
 * up a message handler to listen for TEST_MSG's.  (TEST_MSG and TEST_MSG were arbitrarily chosen and do not implement any of the
 * functionality of those application layers)
 *
 * A periodic task is setup to send a TEST_MSG to THIS_NODE from the THIS_NODE2.  The THIS_NODE node has a listener configured to handle
 * reception of TEST_MSG.  In the handler, the received message is simply printed over UART1.  In addition, the THIS_NODE node, in turn,
 * writes a TEST_MSG back to the THIS_NODE2.  Remember this is for demonstration purposes only.  Typically the THIS_NODE2 and branch nodes would
 * both have message handlers for the same type of system and the THIS_NODE node would just respond with the same message type.
 *
 * When the THIS_NODE2 receives the TEST_MSG, the handler is called and the received message is simply printed to the terminal over UART1.
 *
 * Be sure to read the inline comments below to get a better understanding of how to implement applications using the network layer.
 *
 * To test this example, simply run the program with a terminal setup on UART1 and look for
 *
 * Test Message!
 * Test Message Response!
 *
 */

/*  Macros */
#define RF_SPI_CH SPI_B0

/* Function Declarations */
void SetClk24MHz(void);
void SetVcoreUp (unsigned int level);

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

void SendTest(void);
void ThisNode_TestMsgHandler(uint8_t * data, uint8_t length, uint8_t from);
void ThisNode2_TestMsgHandler(uint8_t * data, uint8_t length, uint8_t from);

/* Variables */
nrfnet_t RF2_Net;

char test_msg[16] = {"Test Message!\r\n"};
char reply_msg[25] = {"Test Message Response!\r\n"};

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    SetClk24MHz();

    DisableInterrupts();

    Timing_Init();
    Task_Init();
    UART_Init(UART1);

    // Setup the SPI channel to be used by the NRF nodes
    spi_settings_t spi_settings;
    spi_settings.channel = RF_SPI_CH;
    spi_settings.bit_rate = 100000;
    spi_settings.hal_settings.char7bit = 0;
    spi_settings.hal_settings.msb_first = 1;
    spi_settings.mode = 0;
    SPI_Init(&spi_settings);

    //Setup IRQ, CE, CSN for node 1
    P2DIR |= BIT3 | BIT4; // CE, CSN as output
    P2DIR &= ~BIT5; // IRQ as input
    RF1_CSN(1);

    //Setup IRQ, CE, CSN for node 2
    P7DIR |= BIT0; // CE as output
    P2DIR |= BIT2; // CSN as output
    P6DIR &= ~BIT4; // IRQ as input
    RF2_CSN(1);

    EnableInterrupts();

    // Setup the default network, this uses the THIS_NODE macro found in system.h
    nrf24_NetworkInit(RF1_CE, RF1_CSN, RF_SPI_CH);
    // Setup the second node as THIS_NODE2 using the extended network framework used on systems with
    // multiple nodes
    nrf24_NetworkInitN(&RF2_Net, RF2_CE, RF2_CSN, RF_SPI_CH, THIS_NODE2);

    // Register a message handler on the THIS_NODE node listening for TEST_MSGs
    nrf24_RegisterMsgHandler(TEST_MSG, ThisNode_TestMsgHandler);
    // Register a message handler on the THIS_NODE2 node listening for TEST_MSGs
    nrf24_RegisterMsgHandlerN(&RF2_Net, TEST_MSG, ThisNode2_TestMsgHandler);

    // Schedule a task to send a TEST_MSG from the THIS_NODE2 node to THIS_NODE every 1 second
    Task_Schedule(SendTest, 0, 500, 1000);

    while(1){
        SystemTick();
        // Poll the interrupt pins coming from the radios so we know whens something has happened
        RF1_PollIRQ();
        RF2_PollIRQ();
    }
}

void SendTest(void){
    // Send a TEST_MSG to THIS_NODE from THIS_NODE2
    nrf24_SendMsgN(&RF2_Net, THIS_NODE, TEST_MSG, (uint8_t*)&test_msg[0], 15);
}

void ThisNode_TestMsgHandler(uint8_t * data, uint8_t length, uint8_t from){
    // When we receive the message from the THIS_NODE2, respond with a TEST_MSG
    nrf24_SendMsg(THIS_NODE2, TEST_MSG, (uint8_t*)&reply_msg[0], 24);
    // Print the received message to the terminal
    UART_Write(UART1, data, length);
}

void ThisNode2_TestMsgHandler(uint8_t * data, uint8_t length, uint8_t from){
    // Print the received message to the terminal
    UART_Write(UART1, data, length);
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
    // Poll the interrupt here so that we poll all the time especially as SPI transactions are happening
    RF1_PollIRQ();
}

// Method to poll the interrupt pin and see if an interrupt has occured
void RF1_PollIRQ(void){
    static uint8_t pin_state = 1;
    uint8_t new_state = (P2IN & BIT5) >> 5;

    if( (new_state != pin_state) && !new_state) {
        nrf24_NetworkISRHandler();
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
    // Poll the interrupt here so that we poll all the time especially as SPI transactions are happening
    RF2_PollIRQ();
}

// Method to poll the interrupt pin and see if an interrupt has occured
void RF2_PollIRQ(void){
    static uint8_t pin_state = 1;
    uint8_t new_state = (P6IN & BIT4) >> 4;

    if( (new_state != pin_state) && !new_state) {
        nrf24_NetworkISRHandlerN(RF2_Net);
    }
    pin_state = new_state;
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
