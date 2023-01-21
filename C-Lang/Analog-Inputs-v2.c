/*********************************************************************************************************
Joel Brigida
4/13/2021
CDA4630-002: Embedded Systems

ADC with temp, light, touch sensors, PWM and motor control
*********************************************************************************************************/

#include <msp430.h>

int i = 0;
int light = 0, lightroom = 0, dimled = 50;
int temp = 0, temproom = 0;
int touch = 0, touchroom = 0;
int flag = 0, direction = 0;            // variables for the motor movement
int ADCReading[3];
int tempthld = 0;                       // threshold value for the temp sensor

void fadeLED(int valuePWM);             // Function Prototypes
void ConfigureAdc(void);
void getanalogvalues();

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;           // Stop Watchdog Timer
    P1OUT = 0;                          // set default values to all zeros for port 1
    P1DIR = 0;                          // set port direction to all inputs for port 1
    P2OUT = 0;                          // set default values to all zeros for port 2
    P2DIR = 0;                          // set port direction to all inputs for port 2

    P1REN = 0;                          // disable pull up/down resistors for port 1
    P2REN = 0;                          // disable pull up/down resistors for port 2

    P1DIR |= ( BIT4 | BIT5 | BIT6 );    // set port 1 bits 4, 5, 6 as outputs
    P2DIR |= ( BIT0 | BIT1 );           // set port 2 bits 0, 1    as outputs

    P1OUT &= ~BIT4;                     // Start motor in FW direction
    P1OUT |= BIT5;
    P1OUT |= BIT6;                      // P1.6 is for the Redboard LED dimmer and Motor speed "dimmer"
    P2OUT &= ~BIT0;                     // Turn temp LED off before starting the loop.
    __delay_cycles(2000000);            // Wait 2 seconds for voltages settle before MSP430 starts reading analog values

    ConfigureAdc();
    __delay_cycles(250);

    getanalogvalues();                  // read & set initial room values: lightroom, touchroom, temproom
    lightroom = light;                  // baseline values for comparison to measurements
    touchroom = touch;
    temproom = temp;
    __delay_cycles(250);

    for (;;)
    {

        getanalogvalues();  // read light, touch, & temp repeatedly at beginning of infinite loop

        dimled = light;     // light controlling LED2 on P1.6 via variable dimled

        // light reading range min = 350, max = 750 (integer ADC value), & convert them to 0 - 100%
        dimled = ((dimled - 350) * 100) / (750 - 350);      // NORMALIZATION of values 0 - 100%

        if (dimled <= 0)                        // if value is out of range
            {
                dimled = 0;
            }
        else if (dimled >= 100)                 // if value is out of range
            {
                dimled = 100;
            }

        fadeLED(dimled);                        // Function takes int value of "dimled" & converts to PWM value
                                                // changes brightness of RED LED2 (P1.6) on board
        
        /****************************************************************************************
        
        NOTE: Integer Value of "light" is inversely proportional with the amount of light on the sensor.
        Voltage to P2.1 of MSP430 increases as amount of light on sensor decreases.
        Integer value of "light" DECREASES with INCREASE of light on sensor.

        ****************************************************************************************/

        if (light > (lightroom * 1.1) && light < (lightroom * 1.8))
            {
                // DO NOTHING: This is a Dead-Zone for Hysteresis
                // note that this if statement can be eliminated to a single if / else if pair
            }
        else if (light >= lightroom * 1.8)
            {
                P2OUT |= BIT1;          // Red Center RGB LED on if dark (Tied to P2.1)
                __delay_cycles(200);
            }
        else if (light <= lightroom * 1.2)
            {
                P2OUT &= ~BIT1;         // Red Center RGB LED off if light
                __delay_cycles(200);
            }
        
        /**************************************************************

        Temperature Controlled LED w/ hysteresis
        
        if temp is higher than 1.04 * temproom, turn LED on
        if temp is lower  than 1.02 * temproom, turn LED off

        ***************************************************************/

        if (temp >= (temproom * 1.02) && (temp <= (temproom * 1.04)))
            { // If the temp is between 2% and 4% of initial value
                // DO NOTHING: This is a Dead-Zone for Hysteresis
            }
        else if (temp > (temproom * 1.04))      // If the temperature is 4% warmer than the initial value
            {
                P2OUT |= BIT0;                  // Turn Blue LED ON
                __delay_cycles(200);
            }
        else // if (temp < (temproom * 1.02))   // If the temperature is lower than the Dead-Zone
            {
                P2OUT &= ~BIT0;                 // Turn Blue LED OFF
                __delay_cycles(200);
            }

        /*************************************************************************
        
        Touch Controlled Motor Direction:

        Each time the touch sensor is touched and released, the motor direction
        will toggle to spin the oppposite direction.
        
        *************************************************************************/

        if (touch <= touchroom * 0.78)
            {
                flag = 1;                               // represents sensor is PRESSED
                __delay_cycles(200);
            }

        else // if (touch > touchroom * 0.78)
            {
                flag = 0;                               // represents sensor NOT PRESSED
                direction = P1OUT;                      // reset value of "direction" to current value
                __delay_cycles(200);
            }

        if (flag == 1)                                  // if sensor is touched
            {
                if ((direction & BIT4) != BIT4)         // if motor is moving FORWARD
                    {
                        P1OUT |= BIT4;
                        P1OUT &= ~BIT5;                 // set motor in REVERSE direction
                        __delay_cycles(200);
                    }

                else if ((direction & BIT4) == BIT4)    // if motor moving in REVERSE
                    {
                        P1OUT &= ~BIT4;
                        P1OUT |= BIT5;                  // set motor in FORWARD direction
                        __delay_cycles(200);
                    }
            }
    } // end infinite for loop
} // end main

void ConfigureAdc(void)
{
    ADC10CTL1 = INCH_2 | CONSEQ_1;                  // A2 + A1 + A0, single sequence
    ADC10CTL0 = ADC10SHT_2 | MSC | ADC10ON;

    while (ADC10CTL1 & BUSY);

    ADC10DTC1 = 0x03;                               // 3 conversions
    ADC10AE0 |= (BIT0 | BIT1 | BIT2);               // ADC10 option select
}

void fadeLED(int valuePWM)
{
    P1SEL |= (BIT6);                                // P1.0 and P1.6 TA1/2 options
    CCR0 = 100 - 0;                                 // PWM Period
    CCTL1 = OUTMOD_3;                               // CCR1 reset/set
    CCR1 = valuePWM;                                // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1;                        // SMCLK, up mode
}

void getanalogvalues()
{
    i = 0; temp = 0; light = 0; touch = 0;          // set all analog values to zero
    for (i = 1; i <= 5 ; i++)                       // read all three analog values 5 times each and average
        {
            ADC10CTL0 &= ~ENC;

            while (ADC10CTL1 & BUSY);               // Wait while ADC is busy

            ADC10SA = (unsigned)&ADCReading[0];     // RAM Address of ADC Data, must be reset every conversion
            ADC10CTL0 |= (ENC | ADC10SC);           // Start ADC Conversion

            while (ADC10CTL1 & BUSY);               // Wait while ADC is busy

            light += ADCReading[0];                 // sum 5 reading for the 3 variables
            touch += ADCReading[1];
            temp += ADCReading[2];
        }
    light = light / 5;                              // Average 5 readings for 3 variables
    touch = touch / 5;
    temp = temp / 5;
}

#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);
}
