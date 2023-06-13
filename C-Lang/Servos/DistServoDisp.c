/**
 * Joel Brigida
 * CDA-3331C: Intro to Microprocessors
 * CDA-4630: Embedded Systems
 * This program uses a Servo & LCD with Arduino Framework
 * The Energia IDE can be used to run this code.
 * Notes:
 * LCD Signal pins: P2_0 = RS, P2_1 = E, P2_2 = D4, P2_3 = D5, P2_4 = D6, P2_5 = D7)
 * LCD Power pins: A & VDD connected to VCC
 * LCD Ground Lins: VO, RW, VSS & K connected to GND
 */

#include <msp430.h>
#include <LiquidCrystal.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdarg.h>
#include <Servo.h>

Servo myservo;                                      // Declare Servo Object
LiquidCrystal lcd(P2_0, P2_1, P2_2, P2_3, P2_4, P2_5);

int irDistance = 2;                                 // analog pin used to connect the potentiometer
int setDistance = 300;
int servo;                                          // variable to read the value from the analog pin
int distance;

void setup()
{
    lcd.begin(20, 2);                               // We are using a 16x2 LCD display
    delay(100);                                     // Wait 100ms
    lcd.clear();
    
    myservo.attach(19);                             // attaches the servo on pin 9 to the servo object

    for(int j = 1; j <= 10; j++)                    // Servo initial test pattern
    { 
        for(int i = 0; i <= 18 * j; i++)
        {
            myservo.write(i);
            delay(100  / (18 * j));
        } 
        delay(1000);
        
        for(int i = 0; i <= 18 * j; i++)
        {
            myservo.write(18 * j - i);
            delay(100 / (18 * j));
        }
        delay(1000);
    }
}

void loop()
{
    distance = 0;
    int i;
    for(i = 0; i <= 2; i++)                         // read IR distance and normalize it
    {
        distance += analogRead(irDistance);
        __delay_cycles(5000);
    } 
    
    distance = distance / 3;                        // read potentiometer value: 0 to 1023
    if(distance <= 300)                             // set servo limits: 300 - 900
    {
        distance = 300;
    }
    if(distance >= 900)
    {
        distance = 900;
    }
    __delay_cycles(1000);                    

    if ((distance > 300) && (abs(setDistance - distance) <= setDistance * 0.1))
    {
        setDistance = distance;                     // map the servo value from distance
        servo = map(distance, 300, 900, 0, 180);    // scale servo between 0 and 180
        
        if (servo <= 0)
        {
            servo = 0;
        }
        if (servo >= 180)                           // set servo limits 0 - 180
        {
            servo = 180;
        }      
        myservo.write(servo);                       // run servo & wait for it to get there
        __delay_cycles(10000);
    }

    lcd.setCursor(0, 0);                            // Print values to LCD
    lcd.print("Distance ");
    lcd.setCursor(9, 0);
    lcd.print("Servo  ");
    lcd.setCursor(0, 1);
    lcd.print("         ");
    lcd.setCursor(9, 1);
    lcd.print("       ");
    lcd.setCursor(0, 1);
    lcd.print(distance);
    lcd.setCursor(4, 1);
    lcd.print(setDistance);
    lcd.setCursor(9, 1);
    lcd.print(servo);

    __delay_cycles(1000000);                        // run servo and wait for it to get there
}