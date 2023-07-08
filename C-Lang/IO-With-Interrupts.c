/************************************************************
Joel Brigida
CDA-3331C - Intro to Microprocessors
3/25/2021

This is a test program to test Input and Output with Interrupts
*************************************************************/

#include <msp430g2553.h>

#define LED1 BIT0                           // GREEN LED
#define LED2 BIT6                           // Red LED
#define BUTTON BIT3                         // On Board Switch

// NEED to enable the pullup resistor for BUTTON / BIT3.

unsigned int folds = 1;                     // Folds is a counting semaphore
int i = 1;

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer

    P1OUT = 0x00;                           // make sure all default outputs are zero on powerup
    P1DIR = 0x00;                           // make all P1.x port pins inputs by default
    P1DIR |= (LED1 | LED2);                 // set P1.0 and P1.6 to output direction (Added OR value)

    P1REN |= BUTTON;                        // enables resistor on the button pin -> OR with 1 to toggle bit HIGH - leaves other bits unaffected
    P1OUT |= BUTTON;                        // make that a pull up -> OR with 1 to toggle that bit HIGH
    P1IE  |= BUTTON;                        // enable interrupt on selected pin -> ENABLE BIT3 to cause interrupt
    P1IES |= BUTTON;                        // interrupt edge select, HI to LO -> INTERRUPT if button gets pressed (active LOW switch)
    //P1IES &= ~BUTTON;                     // interrupt edge select, LO to HI -> INTERRUPT if button gets pressed & RELEASED

    __enable_interrupt();                   // enable interrupts in general

    for (;;)
    {
        P1OUT ^= (LED2);                    // toggle LED2 using XOR (XOR w/ 1 to TOGGLE)
        for(i = 1; i <= folds; i++)         // for folds number of times = 1, 2, or 3.
        {                                   // INTERRUPT Loop affects the value of 'folds'
            __delay_cycles(100000);         // run delay of 100,000 micro seconds
        }
    }
} // end main

/** Port 1 interrupt routine  */
#pragma vector = PORT1_VECTOR               // These 2 lines enable the INTERRUPT ROUTINE for Port1
__interrupt void Port_1(void)               // change both lines to port2 to setup port 2 interrupt
{
    P1OUT |= LED1;                          // LED1 on at start of interrupt service - VISUAL INSPECTION
    folds++;                                // increment folds (AFFECTS DELAY)

    if (folds >= 4)
    {
        folds = 1;                          // cycle folds for 1,2,3
    }

    __delay_cycles(100000);                 // allow some delay to see LED1
    P1IFG &= ~BUTTON;                       // P1.3 IFG cleared - CLEARS INTERRUPT FLAG BIT - SELECTIVE CLEAR WITH MASK

    __delay_cycles(100000);                 // allow some delay for switch debounce
    P1OUT &= ~LED1;                         // LED1 OFF to indicate end of interrupt - VISUAL INSPECTION
}                                           // END port 1 interrupt