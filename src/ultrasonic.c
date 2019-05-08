//#include "timing.h"
#include "task.h"
#include "ultrasonic.h"
#include "project_settings.h"
#include "msp430.h"

/*
* In project_settings.h the following must be defined or set
*   TRIGGER = the Pin which the TRIGGER of the ultrasonic sensor is connected
*           Ex. If TRIGGER is connected to P1.3, then #define TRIGGER = BIT3
*   ECHO = the Pin which the ECHO of the ultrasonic sensor is connected
*           Ex. If ECHO is connected to P1.4, then #define ECHO = BIT4
*
*   If using 2 Ultrasonic Sensors add #define USE_ULTRASONIC2 1 to the project_settings.h file.
*   Also must denote the pins for the second sensor.
*   Ex.
*           #define ECHO2 BIT5
*           #define TRIGGER2 BIT4
*
* NOTE: Ultrasonic Sensor 1 must use Port 1 pins
*       Ultrasonic Sensor 2 must use Port 2 pins
*
*   Ultrasonic.c made to work with HC-SR04 Ultrasonic Sensor
*   Works from 2cm to 400cm with an approximate deviation of + or - 2cm
*/

uint32_t ms;
int echoLength;

void Ultrasonic_Measure();
void Ultrasonic_Calc(uint16_t gap);

#if USE_ULTRASONIC2

uint32_t ms2;
int echoLength2;

void Ultrasonic_Measure2();
void Ultrasonic_Calc2(uint16_t gap2);
//void Ultrasonic_SendValue();

#endif

void Ultrasonic_Init(uint16_t period){
    P1DIR |= TRIGGER;       // TRIGGER pin as output
    P1DIR &= ~ECHO;         // ECHO pin as input
    P1IE |= ECHO;           //Enable Port Interrupt on ECHO Pin
    TA1CCR0 = 24000;    //Set the period to 1ms
    TA1CTL &= ~TAIFG;   //Clear the interrupt flag
    TA1CTL |= TASSEL_2 | MC_1 | TACLR; //Set SMCLK, UP Mode
    TA1CCTL0 |= CCIE;

#if USE_ULTRASONIC2
    P2DIR |= TRIGGER2;       // TRIGGER pin as output
    P2DIR &= ~ECHO2;         // ECHO pin as input
    P2IE |= ECHO2;           //Enable Port Interrupt on ECHO Pin
    TA2CCR0 = 25000;    //Set the period to 1ms
    TA2CTL &= ~TAIFG;   //Clear the interrupt flag
    TA2CTL |= TASSEL_2 | MC_1 | TACLR; //Set SMCLK, UP Mode
    TA2CCTL0 |= CCIE;
#endif
    Task_Schedule(Ultrasonic_Measure, 0, 0, 0);

}
void Ultrasonic_Deinit(uint8_t sensor_handle){
    if (sensor_handle == 1)
        Task_Remove(Ultrasonic_Measure, 0);
#if USE_ULTRASONIC2
    else if (sensor_handle == 2)
        Task_Remove(Ultrasonic_Measure2, 0);
#endif
    else{
        Task_Remove(Ultrasonic_Measure, 0);
#if USE_ULTRASONIC2
        Task_Remove(Ultrasonic_Measure2, 0);
#endif
    }
}

void Ultrasonic_Calc(uint16_t gap){
    distance =  gap / 58;
     Task_Schedule(Ultrasonic_Measure, 0, 0, 0);
}

void Ultrasonic_Measure(){
    P1IE &= ~0x01;          // disable interupt
    P1DIR |= TRIGGER;          // trigger pin as output
    P1OUT |= TRIGGER;          // generate pulse
    __delay_cycles(10);             // for 10us
    P1OUT &= ~TRIGGER;                 // stop pulse
    P1DIR &= ~ECHO;         // make pin P1.2 input (ECHO)
    P1IFG = 0x00;                   // clear flag just in case anything happened before
    P1IE |= ECHO;           // enable interupt on ECHO pin
    P1IES &= ~ECHO;         // rising edge on ECHO pin
}

//void Ultrasonic_SendValue(){
//
//}
// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
    if(P1IFG&ECHO)  //check for pending interrupt
        {
          if(!(P1IES&ECHO)) // if rising edge
          {
            TA1CTL|=TACLR;  // clears timer
            ms = 0;       // reset time
            P1IES |= ECHO;  // falling edge
          }
          else
          {
            int carryOverUS;
            carryOverUS = ((long)TA1R/24);
            echoLength = (long)ms*1000 + (long)carryOverUS;    //calculating ECHO time length
            Ultrasonic_Calc(echoLength);
          }
    P1IFG &= ~ECHO;             //clear flag
    }
   }                  // Clear P1.4 IE


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TIMER1_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    ms++;
    if (ms > 3000){  // after 3000 milliseconds the sensor times out
        Task_Schedule(Ultrasonic_Measure, 0, 0, 0);  // reschedule incase of timeout
    }
    TA1CTL &= ~TAIFG;   //Clear the interrupt flag
}


#if USE_ULTRASONIC2


void Ultrasonic_Calc2(uint16_t gap2){
    distance2 =  gap2 / 58 ;
     Task_Schedule(Ultrasonic_Measure2, 0, 0, 0);
}
void Ultrasonic_Measure2(){
    P2IE &= ~0x01;          // disable interupt
    P2DIR |= TRIGGER2;          // trigger pin as output
    P2OUT |= TRIGGER2;          // generate pulse
    __delay_cycles(10);             // for 10us
    P2OUT &= ~TRIGGER2;                 // stop pulse
    P2DIR &= ~ECHO2;         // make pin P1.2 input (ECHO)
    P2IFG = 0x00;                   // clear flag just in case anything happened before
    P2IE |= ECHO2;           // enable interupt on ECHO pin
    P2IES &= ~ECHO2;         // rising edge on ECHO pin
}

// Port 2 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
    if(P2IFG&ECHO2)  //check for pending interrupt
        {
          if(!(P2IES&ECHO2)) // if rising edge
          {
            TA2CTL|=TACLR;  // clears timer
            ms2 = 0;       // reset time
            P2IES |= ECHO2;  // falling edge
          }
          else
          {
            int carryOverUS2;
            carryOverUS2 = ((long)TA2R/25);
            echoLength2 = (long)ms2*1000 + (long)carryOverUS2;    //calculating ECHO time length
            Ultrasonic_Calc2(echoLength2);
          }
    P2IFG &= ~ECHO2;             //clear flag
    }
   }

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER2_A0_VECTOR
__interrupt void TIMER2_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TIMER1_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    ms2++;
    if (ms2 > 3000){  // after 3000 milliseconds the sensor times out
        Task_Schedule(Ultrasonic_Measure2, 0, 0, 0);  // reschedule incase of timeout
    }
    TA2CTL &= ~TAIFG;   //Clear the interrupt flag
}
#endif
