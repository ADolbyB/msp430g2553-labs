/*********************************************************
Joel Brigida
CDA-3331C - Intro to Microprocessors
3/25/2021

This is test code test Input and Outputs (without using interrupts)
***********************************************************/

#include <msp430.h> 
#include "msp430g2553.h"

int sw2 = 0;                                        // switch 2: declares a swtich variable. The value of 0 is
                                                    // overridden in line 26: "sw2 = P1IN"
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                       // stop watchdog timer WDTCTL

    P1DIR = 0x00;                                   // port 1 all inputs set to zero: keep random output from happening
    P1DIR |= (BIT0 | BIT6);                         // set P1.0 and P1.6 as outputs (LED1, LED2)
    P1REN |= BIT3;                                  // activate resistor on P1.3 (Set BIT3 to HIGH)
    P1OUT |= BIT3;                                  // activate pull up resistor: SW2 is active low (Hardware on Board)

    for(;;)
        {
            sw2 = P1IN;                             // read values from P1
            sw2 &= BIT3;                            // mask out only BIT3 where SW2 is connected

            if (sw2 == BIT3)                        // BIT3 = 0x08: if SW2 is high
                {
                    P1OUT &= ~BIT6;                 // Bitwise AND: turn LED2 off
                    P1OUT ^= BIT0;                  // Bitwise XOR toggles LED1 to next state on next iteration of the loop
                    __delay_cycles(100000);         // delay in microseconds
                }
            else                                    // else (if SW2 is low)
                {
                    P1OUT &= ~BIT0;                 // Bitwise AND: turn LED1 off
                    P1OUT ^= BIT6;                  // Bitwise XOR: toggles LED2 to next state on next iteration of the loop
                    __delay_cycles(100000);         // delay in microseconds
                }
        }   // end infinite loop
}// end main