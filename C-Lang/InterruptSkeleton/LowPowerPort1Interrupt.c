/**
 * Joel Brigida
 * CDA-3331C: Intro to Microprocessors.
 * This is a small code sample which enables Low Power Mode
 * and uses an interrupt to wake MCU, turning on the green
 * LED2 and blinking the red LED1 when BUTTON P1.3 is pressed.
*/

#include <msp430g2553.h>

#define LED1 BIT0
#define LED2 BIT6
#define BUTTON BIT3

unsigned int folds = 0;
unsigned int switches = 0;
int i = 1;
int j = 1;

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer
    P1OUT  = 0x00;                          // Turn off all outputs initially
    
    P1DIR |= (LED1 | LED2 );                // set P1.0 (LED1) and P1.6 (LED1) to outputs
    P1REN |= BUTTON;                        // enable resistor on the button pin
    P1OUT |= BUTTON;                        // resistor becomes a pull up
    P1IES |= BUTTON;                        // interrupt edge select: HIGH to LOW
    P1IE  |= BUTTON;                        // enable interrupt on selected pin, BUTTON
    __enable_interrupt();                   // enable interrupts

    for (;;)                                // main loop, sleep only
    { 
        LPM3;                               // Set controller into sleep mode & keep osc. running
    }
}   // end main()

#pragma vector = PORT1_VECTOR               // Port 1 interrupt service routine
__interrupt void Port_1(void)
{
    P1OUT |=  LED2; __delay_cycles(10000);  // Turn LED2 (green) on

    folds ++;
    if (folds >= 4)                         // cycle folds for 1,2,3
    {
        folds =1;
    }
    
    for (j = 1; j <= 6; j++)                // run toggle logic 6 times
    {
        P1OUT ^= (LED1);                    // toggle P1.0 using XOR
        for(i = 1; i <= folds; i++)         // delay folds number of times
        {
            __delay_cycles(100000);
        }
    }
    
    __delay_cycles(100);				    // allow dome delay
    P1IFG &= ~BUTTON;  					    // P1.3 IFG cleared
    __delay_cycles(10000);                  // allow dome delay before turning LED2 off
    P1OUT &= ~LED2;                         // Turn off Green LED.
    __delay_cycles(100);                    // end of interrupt subroutine
}