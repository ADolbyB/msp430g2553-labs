/***********************************************************************
Joel M Brigida
April 7, 2021
LED Segment Display
CDA-3331C-001: Intro to Microprocessors

Description of Code Function:
SW-321 = 000: Counter resets to 00
SW-321 = 001: Right digit cycles 0-9
SW-321 = 010: Left digit cycles 0-9
SW-321 = 011: R & L digits both hold & store current values.
SW-321 = 101: R & L digits are restored to preset values for counting up / dowm
SW-321 = 110: R & L digits are restored to preset values for counting up / dowm
SW-321 = 100: Counter cycles up from preset values to 90, then repeats
SW-321 = 111: Counter cycles down from preset values to 10, then repeats
************************************************************************/
#include <msp430.h> 

// Digit configuration, make sure segments h-a are connected to PORT1 pins 7-0
// also besides digits 0-9, the single segments a/b/c/d/e/f/g/ produce a rotating pattern
int LEDS[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x01,0x02,0x04,0x08,0x10,0x20,0x40};

int switches = 0;                                                       // takes switch position values from P2IN
int leftdigit = 0, rightdigit = 0;                                      // array index # displayed on the LED 7-segment display
int pleftdigit = 0, prightdigit = 0;                                    // preset values stored @ SW-321 = 011
unsigned one_time = 0;                                                  // counter for one_time use
int flag = 0;                                                           // used in the interrupt loop

int main(void)
{
    //Ports set up, simple and direct
    P1DIR = 0xFF;                                                       // port 1 all output
    P2DIR = 0x03;                                                       // port 2 all inputs, except BIT0 and BIT1
                                                                        // (P2 BIT0 / BIT1 switch the common anodes for 7 segment LED Display)

    //P2REN = 0x38;                                                     // enable resistors on P2.3, P2.4, P2.5
    //P2OUT = 0x38;                                                     // make the resistors pull ups

    // Sub Master clock dividers: DIVS_0 (/1 for 1000 Hz), DIVS_1 (/2 for 500 Hz), DIVS_2 (/4 for 250 Hz), DIVS_3 (/8 for 125 Hz)
    // DIVS_0 means divide by 1, so the SMCLK = MCLK/1, that 1M Hz or 1 millisecond for period.
    // Ex: changing DIVS_0 to DIVS_2 will divide the freq by 4, & the digits will start flickering.
    BCSCTL2 |= DIVS_0;

    // WDT set up to generate a constant interrupt to display multiplexing
    // we chose to generate an interrupt interval every 8*1000 pulses from the SMCLK; other values: WDT_MDLY_32, WDT_MDLY_0_5, etc.
    // so the interrupt frequency is 1,000,000 Hz/(8,000+8,000) = 62.5 Hz, meaning the display is refreshed about 62 times every second
    // if you divide the SMCLK frequency by 4, the digit display multiplexing will drop to 15, which will make flickering easy to see
    WDTCTL = WDT_MDLY_8; // Try later as: WDT_MDLY_32

    IE1 |= WDTIE;                                                       // WDT Interrupt Enable
    __enable_interrupt();                                               // enable general interrupts

    for (;;)
    {
        // read switch status for Port 2, and then evaluate switch combinations on BIT5/BIT4/BIT3
        // switches = ~P2IN;                                            // if wired as active high
        switches = P2IN;                                                // if wired as active low

        if (((switches & BIT5) != BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) != BIT3))
        {
            // SW-321 = 000: left and right digits reset to 00
            leftdigit = 0;
            rightdigit = 0;
            one_time = 0;                                               // set one_time counter to 0 when the display is "00"
        }
        if (((switches & BIT5) != BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) == BIT3))
        {
            // SW-321 = 001: right digit counts up
            rightdigit++;
            if (rightdigit >= 10)
            {
                rightdigit = 0;
            }
        }
        if (((switches & BIT5) != BIT5) && ((switches & BIT4) == BIT4) && ((switches & BIT3) != BIT3))
        {
            // SW-321 = 010: left digit counts up
            leftdigit++;
            if (leftdigit >= 10)
            {
                leftdigit = 0;
            }
        }
        if (((switches & BIT5) != BIT5) && ((switches & BIT4) == BIT4) && ((switches & BIT3) == BIT3))
        {
            // SW-321 = 011: right and left digits both hold values (preset value)
            pleftdigit = leftdigit;                                     // save preset left digit
            prightdigit = rightdigit;                                   // save preset right digit
            one_time = 0;
        }
        if (((switches & BIT5) == BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) == BIT3))
        {
            // SW-321 = 101: recalls preset values from 011
            leftdigit = pleftdigit;                                     // recall preset left digit
            rightdigit = prightdigit;                                   // recall preset right digit
        }
        if (((switches & BIT5) == BIT5) && ((switches & BIT4) == BIT4) && ((switches & BIT3) != BIT3))
        {
            // SW-321 = 110: also recalls preset values from 011
            leftdigit = pleftdigit;                                     // recall preset left digit
            rightdigit = prightdigit;                                   // recall preset right digit
        }
        if (((switches & BIT5) == BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) != BIT3))
        {
            // SW-321 = 100: counter cycles up from preset value to 90 and repeats
            if (one_time == 0)                                          // 1st activation of SW-321 = 100 after PRESET value stored
            {
                leftdigit = pleftdigit;
                rightdigit = prightdigit;
            }
            else if (rightdigit == 0 && (leftdigit == 9 || leftdigit == 0))
            {
                // resets the counter digits to PRESET VALUE if display reads "00" or "90"
                leftdigit = pleftdigit;
                rightdigit = prightdigit;
            }
            else                                                        // every other case: advance the counter
            {
                rightdigit++;
            }

            if (rightdigit >= 10)                                       // reset the right digit and advance the left digit
            {
                rightdigit = 0;
                leftdigit++;
            }

            one_time++;                                                 // 1st cycle counter
            if (one_time > 1000)
            {   
                one_time = 1;                                           // optional to keep "one_time" from overflow
            }
        }
        if (((switches & BIT5) == BIT5) && ((switches & BIT4) == BIT4) && ((switches & BIT3) == BIT3))
        {
            // SW-321 = 111: count down from preset value to 10, then repeat
            if (one_time == 0)                                          // 1st activation of SW-321 = 111 after PRESET value stored
            {
                rightdigit = prightdigit;
                leftdigit = pleftdigit;
            }

            rightdigit--;                                               // count down right digit
            one_time++;                                                 // 1st cycle counter

            if (one_time > 1000)                                        // optional to keep counter value from overflowing
            {
                one_time = 1;
            }

            if (rightdigit <= -1)
            {
                rightdigit = 9;                                         // reset right digit to 9 after decrementing left digit
                leftdigit--;                                            // decrement left digit after right digit = 0;
            }

            if (leftdigit == 0 && rightdigit == 9)
            {
                // resets the counter digits to PRESET VALUE if display reads "00" or "10"
                rightdigit = prightdigit;
                leftdigit = pleftdigit;
            }
        }
        // delay determines the speed of changing numbers on the 7-segment display (in milliseconds)
        __delay_cycles (300000); // delay for main loop has no effect on the interrupt frequency.
    } // end for(;;)
} // end main()

