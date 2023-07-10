/**
 * Joel Brigida
 * CDA-3331C: Microprocessors
 * This is a simple sketch for MSP430 that 
*/

#include <msp430.h> 

int a = 10;                                 // 16 bit number
long int b = 0;                             // 32 bit number
char c = 'c';                               // 16 bit, 0x63 in ASCII
float d = -255.25;                          // 64 bit number

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    P1DIR |= BIT0;
	
	for(;;)
	{
		b = b + 1;                          // increment variable
		P1OUT ^= BIT0;                      // Toggle the LED on BIT0
	}
}