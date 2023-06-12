/**
 * Joel Brigida
 * CDA-3331C: Microprocessors.
 * This is a small code sample which flashes the green LED 
 * continuously, and uses an ISR to 
 * triggered by the button on Port P1.3
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
    P1OUT  = 0x00;				                            // set initial outputs to off.
    
    P1DIR  = (LED1 | LED2); 	                            // set P1.0 and P1.6 to outputs
    P1REN |= BUTTON; 			                            // enable resistor on button pin
    P1OUT |= BUTTON; 			                            // pull up resistor for button
    P1IES |= BUTTON; 			                            // interrupt edge select: HIGH to LOW
    P1IE  |= BUTTON; 			                            // enable interrupt on selected pin
    __enable_interrupt(); 		                            // enable interrupts

    for (;;)
    {
        P1OUT ^= (LED2); 	                                // toggle P1.6 LED (Green) using XOR
        for(i = 1; i <= folds; i++)
        {
            __delay_cycles(100000);                         // delay `folds` number of times
        }
    }
}

#pragma vector = PORT1_VECTOR                               // Port 1 interrupt service routine
__interrupt void Port_1(void)
{
    P1OUT |= LED1;											// LED1 (Red) on to indicate ISR tripped
    
    folds ++; 
    if (folds >= 4) 
    {
        folds = 1;
    }
    
    __delay_cycles(20000);									// allow some delay for switch debounce
    P1IFG &= ~BUTTON;  										// P1.3 IFG cleared
    __delay_cycles(20000);									// allow some delay for switch debounce
    P1OUT &= ~LED1;											// LED1 (Red) off to indicate end of ISR
}