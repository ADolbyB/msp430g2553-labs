/**
 * Joel Brigida
 * CDA-3331C: Microprocessors.
 * This is a small code sample which flashes an LED using a sensor
*/

#include <msp430g2553.h>

#define LED1 BIT0
#define LED2 BIT6
#define BUTTON BIT3

unsigned int folds = 1;
int i = 1;

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; 	                            // Stop watchdog timer

    P1OUT  = 0x00;				                            // make sure all default outputs are zero
    P1DIR  = (LED1 | LED2); 	                            // set P1.0 and P1.6 to output direction

    P1REN |= BUTTON; 			                            // enables eesistor on the button pin
    P1OUT |= BUTTON; 			                            // make that a pull up
    P1IES |= BUTTON; 			                            // interrupt edge selec, hi to lo
    P1IE  |= BUTTON; 			                            // enable interrupt on selected pin
    __enable_interrupt(); 		                            // enable interrupts

    for (;;)
    {
        P1OUT ^= (LED2); 	                                // toggle P1.0 using XOR
        for(i = 1; i <= folds; i++)
        {
            __delay_cycles(100000);                         // delay `folds` number of times
        }
    }
}


#pragma vector = PORT1_VECTOR                               // Port 1 interrupt service routine
__interrupt void Port_1(void)
{
    P1OUT |= LED1;											// LED1 on to indicate interrupt service
    
    folds ++; 
    if (folds >= 4) 
    {
        folds = 1;
    }
    
    __delay_cycles(20000);									// allow some delay for switch debounce
    P1IFG &= ~BUTTON;  										// P1.3 IFG cleared
    __delay_cycles(20000);									// allow some delay for switch debounce
    P1OUT &= ~LED1;											// LED1 off to indicate end of interrupt service
}