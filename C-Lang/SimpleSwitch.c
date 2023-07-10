/**
 * Joel Brigida
 * CDA-3331C: Microprocessors
 * This is a simple sketch that toggles 2 LEDs (good for testing)
*/

#include <msp430.h> 
#include "msp430g2553.h"

int sw2 = 0;                                            // declaration for switch2

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                           // Stop watchdog timer

    P1DIR  = 0x00;                                      // Port 1 all inputs
    P1DIR  = (BIT0 | BIT6);                             // Port 1.0 and 1.6 out pututs for the LEDs
    P1REN |= BIT3;                                      // Activate resister on P1.3
    P1OUT |= BIT3;                                      // Make it pull up. becuase SW1 asserts a 0

    for(;;)
    { 
        sw2 = P1IN;                                     // read complete Port1
        sw2 &= BIT3;                                    // mask out only BIT 3 where SW1 is connected
        
        if (sw2 == BIT3)                                // if SW2 is high toggle BIT 0 which is LED1
        {
            P1OUT &= ~BIT6;
            P1OUT ^= BIT0;
            __delay_cycles(50000);
        }
        else                                            // if SW2 is low toggle BIT 6 which is LED2
        {
            P1OUT &= ~BIT0;
            P1OUT ^= BIT6;
            __delay_cycles(200000);
        }
    }
}