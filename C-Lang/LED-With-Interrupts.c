/********************************************************************************************
Joel Brigida
4/12/2021
CDA 3331C-001: Intro to Microprocessors

Alternate version for LED flash patterns using interrupt routine to change flash mode.
This program performs the following functions:
    At the push of the button SW-2 on P1.3, the microcontroller cycles through 4 different modes:
    Mode 1: LED1 (Green) flashes while LED2 (Red) is OFF
    Mode 2: LED2 (Red) flashes while LED1 (Green) is OFF
    Mode 3: LED1 and LED2 flash simultaneously (Red and Green in Unison)
    Mode 4: LED1 and LED2 flash alternately (Red / Green / Red / Green)
********************************************************************************************/

#include <msp430.h>
#include "msp430g2553.h"

#define LED1 BIT0                                               // define LED1 and LED2 as constant macros
#define LED2 BIT6
#define BUTTON BIT3

int counter = 1;                                                // counting semaphore that determines LED MODE #(1 - 4)
int i, delay = 0;                                               // variables to implement a cycling delay
int the_switch = 0;                                             // variable to hold value of the side switch

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                   // stop watchdog timer

    P1OUT = 0x00;                                               // Kill all P1 outputs on powerup
    P2OUT = 0x00;                                               // Kill all P2 outputs on startup
    P1DIR = 0x00;                                               // Initialize Port 1 to Zero
    P2DIR = 0x00;                                               // Initialize Port 2 to Zero

    P1DIR |= (BIT0 | BIT6);                                     // set P1.0 and P1.6 as outputs (LED1, LED2)

    P1REN |= BIT3;                                              // activate resistor on P1.3 for SW2
    P1OUT |= BIT3;                                              // P1.3 = pull up resistor: SW2 is active LOW

                                                                // On-Board Button is active Low
    P1IE  |= BUTTON;                                            // enable interrupt on selected pin: ENABLE BIT3 to cause interrupt
    //P1IES |= BUTTON;                                          // interrupt edge select HI to LO: INTERRUPT if button gets pressed
    P1IES &= ~BUTTON;                                           // interrupt edge select LO to HI: INTERRUPT if button gets pressed & RELEASED

    __enable_interrupt();                                       // enable interrupts in general

    for (;;)
    {
        if (counter == 1)                                       // MODE #1 - Flash Green LED
        {
            P1OUT &= ~LED2;                                     // turn LED2 (RED) off
            P1OUT ^= LED1;                                      // toggle LED1 (GREEN)
            __delay_cycles(150000);                             // Flash Delay: 150,000 micro seconds
        }
        else if (counter == 2)                                  // MODE #2 - progressive delay Green LED
        {
            P1OUT &= ~LED2;                                     // turn LED2 (RED) off
            P1OUT ^= LED1;                                      // toggle LED1 (GREEN)
            delay++;
            if (delay >= 20)
            {
                delay = 1;
            }
            for (i = 0; i < delay; i++)
            {
                __delay_cycles(50000);                          // Flash Delay: 50,000 micro seconds
            }
        }
        else if (counter == 3)                                  // MODE #3 - Flash Red LED
        {
            P1OUT &= ~LED1;                                     // turn LED1 (GREEN) off
            P1OUT ^= LED2;                                      // toggle LED2 (RED)
            __delay_cycles(150000);                             // Flash Delay: 150,000 micro seconds
        }
        else if (counter == 4)                                  // MODE #4 - Flash Green & Red Alternately
        {
            P1OUT |= LED1;                                      // switch GREEN LED1 ON
            P1OUT &= ~LED2;                                     // switch RED LED2 OFF
            __delay_cycles(150000);                             // wait 150,000 microsec
            P1OUT &= ~LED1;                                     // switch GREEN LED1 OFF
            P1OUT |= LED2;                                      // switch RED LED2 ON
            __delay_cycles(150000);                             // Flash Delay: 150,000 micro seconds
        }
        else if (counter == 5)                                  // MODE #5 - Flash Green & Red Simultaneously
        {
            P1OUT |= (LED1 | LED2);                             // switch RED & GREEN LEDs ON
            __delay_cycles(150000);                             // wait 150,000 microsec
            P1OUT &= ~(LED1 | LED2);                            // Switch RED & GEEN LEDs OFF
            __delay_cycles(150000);                             // Flash Delay: 100,000 micro seconds
        }
        else if (counter == 6)                                  // MODE #6 - Flash Red 2x & Green 2x Alternately
        {
            P1OUT |= LED2;
            __delay_cycles(150000);
            P1OUT &= ~LED2;
            __delay_cycles(150000);
            P1OUT |= LED2;
            __delay_cycles(150000);
            P1OUT &= ~LED2;
            __delay_cycles(150000);
            P1OUT |= LED1;
            __delay_cycles(150000);
            P1OUT &= ~LED1;
            __delay_cycles(150000);
            P1OUT |= LED1;
            __delay_cycles(150000);
            P1OUT &= ~LED1;
            __delay_cycles(150000);
            P1OUT |= (LED1 | LED2);
            __delay_cycles(150000);
            P1OUT &= ~(LED1 | LED2);
            __delay_cycles(150000);
            P1OUT |= (LED1 | LED2);
            __delay_cycles(150000);
            P1OUT &= ~(LED1 | LED2);
            __delay_cycles(150000);
        }
    }  // end of for loop
} //end of main

/*********************************** Port 1 interrupt routine  **************************************/
#pragma vector = PORT1_VECTOR                                   // enable the INTERRUPT ROUTINE for Port1
__interrupt void Port_1(void)                                   // change both lines to Port_2 to setup port 2 interrupt
{
    P1OUT |= (LED1 | LED2);                                     // Both LEDs on at start of interrupt routine: VISUAL INSPECTION

    i = 0;                                                      // reset the variables for mode #2
    delay = 0;                                                  // ensures mode 2 starts from the beginning every time

    __delay_cycles(500000);                                     // keep LEDs on for 1/2 second
    counter++;                                                  // increment MODE counter (changes flash mode)

    if (counter >= 7)
    {
        counter = 1;                                            // reset if value is implausible
    }

    P1IFG &= ~BUTTON;                                           // P1.3 IFG cleared: CLEAR INTERRUPT FLAG BIT: SELECTIVE CLEAR WITH MASK

    __delay_cycles(100000);                                     // allow delay for switch debounce

    P1OUT &= ~(LED1 | LED2);                                    // Both LEDs off at the end of interrupt routine

}   // END port 1 interrupt