/**
 * Joel Brigida
 * CDA-3331C: Intro to Microprocessors
 * CDA-4630: Embedded Systems
 * This program uses two timers (A0 and A1) which creates two PWM signals
 * to drive two servos. Two analog values are read from a 2-D joystick 
 * to control the two servos. Contibutors to this program:
 * Dr. Bassem Alhalabi & Sam Rosenfield
 */

#include <msp430.h>
#include <math.h>
#include <stdlib.h>

unsigned int ADCReading[2];
double potvals[2];                                              // potentiometer values
double prevals[2];

void configureADC()                                             // set up the ADC
{
    ADC10CTL1 = (INCH_2 | CONSEQ_1);                		    // channels A0 to A2, single sequence
    ADC10CTL0 = (ADC10SHT_2 | MSC | ADC10ON | SREF_0);          // 16 clocks, multiple sample cov, ADC on, Ver GND-VCC
    ADC10DTC1 = 0x03;                             		        // 3 conversions
    ADC10AE0 |= (BIT1 | BIT2);                    		        // ADC10 option select
    ADC10SA = (unsigned)&ADCReading[0]; 				        // RAM Address of ADC Data, must be reset every conversion
}

void readPot()                                                  // read values from analog input joysticks
{
    int i = 0, j = 10;
    for (i = 0; i < j ; ++i)                 			        // read all three analog values j nmber of times
    {
        ADC10SA = (unsigned)&ADCReading[0]; 			        // RAM Address of ADC Data, must be reset every conversion
        ADC10CTL0 |= (ENC | ADC10SC);          			        // Start ADC Conversion
        
        while (ADC10CTL1 & BUSY);              			        // Wait while ADC is busy
        
        potvals[0] += ADCReading[0];
        potvals[1] += ADCReading[1];
    }
    
    potvals[0] /= j;
    potvals[1] /= j;                                            // Average the readings for the three variables
    __delay_cycles(100);
}

void setServos0()                                               // activate servo 0
{
    potvals[0] = ((potvals[0] - (677 - 512)) * 100) / (1024);   // Normalize pot values
    
    if(potvals[0] <= 1)
    {
        potvals[0] = 0;
    }
    else if (potvals[0] >= 99)
    {
        potvals[0] = 100;
    }

    TA0CCR0 = 10000;                                            // Total PWM period, 10000 microsecond
    TA0CCR1 = 7500 + potvals[0] * 20;                           // Range of PWM off portion between 7500-9500, making duty cycle 2500-500 microsecond
    TA0CCTL1 = OUTMOD_3;                                        // PWM output mode: 3 - PWM set/reset
    TA0CTL = TASSEL_2 + MC_1;                                   // SMCLK, Up to CCR0
    __delay_cycles(100);
}

void setServos1()                                               // activate servo 1
{
    potvals[1] = ((potvals[1] - (580 - 512)) * 100) / (1024);   // Normalize the pot values
    
    if(potvals[1] <= 1)
    {
        potvals[1] = 0;
    }
    else if (potvals[1] >= 99)
    {
        potvals[1] = 100;
    }

    TA1CCR0 = 10000;                                            // Total PWM period, 10000 microsecond
    TA1CCR1 = 7500 + potvals[1] * 20;                           // Range of PWM off portion between 7500-9500, making duty cycle 2500-500 microsecond
    TA1CCTL1 = OUTMOD_3;                                        // PWM output mode: 3 - PWM set/reset
    TA1CTL = TASSEL_2 + MC_1;                                   // SMCLK, Up to CCR0
    __delay_cycles(100);
}                                

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                   // Deactivate watchdog timer
    P1DIR = 0;
    P2DIR = 0;
    P1DIR |= BIT6;                                              // P1.6, 2.1 are outputs to the servos
    P2DIR |= BIT1;
    P1SEL |= BIT6;
    P2SEL |= BIT1;

    configureADC();
    __delay_cycles(100);

    for(;;)
    {
        potvals[0] = 0;
        potvals[1] = 0;
        readPot();

        potvals[0] = ADCReading[0];
        potvals[1] = ADCReading[1];
        __delay_cycles(1000);
        
        /* DEBUG
        if (abs((potvals[0] - prevals[0]) > 5))
        {
            prevals[0] = potvals[0];
            setServos0();
            __delay_cycles(10000);
        }
        if (abs((potvals[1] - prevals[1]) > 5))
        {
            prevals[1] = potvals[1];
            setServos1();
            __delay_cycles(10000);
        }        
        */

        setServos0();
        __delay_cycles(10000);
        
        setServos1();
        __delay_cycles(10000);
    }
}

#pragma vector = ADC10_VECTOR                                   // ISR Routine
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);
}