#include "seven_seg.h"
#include "hal_general.h"
#include "hal_seven_seg.h"
#include <stdint.h>


 /*
   * CONFIGURATION
   * P1.2 -> B -> PIN 7
   * P1.3 -> E -> PIN 1
   * P1.4 -> F -> PIN 10
   * P1.5 -> A -> PIN 11
   *
   * P2.2 -> G -> PIN 5
   * P2.3 -> D -> PIN 2
   * P2.4 -> Decimal -> PIN 3 
   * P2.5 -> C -> PIN 4
   *
   * P3.3 -> D4 -> PIN 6
   * P3.4 -> D3 -> PIN 8
   * P3.5 -> D2 -> PIN 9
   * P3.6 -> D1 -> PIN 12
   *
   * */
   const int delay = 2000;
   
   void hal_seven_seg_Init(void)
   {
	    P1DIR |= BIT2 + BIT3 + BIT4 + BIT5;                            // P1.0 set as output
		P2DIR |= BIT2 + BIT3 + BIT4 + BIT5;

		P3DIR |= BIT3 + BIT4 + BIT5 + BIT6;
   
		P1OUT &= 0;
		P2OUT &= 0;
		P3OUT &= 0;

		P3OUT &= 0;
		P3OUT |= ~BIT3;

		
   }

void hal_Display(uint8_t seg0, uint8_t seg1, uint8_t seg2, uint8_t seg3, uint8_t dec)
{
	if(dec == 1)
       P2OUT |= BIT4;
	 switch(seg0)
          {
              case 0 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT3+ BIT5;
                  break;
              case 1 :
                  P1OUT |= BIT2;
                  P2OUT |= BIT5;
                  break;
              case 2 :
                  P1OUT |= BIT2 + BIT3 + BIT5;
                  P2OUT |= BIT2 + BIT3;
                  break;
              case 3 :
                  P1OUT |= BIT2 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 4 :
                  P1OUT |= BIT2 + BIT4;
                  P2OUT |= BIT2 + BIT5;
                  break;
              case 5 :
                  P1OUT |= BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 6 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 7 :
                  P1OUT |= BIT5 + BIT2;
                  P2OUT |= BIT5;
                  break;
              case 8 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 9 :
                  P1OUT |= BIT5 + BIT2 + BIT4;
                  P2OUT |= BIT5 + BIT2;
                  break;
              case 10 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT5;
                  break;
              case 11 :
                  P1OUT |= BIT3 + BIT4;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 12 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT3;
                  break;
              case 13 :
                  P1OUT |= BIT2 + BIT3;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 14 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3;
                  break;
              case 15 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2;
                  break;
              default:
                  P1OUT &= 0;
                  P2OUT &= 0;
                  break;

          }
          __delay_cycles(delay);
          P3OUT &= 0;
          P3OUT |= ~BIT5;
          P1OUT &= 0;
          P2OUT &= 0;
		  
		  if(dec == 2)
			P2OUT |= BIT4;
          switch(seg1)
          {
              case 0 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT3+ BIT5;
                  break;
              case 1 :
                  P1OUT |= BIT2;
                  P2OUT |= BIT5;
                  break;
              case 2 :
                  P1OUT |= BIT2 + BIT3 + BIT5;
                  P2OUT |= BIT2 + BIT3;
                  break;
              case 3 :
                  P1OUT |= BIT2 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 4 :
                  P1OUT |= BIT2 + BIT4;
                  P2OUT |= BIT2 + BIT5;
                  break;
              case 5 :
                  P1OUT |= BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 6 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 7 :
                  P1OUT |= BIT5 + BIT2;
                  P2OUT |= BIT5;
                  break;
              case 8 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 9 :
                  P1OUT |= BIT5 + BIT2 + BIT4;
                  P2OUT |= BIT5 + BIT2;
                  break;
              case 10 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT5;
                  break;
              case 11 :
                  P1OUT |= BIT3 + BIT4;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 12 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT3;
                  break;
              case 13 :
                  P1OUT |= BIT2 + BIT3;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 14 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3;
                  break;
              case 15 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2;
                  break;
              default:
                  P1OUT &= 0;
                  P2OUT &= 0;
                  break;

          }
          __delay_cycles(delay);
          P3OUT &= 0;
          P3OUT |= ~BIT4;
          P1OUT &= 0;
          P2OUT &= 0;
		  
		  if(dec == 3)
			P2OUT |= BIT4;
          switch(seg2)
          {
              case 0 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT3+ BIT5;
                  break;
              case 1 :
                  P1OUT |= BIT2;
                  P2OUT |= BIT5;
                  break;
              case 2 :
                  P1OUT |= BIT2 + BIT3 + BIT5;
                  P2OUT |= BIT2 + BIT3;
                  break;
              case 3 :
                  P1OUT |= BIT2 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 4 :
                  P1OUT |= BIT2 + BIT4;
                  P2OUT |= BIT2 + BIT5;
                  break;
              case 5 :
                  P1OUT |= BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 6 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 7 :
                  P1OUT |= BIT5 + BIT2;
                  P2OUT |= BIT5;
                  break;
              case 8 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 9 :
                  P1OUT |= BIT5 + BIT2 + BIT4;
                  P2OUT |= BIT5 + BIT2;
                  break;
              case 10 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT5;
                  break;
              case 11 :
                  P1OUT |= BIT3 + BIT4;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 12 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT3;
                  break;
              case 13 :
                  P1OUT |= BIT2 + BIT3;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 14 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3;
                  break;
              case 15 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2;
                  break;
              default:
                  P1OUT &= 0;
                  P2OUT &= 0;
                  break;

          }
          __delay_cycles(delay);
          P3OUT &= 0;
          P3OUT |= ~BIT6;
          P1OUT &= 0;
          P2OUT &= 0;
		  
		  if(dec == 4)
			P2OUT |= BIT4;
          switch(seg3)
          {
              case 0 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT3+ BIT5;
                  break;
              case 1 :
                  P1OUT |= BIT2;
                  P2OUT |= BIT5;
                  break;
              case 2 :
                  P1OUT |= BIT2 + BIT3 + BIT5;
                  P2OUT |= BIT2 + BIT3;
                  break;
              case 3 :
                  P1OUT |= BIT2 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 4 :
                  P1OUT |= BIT2 + BIT4;
                  P2OUT |= BIT2 + BIT5;
                  break;
              case 5 :
                  P1OUT |= BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 6 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 7 :
                  P1OUT |= BIT5 + BIT2;
                  P2OUT |= BIT5;
                  break;
              case 8 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 9 :
                  P1OUT |= BIT5 + BIT2 + BIT4;
                  P2OUT |= BIT5 + BIT2;
                  break;
              case 10 :
                  P1OUT |= BIT2 + BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT5;
                  break;
              case 11 :
                  P1OUT |= BIT3 + BIT4;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 12 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT3;
                  break;
              case 13 :
                  P1OUT |= BIT2 + BIT3;
                  P2OUT |= BIT2 + BIT3+ BIT5;
                  break;
              case 14 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2 + BIT3;
                  break;
              case 15 :
                  P1OUT |= BIT3 + BIT4 + BIT5;
                  P2OUT |= BIT2;
                  break;
              default:
                  P1OUT &= 0;
                  P2OUT &= 0;
                  break;
          }
          __delay_cycles(delay);
          P1OUT &= 0;
          P2OUT &= 0;
          P3OUT &= 0;
          P3OUT |= ~BIT3;
}
