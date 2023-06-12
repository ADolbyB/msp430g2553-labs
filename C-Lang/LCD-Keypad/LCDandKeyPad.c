/**
 * Joel Brigida
 * CDA-3331C: Intro to Microprocessors
 * This is test code for a 2x16 LCD display (8-bit Parallel Interface) and a 12-button
 * keypad. Pressing a button on the keypad triggers an ISR.
 * All commands take ~40us to execute, except clear & cursor return which take ~1.64ms,
 * which is the reason for the delay implementation.
 * This IS NOT for use with a 4 wire I2C/SPI display.
*/

#include <msp430.h>
#include <msp430g2553.h>
#include <stdio.h>

#define RS BIT2                                          // LCD display pins: connect as indicated below
#define EN BIT3
#define D4 BIT4
#define D5 BIT5
#define D6 BIT6
#define D7 BIT7
#define CMD  0
#define DATA 1
#define LCD_OUT P2OUT
#define LCD_DIR P2DIR

#define ROW1 BIT0                                        // keypad pins, connect pins as indicated below
#define ROW2 BIT1
#define ROW3 BIT2
#define ROW4 BIT3
#define COL1 BIT4
#define COL2 BIT5
#define COL3 BIT6

#define KEYP_DIR P1DIR
#define OTROWS P1OUT
#define INCOLS P1IN
#define ALLROWS (ROW4 | ROW3 | ROW2 | ROW1)
#define ALLCOLS (COL3 | COL2 | COL1)

int CountVal = 0;
int keynum = 0;
int rownum;
int ROWS;
char str[4];

#define LEDG BIT0                                        // Green LED
#define LEDR BIT1                                        // Red LED

void delay(int t)                                        // Delay function for producing delay in 0.1s increments
{
   int i;
   for(i = t; i > 0; i--)
   {
      __delay_cycles(100);
   }
}

void intToChar(char str[], int num)                      // integer to char conversion
{
   int i;
   int rem;
   int len = 0;
   int n = num;
   
   if(n == 0)
   {
      len = 1;
   }
   else
   {
      while(n != 0)
      {
         len++;   
         n /= 10;
      }
   }
   
   for(i = 0; i < len; i++)
   {
      rem = num % 10;
      num = num / 10;
      str[len - (i + 1)] = rem + '0';
   }
   str[len] = '\0';
}

void pulseEN(void)                                       // Pulse ENABLE pin after data is written
{
   LCD_OUT |= EN;
   delay(1);
   
   LCD_OUT &= ~EN;
   delay(1);
}

void lcdWrite(int value, int mode)                       // Function to write data/command to LCD
{
   if(mode == CMD)
   {
      LCD_OUT &= ~RS;                                    // Set RS -> LOW for Command mode
   }
   else
   {
      LCD_OUT |= RS;                                     // Set RS -> HIGH for Data mode
   }
   
   LCD_OUT = ((LCD_OUT & 0x0F) | (value & 0xF0));        // Write high nibble first
   pulseEN();
   delay(1);
   
   LCD_OUT = ((LCD_OUT & 0x0F) | ((value << 4) & 0xF0)); // Write low nibble next
   pulseEN();
   delay(1);
}

void lcdPrint(char *s)                                   // Function to print a string on LCD
{
   while(*s)
   {
      lcdWrite(*s, DATA);
      s++;
   }
}

void lcdSetCursor(int row, int col)                      // Function to move cursor to desired position on LCD
{
   const int rowOffsets[]
   rowOffsets = { 0x00, 0x40 };
   
   lcdWrite(0x80 | (col + rowOffsets[row]), CMD);
   delay(1);
}

void lcdInit()                                           // Initialize LCD
{
   P2SEL &= ~(BIT6 + BIT7);
   LCD_DIR |= (D4 + D5 + D6 + D7 + RS + EN);
   LCD_OUT &= ~(D4 + D5 + D6 + D7 + RS + EN);

   delay(150);                                           // Wait for power up: 15ms
   lcdWrite(0x33, CMD);                                  // Initialization Sequence 1
   delay(50);       
   lcdWrite(0x32, CMD);                                  // Initialization Sequence 2
   delay(1);        

   lcdWrite(0x28, CMD);                                  // 4 bit mode, 2 line
   delay(1);
   lcdWrite(0x0C, CMD);                                  // Display ON, Cursor off, for cursor on F
   delay(1);
   lcdWrite(0x01, CMD);                                  // Clear screen
   delay(20);
}

void lcdLayout()                                         // LCD fixed layout text
{
   lcdSetCursor(0, 0);
   lcdPrint("Embedded Systems");
   lcdSetCursor(1, 1);
   lcdPrint("CNT:");
   lcdSetCursor(1, 9);
   lcdPrint("SPD:");
}

void lcdUpdate()                                         // LCD field updates
{
   intToChar (str, CountVal);
   
   lcdSetCursor(1, 5);
   lcdPrint("   ");
   lcdSetCursor(1, 5);
   lcdPrint(str);
   intToChar(str, keynum);
   lcdSetCursor(1, 13);
   lcdPrint("   ");
   lcdSetCursor(1, 13);
   lcdPrint(str);
}

void keypadInit()                                        // Initialize keypad
{
   KEYP_DIR = 0x00;                                      // Set port 1 all inputs
   KEYP_DIR |= ALLROWS;                                  // Set all rows as output
   
   P1REN |= ALLCOLS;                                     // Enable resistors for all columns
   P1OUT |= ALLCOLS;                                     // pullup resistors for all columns
   P1IES |= ALLCOLS;                                     // Interrupt edge select: HIGH to LOW
   P1IE |= ALLCOLS;                                      // Enable interrupt on all columns
   P1IFG &= ~ALLCOLS;                                    // Clear all interrupt flags on the columns
   OTROWS &= ~(ALLROWS);                                 // Assert low on all rows to detect any key press interrupt
   delay(2);
}

// MAIN() FUNCTION
void main(void)
{
   WDTCTL = WDTPW + WDTHOLD;                             // stop watchdog
   P2DIR |= (LEDG | LEDR);                               // two outputs for LEDs

   lcdInit();
   lcdLayout();

   keypadInit();
   __enable_interrupt();                                 // enable interrupts

   for(;;)
   {
      lcdUpdate();

      if(keynum == 10)                                   // Green LED on
      {
         P2OUT |= LEDG;
      }
      if(keynum == 12)                                   // Red LED on
      {
         P2OUT |= LEDR;
      }
      if(keynum == 11)                                   // Both LEDs off
      {
         P2OUT &= ~(LEDG | LEDR); 
      }
      delay(2);

      if ((keynum >= 1) && (keynum <= 9))                // Counter activated if 1 <= keynum <= 9
      {
         CountVal ++;
         if (CountVal >= 1000)
         {
            CountVal = 0;
         }
         delay(10000 / (keynum * keynum + 1));           // speed is proportional to keynum
      }
   }
}

// Port 1 ISR
#pragma vector = PORT1_VECTOR                            // Port 1 interrupt service routine
__interrupt void Port_1(void)
{
   OTROWS |= ALLROWS;                                    // Make all rows high
   delay(1);
   
   for(rownum = 0; rownum <= 3; rownum++)                // Start scanning the Keypad
   {
      OTROWS = ((ROW1 << rownum) ^ ALLROWS);             // start a 1 in first roww and shift left rownum times, and XOR
      delay(1);
      
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

   OTROWS &= ~ALLROWS;                                   // Make all rows low for next key stroke interrupt
   delay(1);
   P1IFG  &= ~ALLCOLS;                                   // Clear the interrupt flags
   delay(1); 
}