// WDT interrupt service routine
#pragma vector = WDT_VECTOR
__interrupt void WDT(void)
{
    // This executes every time there is an interrupt from WDT
    // The frequency of this interrupt is about 62.5 Hz which is enough to eliminate display flicker
    // right & left digits are displayed alternately
    // Note that both digits must be turned off to avoid aliasing

    // Display code for Common-Anode display
    P1OUT = 0xFF; // switch off display temporarily to prevent aliasing
    P2OUT = 0xFF; // switch off display temporarily to prevent aliasing
    __delay_cycles (100);

    // alternate left and right digit on display based on the flag value
    if (flag == 0)
    {
        P2OUT &= ~BIT0;
        P1OUT = ~LEDS[leftdigit];                                       // display left digit and change flag to 1
        flag = 1;
    }
    else
    {
        P2OUT &= ~BIT1;
        P1OUT = ~LEDS[rightdigit];                                      // display right digit and change flag to 0
        flag = 0;
    }
    __delay_cycles (100);
    
    /******************************************
    // Display code for Common-Cathode display:
    P1OUT = 0; 
    P2OUT = 0; 
    
    __delay_cycles (100);
    
    if (flag == 0) 
    { 
        P2OUT |= BIT0; 
        P1OUT = LEDS[leftdigit]; 
        flag = 1; 
    }
    
    else 
    { 
        P2OUT |= BIT1; 
        P1OUT = LEDS[rightdigit]; 
        flag = 0; 
    }
    __delay_cycles (100);
    *******************************************/
}