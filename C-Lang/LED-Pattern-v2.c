/***********************************************************************
Joel Brigida
CDA 3331C-001: Intro to Microprocessors
4/5/2021

This program uses 8 LEDs on a breadboard and produces a rotating pattern
That can move left or right by switching the 1st switch, and has a fast
and slow speed toggled by the second switch.

************************************************************************/

#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                           // Stop watch dog timer

    int pattern = 1;
    int R5_SW = 0, R6_LED = 0;

    P1OUT = 0b00000000;                                                 // ZERO the OUTPUT upon Power Up
    P1DIR = 0b11111111;                                                 // Set P1 to OUTPUT
    P2DIR = 0b00000000;                                                 // Set P2 to INPUT
    P2REN = 0xFF;                                                       // enable all resistors on port 2 for Debounce Switches
    P2OUT = 0xFF;                                                       // make them all of them pull ups

    while (1)
    {
        R5_SW = P2IN;                                                   // read the states of the switches

        // check P2.0 for read mode
        if (R5_SW & BIT0)                                               // MASKS the entire switch sequence and checks which bits are high
        {
            R6_LED = R5_SW & (BIT3 | BIT4 | BIT5);                      // mask pattern bits: which LEDs are toggled separately
            P1OUT = R6_LED;                                             // set display pattern
        }

        else                                                            // Rotate & Display mode
        {
            if(R5_SW & BIT1)
            {
                P1OUT = pattern;
                pattern = pattern >> 1;
                if(pattern == 0)
                {
                    pattern = 128;
                }
            }
            else
            {
                P1OUT = pattern;
                pattern = pattern << 1;
                if(pattern > 255)
                {
                    pattern = 1;
                }
            }

            if(R5_SW & BIT2)
            {
                __delay_cycles(50000);                                  // fast
            }
            else
            {
                __delay_cycles(150000);                                 // slow
            }
        }
    }   //end while
}   // end main