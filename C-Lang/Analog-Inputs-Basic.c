/***********************************************************************
Joel Brigida
4/13/2021
CDA 3331C-001 - Intro to Microprocessors

Lab Exercise: Analog Inputs. This is a BASIC setup to test analog inputs
************************************************************************/

#include <msp430.h>

int value = 0, i = 0 ;
int light = 0, lightroom = 0, dimled = 50;
int temp = 0, temproom = 0;
int touch = 0, touchroom = 0;
int flag = 0;
int ADCReading [3];

// Function Prototypes
void fadeLED(int valuePWM);
void ConfigureAdc(void);
void getanalogvalues();

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                                           // Stop WDT
    P1OUT = 0;
    P2OUT = 0;
    P1DIR = 0;
    P1REN = 0;
    P2REN = 0;
    P2DIR = 0;
    P1DIR |= (BIT4 | BIT5 | BIT6);                                      // set bits 4, 5, 6 as outputs
    P2DIR |= BIT0;                                                      // set bit 0 as output

    ConfigureAdc();

    __delay_cycles(250);
    
    getanalogvalues();                                                  // read initial room values: lightroom, touchroom, temproom
    
    lightroom = light; 
    touchroom = touch; 
    temproom = temp;
    
    __delay_cycles(250);

    for (;;)
    {
        getanalogvalues();                                              // reading light, touch, and temp repeatedly at the beginning of the main loop


        dimled = light;                                                 // light controlled LED2 on launch pad (P1.6) via variable dimled
        dimled = ((dimled - 100) * 100) / (1000 - 100);                 // signal normalization
        
        if(dimled <= 5)
        {
          dimled = 0; 
        }
        else if(dimled >=95)
        {
          dimled = 100;
        }
        
        fadeLED(dimled);

        /* Ambient light Controls LED1 brightness of on the breadboard */
        if(light < lightroom * 1.80 && light > lightroom * 1.20)
        {
            // Dead Zone: Do nothing
        }
        else
        {
            if(light >= lightroom * 1.80)
            {
              P1OUT |=  BIT4; __delay_cycles(200);                      // on if dark
            }
            if(light <= lightroom * 1.20)
            {
              P1OUT &= ~BIT4; __delay_cycles(200);                      // off if light
            }
        }

        /* Temperature Controlled LED2 */ 
        if(temp > temproom * 1.03)
        { 
            P1OUT |=  BIT5;
            __delay_cycles(200); 
        }
        else
        {
            P1OUT &= ~BIT5;
            __delay_cycles(200); 
        }
        // Touch Controlling LED3
        // The following code uses a dead zone of no action between 0.7-0.9 of the value touch
        // This makes a simple turn-on while when touched, and a simple turn-off when not touched.

        if(touch > touchroom * 0.7 && touch < touchroom * 0.9)
        {
            // Dead Zone: Do nothing
        }
        else
        {
            if(touch >= touchroom * 0.9)
            {
                P2OUT &= ~BIT0; __delay_cycles(200);                    // LED off
            }
            if(touch <= touchroom * 0.7)
            {
                P2OUT |=  BIT0; __delay_cycles(200);                    // LED on
            }
        }
    }
}

void ConfigureAdc(void)
{
    ADC10CTL1 = INCH_2 | CONSEQ_1;                                      // A2 + A1 + A0, single sequence
    ADC10CTL0 = ADC10SHT_2 | MSC | ADC10ON;
    while (ADC10CTL1 & BUSY);
    ADC10DTC1 = 0x03;                                                   // 3 conversions
    ADC10AE0 |= (BIT0 | BIT1 | BIT2);                                   // ADC10 option select
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
    i = 0; temp = 0; light = 0; touch = 0;                              // set all analog values to zero

    for(i = 1; i <= 5; i++)                                             // read all three analog values 5 times each and average
    {
        ADC10CTL0 &= ~ENC;
        while (ADC10CTL1 & BUSY);                                       // Wait while ADC is busy
        ADC10SA = (unsigned)&ADCReading[0];                             // RAM Address of ADC Data, must be reset every conversion
        ADC10CTL0 |= (ENC | ADC10SC);                                   // Start ADC Conversion
        while (ADC10CTL1 & BUSY);                                       // Wait while ADC is busy
        light += ADCReading[0];                                         // sum all 5 reading for the three variables
        touch += ADCReading[1];
        temp += ADCReading[2];
    }

    // Average the 5 readings for the three variables
    light = light / 5; touch = touch / 5; temp = temp / 5;
}

#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);
}