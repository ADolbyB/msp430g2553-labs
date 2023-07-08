/***********************************************************************
Joel Brigida
4/13/2021
CDA 3331C-001 - Intro to Microprocessors

Lab Exercise: Analog Inputs

This program performs the following functions:
    1) Demostrates the concept of hysteresis:
        - Hysteresis is a "Dead-Zone" concept which acts like a damper. Any value inside the selected band of
            values has no effect on output. While input fluctuates inside the band of values, the output
            remains unchanged. The output switches on when the input value crosses the UPPER threshold, and
            input value must drift down through the dead-zone and cross the LOWER threshold to get switched off. In
            order for the output to switch on again, it must drift back across the dead-zone and pass the UPPER
            threshold again.
    2) Reads 3 analog signals coming from 3 analog sensors:
        - Bare Metal Touch sensor: which when touched and then released toggles the RED LED on or off.
        - Light Sensor: lights a Yellow LED when the light over the sensor dims past the upper value threshold, and
            then turns off the Yellow LED when the lower value threshold is crossed.
            This variable "light" is inversely proportional to the amount of light hitting the sensor.
            In darkness, the "light" variable is high, In light, the "light variable is low.
        - Temperature Sensor: Lights up a Blue LED after the temperature rises 2% above the initially
            measured temperature of the room. This simulates the Air Conditioning turning on.
            When the temperature cools off to <= the original room temperature, the light turns off.
            the "Dead-Zone" is the 2% zone between the switch on threshold and the switch off threshold.
************************************************************************************************************/

#include <msp430.h>

int value = 0, i = 0;
int light = 0                                                           // continuously changing value used for instantaneous light value
int lightroom = 0;                                                      // baseline light value for the room stored as a constant before infinite loop
int dimled = 50;                                                        // value used to dim the LED on P1.6
int temp = 0;
int temproom = 0;                                                       // baseline temp value stored as a constant before infinite loop
int touch = 0;
int touchroom = 0;                                                      // baseline touch sensor value stored as a constant before infinite loop
int flag = 0;                                                           // touch sensor variable that toggles when sensor is touched and released.

int ADCReading[3];                                                      // array for the ANALOG values: [3] for 3 sensors

