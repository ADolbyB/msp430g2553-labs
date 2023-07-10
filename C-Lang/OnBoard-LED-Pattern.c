/********************************************************************************************
Joel Brigida
4/12/2021
CDA 3331C-001
On Board LED Patterns. This program uses the Red and Green on board LEDs only. No extra
Hardware needed.

This program performs the following functions:
    At the push of the button SW-2 on P1.3, the microcontroller cycles through 4 different modes:
    Mode 1: LED1 (Green) flashes while LED2 (Red) is OFF
    Mode 2: LED2 (Red) flashes while LED1 (Green) is OFF
    Mode 3: LED1 and LED2 flash simultaneously (Red and Green in Unison)
    Mode 4: LED1 and LED2 flash alternately (Red / Green / Red / Green)
********************************************************************************************/

#include <msp430.h>
#include "msp430g2553.h"

int sw2 = 0;                                                    // variable to hold value of switch 2
int counter = 1;                                                // counting semaphore that determines LED MODE #(1 - 4)

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                   // stop watchdog timer
    P1DIR  = 0x00;                                              // Make Port 1 all inputs
    P1DIR |= (BIT0 | BIT6);                                     // set P1.0 and P1.6 as outputs (LED1, LED2)
    P1REN |= BIT3;                                              // activate resistor on P1.3 for SW2
    P1OUT |= BIT3;                                              // P1.3 = pull up resistor: SW2 is active LOW

    for (;;)
    {
        sw2 = P1IN;                                             // read values from Port 1
        sw2 &= BIT3;                                            // mask out only BIT3 where SW2 is connected

        if ((sw2 & BIT3) == 0)                                  // if button P1.3 is pressed
        {
            counter++;                                          // increment the counter
            if (counter == 5)                                   // if counter reaches undefined value
            {
                counter = 1;                                    // reset the counter to 1
            }
            while ((sw2 & BIT3) == 0)                           // while the switch is held down
            {
                sw2 = P1IN;                                     // read the switch status for a change
                sw2 &= BIT3;                                    // mask the switch variable
            }                                                   // if the switch is let go, the loop condition becomes false
        }
        if (counter == 1)                                       // MODE #1 - Flash Green LED
        {
            P1OUT &= ~BIT6;                                     // turn LED2 (RED) off
            P1OUT ^= BIT0;                                      // toggle LED1 (GREEN)
        }
        else if (counter == 2)                                  // MODE #2 - Flash Red LED
        {
            P1OUT &= ~BIT0;                                     // turn LED1 (GREEN) off
            P1OUT ^= BIT6;                                      // toggle LED2 (RED)
        }
        else if (counter == 3)                                  // MODE #3 - Flash Green & Red Simultaneously
        {
            P1OUT |= BIT6;                                      // switch RED LED ON
            P1OUT |= BIT0;                                      // switch GREEN LED ON
            __delay_cycles(100000);                             // wait 100,000 microsec
            P1OUT &= ~BIT0;                                     // switch GREEN LED OFF
            P1OUT &= ~BIT6;                                     // switch RED LED OFF
        }
        else if (counter == 4)                                  // MODE #4 - Flash Green & Red Alternately
        {
            P1OUT |= BIT0;                                      // switch GREEN LED ON
            P1OUT &= ~BIT6;                                     // switch RED LED OFF
            __delay_cycles(100000);                             // wait 100,000 microsec
            P1OUT &= ~BIT0;                                     // switch GREEN LED OFF
            P1OUT |= BIT6;                                      // switch RED LED ON
        }
        __delay_cycles(100000);                                 // wait 100,000 micro seconds
    }   // end of for loop
}   //end of main