/***********************************************************************
Joel Brigida
CDA 3331C-001: Intro to Microprocessors
4/5/2021

This program uses 8 LEDs on a breadboard and produces a simple toggle
pattern. In "read mode" (1st switch on) the individual LEDs can be 
switched on or off to control what the toggle pattern looks like.
Then, when switch 1 is turned off, all LEDs are toggled alternately 
in the chosen pattern.

************************************************************************/

#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                   // Stop watch dog timer

    int R5_SW = 0, R6_LED = 0;

    P1OUT = 0b00000000;                                         // P1OUT = 0x00
    P1DIR = 0b11111111;                                         // P1DIR = 0xFF
    P2DIR = 0b00000000;                                         // P2DIR = 0x00
    P2REN = 0xFF;                                               // enable all resistors on port 2
    P2OUT = 0xFF;                                               // make them all of them pull ups

    while (1)
    {
        R5_SW = P2IN;                                           // read switch states

        // checking P2.0 for read mode
        if (R5_SW & BIT0)                                       // MASKS the entire switch sequence
        {
            R6_LED = R5_SW & (BIT2 | BIT3 | BIT4 | BIT5);       // mask pattern bits: which LEDs are toggled separately
            P1OUT = R6_LED;                                     // sets the display pattern
        }
        else                                                    // display the pattern
        {
            R6_LED ^= 0xFF;                                     // toggle pattern by XORing w/ all 1's
            R6_LED &= 0x00FF;                                   // mask any excessive bits
            P1OUT = R6_LED;                                     // Displays the pattern
        }

        if (R5_SW & BIT1)
        {
            __delay_cycles(120000);                             // fast
        }
        else
        {
            __delay_cycles(175000);                             // slow
        }
    }   //end while
}   // end main