/** Function Prototypes **/
void fadeLED(int valuePWM);                                             // takes value for light to dim LED
void ConfigureAdc(void);                                                // configures Analog to Digital convertor
void getanalogvalues();                                                 // reads and triggers the ADC and saves them in the ADCReading Array

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                                           // Stop WDT
    P1OUT = 0;                                                          // CLEAR all outputs before beginning program
    P2OUT = 0;
    P1DIR = 0;
    P2DIR = 0;
    P1REN = 0;                                                          // DISABLE the Pullup / pulldown resistors
    P2REN = 0;

    P1DIR |= (BIT4 | BIT5 | BIT6);                                      // set bits 4, 5, 6 as outputs
    P2DIR |= BIT0;                                                      // set bit 0 as output

    ConfigureAdc();                                                     // Configure ADT before entering Inf Loop - # of channels used

    /** read and save the initial room values as constants: lightroom, touchroom, temproom **/

    __delay_cycles(250);                                                // Allow time for the hardware to settle
    getanalogvalues();                                                  // retrieve the initial baseline values
    lightroom = light;                                                  // backup initial values as baselines
    touchroom = touch;                                                  // remains fixed until next reset
    temproom = temp;
    __delay_cycles(250);                                                // allow time for hardware to settle

    for (;;)
    {
        /** read light, touch, and temp repeatedly at the beginning of the main loop **/

        getanalogvalues();                                              // read analog values and store in current variables
                                                                        // NO COMPARISON done here - just refreshing the values

        // light controlling Red LED2 on LaunchPad (P1.6) via variable "dimled"

        dimled = light;                                                 // light variable received from analog readings

        dimled = ((dimled - 350) * 100) / (750 - 350);                  // NORMALIZATION of values 0 - 100%

        if(dimled <= 0)                                                 // if value is out of range
        {
            dimled = 0;
        }
        else if (dimled >= 100)                                         // if value is out of range
        {
            dimled = 100;
        }

        fadeLED(dimled);                                                // Function takes the value of "dimled" and converts to PWM
                                                                        // changes brightness of RED LED2 on the board

        /*********************************************************************************
        - Light Controlling LED1 on breadboard with hysteresis
        
        NOTE: Integer Value of "light" is inversely proportional with the amount of 
        light on the sensor. Voltage to P1.2 of MSP430 increases as amount of light on 
        sensor decreases.

        **********************************************************************************/

        /** Optimized Code for section 1/3 - light sensor switches on and off with hysteresis **/

        if (light > (lightroom * 1.1) && light < (lightroom * 1.8))     // Hysteresis Dead-Zone
        {
            // DO NOTHING: This is a Dead-Zone for Hysteresis
            // note that this if statement can be eliminated to a single if / else if pair
        }
        else if (light <= (lightroom * 1.1))                            // If brighter than dead zone threshold
        {
            P1OUT &= ~BIT4;                                             // Turn OFF Yellow LED
            __delay_cycles(200);
        }
        else if (light >= (lightroom * 1.8))                            // If darker than dead zone threshold
        {
            P1OUT |= BIT4;
            __delay_cycles(200);                                        // Turn ON Yellow LED
        }

        /********************************************************************************
        - Temperature Controlling LED2 - Blue LED

        NOTE: integer "temp" increases proportionately with actual temperature of the sensor.
        Voltage to MSP430 P1.0 Increases with actual temperature of sensor

        *********************************************************************************/

        if (temp >= (temproom * 1.01) && (temp <= (temproom * 1.04)))   // If the temp is between 1% and 4% of initial value
        {
            // DO NOTHING: This is a Dead-Zone for Hysteresis
        }
        else if (temp > (temproom * 1.04))                              // If the temperature is 4% warmer than the initial value
        {
            P1OUT |= BIT5;                                          // Turn Blue LED ON
            __delay_cycles(200);
        }
        else // if (temp < (temproom * 1.01))                           // If the temperature is lower than the Dead-Zone
        {
            P1OUT &= ~BIT5;                                         // Turn Blue LED OFF
            __delay_cycles(200);
        }

        /********************************* section 3/3 *********************************
        - Touch Controlled LED3

        The following code uses a dead zone of no action between 0.7-0.9 of the value touch.
        When the touch sensor is touched and released, the LED toggles from ON to OFF or 
        from Off to ON. Each touch and release causes a toggle event.

        *********************************************************************************/

        if (touch < touchroom * 0.78)                                   // when the value of touch is approx < 800, toggle flag value
        {                                                           // "touch" fluctuates from approx 160 - 750 when touched
            if (flag == 0)                                          // Hysteresis "dead zone" approx = (800 < touch < 1023)
            {
                flag = 1;
            }
            else // if (flag != 0)                                  // This section switches the control variable for the LED
            {
                flag = 0;
            }

            __delay_cycles(1000);                                   // 0.001 second delay

            while (touch < (touchroom * 0.78))                      // while your finger is on the touch sensor
            {
                getanalogvalues();                              // program "hangs" here until finger released from sensor.
                __delay_cycles(10000);                          // 0.01 second delay
            }

            if (flag == 0)                                          // This section of code toggles the LED based on control variable
            {
                P2OUT &= ~BIT0;                                 // TURN LED off
                __delay_cycles(50000);                          // 0.05 second delay
            }
            else // if (flag == 1)
            {
                P2OUT |= BIT0;                                  // TURN LED on
                __delay_cycles(50000);                          // 0.05 second delay
            }
        }
    }   // END for(;;)
}   // END main()

/** Begin Function Definitions **/
void ConfigureAdc(void)
{
   ADC10CTL1 = INCH_2 | CONSEQ_1;                                       // A2 + A1 + A0, single sequence
   ADC10CTL0 = ADC10SHT_2 | MSC | ADC10ON;
   while (ADC10CTL1 & BUSY);
   ADC10DTC1 = 0x03;                                                    // 3 conversions
   ADC10AE0 |= (BIT0 | BIT1 | BIT2);                                    // ADC10 option select
}

void fadeLED(int valuePWM)
{
    P1SEL |= (BIT6);                                                    // P1.0 and P1.6 TA1/2 options
    CCR0 = 100 - 0;                                                     // PWM Period
    CCTL1 = OUTMOD_3;                                                   // CCR1 reset/set
    CCR1 = valuePWM;                                                    // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1;                                            // SMCLK, up mode
}

void getanalogvalues()
{
    i = 0;
    temp = 0;                                                           // set all analog values to zero
    light = 0;
    touch = 0;

    for(i = 1; i <= 5 ; i++)                                            // read all three analog values 5 times each and average
    {
        ADC10CTL0 &= ~ENC;

        while (ADC10CTL1 & BUSY);                                   // Wait while ADC is busy
        ADC10SA = (unsigned)&ADCReading[0];                         // RAM Address of ADC Data, must be reset every conversion
        ADC10CTL0 |= (ENC | ADC10SC);                               // Start ADC Conversion

        while (ADC10CTL1 & BUSY);                                   // Wait while ADC is busy
        light += ADCReading[0];                                     // sum all 5 reading for the three variables
        touch += ADCReading[1];
        temp += ADCReading[2];
    }

    light = light / 5;                                                  // Average the 5 readings to reduce fluctuations
    touch = touch / 5;                                                  // capacitors also help
    temp = temp / 5;                                                    // Average the 5 reading for the three variables
}

#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);
}