/**
 * Joel Brigida
 * CDA-3331C: Intro to Microprocessors
 * This is test code for a 2x16 LCD display (8-bit Parallel Interface)
 * All commands take ~40us to execute, except clear & cursor return which take ~1.64 ms,
 * which is the reason for the delay implementation.
 * This IS NOT for use with a 4 wire I2C/SPI display.
*/

#include <msp430.h>
#include <inttypes.h>

#define D4 BIT4
#define D5 BIT5
#define D6 BIT6
#define D7 BIT7
#define RS BIT2
#define EN BIT3
#define CMD 0
#define DATA 1
#define LCD_OUT P2OUT
#define LCD_DIR P2DIR

void delay(uint8_t t)                                       // Delay function for producing delay in 0.1s increments
{
    uint8_t i;
    for(i = t; i > 0; i--)
    {
        __delay_cycles(100);                                // 100ms delay
    }
}

void pulseEN(void)                                          // Function to pulse ENABLE pin after data is written
{
    LCD_OUT |= EN;
    delay(1);
    LCD_OUT &= ~EN;
    delay(1);
}

void lcdWrite(uint8_t value, uint8_t mode)                  // Function to write data/command to LCD
{
    if(mode == CMD)
    {
        LCD_OUT &= ~RS;                                     // Set RS -> LOW for Command mode
    }            
    else
    {
        LCD_OUT |= RS;                                      // Set RS -> HIGH for Data mode
    }            

    LCD_OUT = ((LCD_OUT & 0x0F) | (value & 0xF0));          // Write high nibble first
    pulseEN();
    delay(1);
    
    LCD_OUT = ((LCD_OUT & 0x0F) | ((value << 4) & 0xF0));   // Write low nibble next
    pulseEN();
    delay(1);
}

void lcdPrint(char *s)                                      // Function to print a string to LCD
{
    while(*s)
    {
        lcdWrite(*s, DATA); 
        s++;
    }
}

void lcdSetCursor(uint8_t row, uint8_t col)                 // Moves cursor to desired position on LCD
{
    const uint8_t row_offsets[];
    row_offsets = { 0x00, 0x40 };
    
    lcdWrite(0x80 | (col + row_offsets[row]), CMD);
    delay(1);
}

void lcdInit()                                              // Initialize LCD
{
    P2SEL &= ~(BIT6 + BIT7);
    
    LCD_DIR |= (D4 + D5 + D6 + D7 + RS + EN);
    LCD_OUT &= ~(D4 + D5 + D6 + D7 + RS + EN);

    delay(150);                                             // Wait for power up (150ms)
    lcdWrite(0x33, CMD);                                    // Initialization Sequence 1
    delay(50);
    lcdWrite(0x32, CMD);                                    // Initialization Sequence 2
    delay(1);

    lcdWrite(0x28, CMD);                                    // 4 bit mode, 2 line
    delay(1);
    lcdWrite(0x0F, CMD);                                    // Display ON, Cursor ON, Blink ON
    delay(1);           
    lcdWrite(0x01, CMD);                                    // Clear screen
    delay(20);
    lcdWrite(0x06, CMD);                                    // Auto Increment Cursor
    delay(1);
    lcdSetCursor(0,0);                                      // Goto Row 1 Column 1
}

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                               // Stop Watch Dog Timer

    lcdInit();
    lcdSetCursor(0,0);
    lcdPrint("Embedded Systems");
    lcdSetCursor(1,2);
    lcdPrint("Analog: ");
    lcdSetCursor(1,10);
    
    for(;;) {}                                              // Inf Loop: Add code here...
}