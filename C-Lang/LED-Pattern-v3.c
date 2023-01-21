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
    WDTCTL = WDTPW | WDTHOLD;   // Stop watch dog timer

    int R_Left = 0;
    int R_Right = 0;
    int Pattern = 0;
    int R5_SW = 0, R6_LED = 0;

    P1OUT = 0b00000000;         // ZERO the OUTPUT upon Power Up
    P1DIR = 0b11111111;         // Set P1 to OUTPUT
    P2DIR = 0b00000000;         // Set P2 to INPUT
    P2REN = 0xFF;               // enable all resistors on port 2 for Debounce Switches
    P2OUT = 0xFF;               // make them all of them pull ups

    while (1)
        {
            //read the states of the switches
            R5_SW = P2IN;

            //checking P2.0 for read mode
            if (R5_SW & BIT0) // MASKS the entire switch sequence and checks which bits are high
                {
                    R6_LED = R5_SW & (BIT3 | BIT4 | BIT5); // mask pattern bits: which LEDs are toggled separately
                    P1OUT = R6_LED;                        // sets the display pattern
                }

            else // else, it is the rotate and display mode
                {
                    if(R5_SW & BIT1)                       // Rotate RIGHT with Logic 1
                        {
                            Pattern = R6_LED;              // Set user input bits to a new variable
                            R_Right = Pattern >> 1;        // Rotate new variable to the right 1 bit
                            R_Left = Pattern << 7;         // Rotate same new variable to the left 7 bits
                            Pattern = (R_Right | R_Left);  // Set the new pattern to the new combination
                            Pattern &= 0x00FF;             // Set extra bits to ZERO
                            R6_LED = Pattern;              // Set new pattern to the output variable
                            P1OUT = R6_LED;
                        }
                    else // Rotate LEFT with Logic 0
                        {
                            Pattern = R6_LED;
                            R_Right = Pattern >> 7;
                            R_Left = Pattern << 1;
                            Pattern = (R_Right | R_Left);
                            Pattern &= 0x00FF;
                            R6_LED = Pattern;
                            P1OUT = R6_LED;
                        }
                    if(R5_SW & BIT2) // Rotate SLOW with Logic 1
                        {
                            __delay_cycles(150000); // slow
                        }
                    else // Rotate FAST with Logic 0
                        {
                            __delay_cycles(60000); // fast
                        }
                }
        } //end while
} // end main