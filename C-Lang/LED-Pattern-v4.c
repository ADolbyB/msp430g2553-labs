/***************************************************************************
Joel Brigida
CDA 3331C-001
Dr. Bassem Alhalabi
4/5/2021

LED Pattern Version 4 - makes 7 different LED display patterns using a switch
statement and 3 hardware switch inputs (BIT0, BIT1, BIT2).
A 4th switch (BIT5) also controls the speed of the patterns.

****************************************************************************/

#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watch dog timer

    int User_Choice = 0;        // variable for hardware switch pattern
    int speed = 0;              // variable for speed control
    int counter = 0;            // counting semaphore variable (determines display pattern)
    int wig_wag1 = 0;           // variable for 1st alternating wig wag pattern
    int wig_wag2 = 0;           // variable for 2nd alternating wig wag pattern
    int rotate_pattern = 0;     // output for the rotate patterns
    int R_Left = 0;             // right rotated string
    int R_Right = 0;            // left rotated string

    P1OUT = 0b00000000;         // ZERO the OUTPUT upon PowerUp
    P1DIR = 0b11111111;         // Set P1 to OUTPUT
    P2DIR = 0b00000000;         // Set P2 to INPUT
    P2REN = 0xFF;               // enable all resistors on port 2 for Debounce Switches
    P2OUT = 0xFF;               // make them all of them pull ups

    for(;;)
        {
            speed = P2IN;
            User_Choice = P2IN;
            User_Choice &= 0x07;                            // MASK: Use only Last 3 bits (7 options)
            switch(User_Choice)
                {
                case 0x00:                                  // BASE CASE: Reset all variables.
                    P1OUT = 0x00;                           // TURN OFF LEDs
                    rotate_pattern = 0x18;                  // set/reset the rotate pattern
                    wig_wag1 = 0x0055;                      // set wig_wag1 base pattern
                    wig_wag2 = 0x0099;                      // set wig_wag2 base pattern
                    counter = 0;                            // reset counter
                    break;
                case 0x01:                                  // Pattern: Rotate OUT
                    if(counter == 0)
                    {
                        P1OUT = 0x00;
                    }
                    else if(counter == 1)
                    {
                        P1OUT = 0x18;
                    }
                    else if(counter == 2)
                    {
                        P1OUT = 0x24;
                    }
                    else if(counter == 3)
                    {
                        P1OUT = 0x42;
                    }
                    else if(counter == 4)
                    {
                        P1OUT = 0x81;
                    }
                    else if(counter >= 5)
                    {
                        P1OUT = 0x00;
                        counter = 0;
                    }
                    counter++;
                    break;
                case 0x02:                                  // Pattern: Rotate Left
                    P1OUT = rotate_pattern;                 // Set new pattern to the output variable
                    R_Right = rotate_pattern >> 1;          // Rotate new variable to the right 1 bit
                    R_Left = rotate_pattern << 7;           // Rotate same new variable to the left 7 bits
                    rotate_pattern = (R_Right | R_Left);    // Set the new pattern to the new combination
                    rotate_pattern &= 0x00FF;               // Set extra bits to ZERO
                    break;
                case 0x03:                                  // Pattern: Rotate IN
                    if(counter == 0)
                    {
                        P1OUT = 0x00;
                    }
                    else if(counter == 1)
                    {
                        P1OUT = 0x81;
                    }
                    else if(counter == 2)
                    {
                        P1OUT = 0x42;
                    }
                    else if(counter == 3)
                    {
                        P1OUT = 0x24;
                    }
                    else if(counter == 4)
                    {
                        P1OUT = 0x18;
                    }
                    else if(counter >= 5)
                    {
                        P1OUT = 0x00;
                        counter = 0;
                    }
                        counter++;
                    break;
                case 0x04:
                    if(counter == 0 || counter == 10 || counter == 20)
                        {
                            P1OUT = 0x00;
                            if(counter >= 20)
                                {
                                    counter = 0;
                                }
                        }
                    else if(counter == 1 || counter == 19)
                        {
                            P1OUT = 0x01;
                        }
                    else if(counter == 2 || counter == 18)
                        {
                            P1OUT = 0x03;
                        }
                    else if(counter == 3 || counter == 17)
                        {
                            P1OUT = 0x06;
                        }
                    else if(counter == 4 || counter == 16)
                        {
                            P1OUT = 0x0C;
                        }
                    else if(counter == 5 || counter == 15)
                        {
                            P1OUT = 0x18;
                        }
                    else if(counter == 6 || counter == 14)
                        {
                            P1OUT = 0x30;
                        }
                    else if(counter == 7 || counter == 13)
                        {
                            P1OUT = 0x60;
                        }
                    else if(counter == 8 || counter == 12)
                        {
                            P1OUT = 0xC0;
                        }
                    else if(counter == 9 || counter == 11)
                        {
                            P1OUT = 0x80;
                        }
                    counter++;
                    break;
                case 0x05:                                  // Pattern: Wig Wag #1
                    P1OUT = (wig_wag1 ^= 0x00FF);           // Toggle bits
                    break;
                case 0x06:                                  // Pattern: Rotate Left
                    P1OUT = rotate_pattern;                 // Set new pattern to the output variable
                    R_Right = rotate_pattern << 1;          // Rotate new variable to the left 1 bit
                    R_Left = rotate_pattern >> 7;           // Rotate same new variable to the right 7 bits
                    rotate_pattern = (R_Right | R_Left);    // Set the new pattern to the new combination
                    rotate_pattern &= 0x00FF;               // Set extra bits to ZERO
                    break;
                case 0x07:                                  // Pattern: Wig Wag #2
                    P1OUT = (wig_wag2 ^= 0x00FF);           // Toggle bits each time
                    break;
            } // end switch
        if(speed & BIT5)
            {
                __delay_cycles(70000);                      // fast
            }
        else
            {
                __delay_cycles(140000);                     // slow
            }
        } // end for loop
} // end main
