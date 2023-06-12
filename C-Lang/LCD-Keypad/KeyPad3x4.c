/**
 * Joel Brigida
 * CDA-3331C: Intro to Microprocessors
 * This is a small program that checks for user keypad input
 * from a 12 button 3x4 keypad. Pressing a button on the keypad
 * triggers an interrupt and the value of that button is stored in
 * the global variable `keynum`
*/

#include <msp430g2553.h>
#include <stdio.h>

#define OTROWS  P1OUT
#define ROW1    BIT0
#define ROW2    BIT1
#define ROW3    BIT2
#define ROW4    BIT3
#define ALLROWS (ROW4 | ROW3 | ROW2 | ROW1)

#define INCOLS  P1IN
#define COL1    BIT4
#define COL2    BIT5
#define COL3    BIT6
#define ALLCOLS (COL3 | COL2 | COL1)

int ROWS;
int rownum;
int keynum;                                     // Stores value of user key press   

int main()
{
    WDTCTL =  WDTPW + WDTHOLD;                  // Stop Watch Dog Timer
    P1DIR  =  0x00;                             // Set port 1 all inputs
    
    P1DIR |=  ALLROWS;                          // Set all rows as output
    P1REN |=  ALLCOLS;                          // Enable resistors for all columns
    P1OUT |=  ALLCOLS;                          // pullup resistors
    P1IES |=  ALLCOLS;                          // Interrupt edge select, HIGH to LOW
    P1IE  |=  ALLCOLS;                          // Enable interrupts on all columns
    P1IFG &= ~ALLCOLS;                          // Clear all interrupt flags on the columns before starting.
    __enable_interrupt();                       // enable interrupts
    
    OTROWS &= ~(ALLROWS);                       // Assert low on all rows to detect any key stroke interrupt
    __delay_cycles(200);

    for(;;)
    {
        // main loop empty 
    }
}

#pragma vector = PORT1_VECTOR                   // Port 1 interrupt service routine
__interrupt void Port_1(void)
{
    OTROWS |= ALLROWS;                          // Make all rows high
    __delay_cycles(200);
    
    keynum = 0;
    for(rownum = 0; rownum <= 3; rownum++)      // Start scanning the KP
    {
        OTROWS = ((ROW1 << rownum) ^ ALLROWS);  // start a 1 in first row, shift left `rownum` times & XOR
        __delay_cycles(200);
        
        if(!(INCOLS & COL1))
        {
            keynum = 3 * rownum + 1;
        }
        if(!(INCOLS & COL2))
        {
            keynum = 3 * rownum + 2;
        }
        if(!(INCOLS & COL3))
        {
            keynum = 3 * rownum + 3;
        }
    }

    OTROWS &= ~ALLROWS;                         // Make all rows low for for next key stroke interrupt
    __delay_cycles(200);
    P1IFG  &= ~ALLCOLS;                         // Clear the interrupt flags
    __delay_cycles(200);
}