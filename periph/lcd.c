/*
 * lcd.c
 *
 *  Created on:  Aug 12, 2018
 *  Modified on: Jul 15, 2019
 *  Modified on: May 29, 2020
 *  Modified on: Aug 4, 2020
 *
 *      Author: khughes
 * 
 * NHD-C12864WC-FSW-FBW-3V3 LCD driver.
 */

// 8080 (Intel) mode parallel interface.

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// SYSCTL, GPIO, and PPB definitions.
#include "sysctl.h"
#include "gpio.h"
#include "ppb.h"

// oscillator definitions.
#include "osc.h"

// Character definitions 
#include "font5x7.h"

// Dimensions of the LCD module
#define LCD_HEIGHT               64
#define LCD_WIDTH               128
#define LINES_PER_PAGE            8  // each page is 8 lines

// ST7565R commands and bitmasks
#define ST7565R_DISPLAY_OFF                     (0xae | 0b0)
#define ST7565R_DISPLAY_ON                      (0xaf | 0b1)
#define ST7565R_DISPLAY_START_LINE_SET          0x40
#define   ST7565R_DISPLAY_START_LINE_SET_M        0b111111
#define ST7565R_PAGE_ADDRESS_SET                0xb0
#define   ST7565R_PAGE_ADDRESS_SET_M              0b1111
#define ST7565R_COLUMN_ADDRESS_SET_HIGHBITS     0x10
#define ST7565R_COLUMN_ADDRESS_SET_LOWBITS      0x00
#define   ST7565R_COLUMN_ADDRESS_SET_M            0b1111
#define ST7565R_ADC_SELECT_NORMAL               (0xa0 | 0b0)
#define ST7565R_ADC_SELECT_REVERSE              (0xa0 | 0b1)
#define ST7565R_DISPLAY_ALL_POINTS_OFF          0xa4
#define ST7565R_DISPLAY_ALL_POINTS_ON           0xa5
#define ST7565R_LCD_BIAS_SET                    0xa2
#define ST7565R_READ_MODIFY_WRITE               0xe0
#define ST7565R_READ_MODIFY_WRITE_END           0xee
#define ST7565R_RESET                           0xe2
#define ST7565R_COMMON_OUT_MODE_SELECT_NORMAL   0xc0
#define ST7565R_COMMON_OUT_MODE_SELECT_REVERSE  0xc8
#define ST7565R_POWER_CONTROL_SET               0x28
#define   ST7565R_POWER_CONTROL_SET_M             0b111
#define ST7565R_INTERNAL_RESISTOR_RATIO_SET     0x20
#define ST7565R_ELECTRONIC_VOLUME_MODE_SET      0x81
#define ST7565R_SLEEP_MODE_SET_SLEEP            (0xac | 0b0)
#define ST7565R_SLEEP_MODE_SET_NORMAL           (0xac | 0b1)
#define ST7565R_BOOSTER_RATION_SELECT_SET       0xf8

// Local copy of actual LCD column
static uint8_t _col;
// The orientation of the display
static bool rightSideUp;

#define LCD_CHARS 21 // LCD can display 21 characters on a line

// Macros for controlling signal pins
#define assertPIN(val,pin) {if(val)GPIO_PORTN[pin] = 0; else GPIO_PORTN[pin] = pin;}
#define assertWR(val)      assertPIN(val,GPIO_PIN_5)
#define assertRD(val)      assertPIN(val,GPIO_PIN_4)
#define assertCS(val)      assertPIN(val,GPIO_PIN_3)
#define assertReset(val)   assertPIN(val,GPIO_PIN_2)
#define A0Low()            {GPIO_PORTQ[GPIO_PIN_4] = 0;}
#define A0High()           {GPIO_PORTQ[GPIO_PIN_4] = 0xff;}
#define writeDB(val)       {GPIO_PORTM[GPIO_ALLPINS] = val;}

// Write one byte to LCD.  
static void writeLCD( uint8_t c ) {
// INSERT YOUR CODE HERE.  YOU SHOULD ONLY USE THE MACROS ABOVE, INCLUDING
// FOR TIMING DELAYS.
  assertCS(true); // active low so true sets to 0
  assertWR(true);
  writeDB( c );
  assertWR(false);
  assertCS(false);

}

// Send command to LCD.
static void writeCommand( uint8_t c ) {
  GPIO_PORTM[GPIO_DIR] = 0xff;  // prepare data bus for output
  A0Low();                      // prepare for command write
  writeLCD( c );
}

// Send data to LCD.
static void writeData( uint8_t c ) {
  GPIO_PORTM[GPIO_DIR] = 0xff;  // prepare data bus for output
  A0High();                     // prepare for data write
  writeLCD( c );
}

// Set page address.  This is our equivalent of setting the y location of 
// the cursor.
static void setPage( uint8_t p ) {
  writeCommand( ST7565R_PAGE_ADDRESS_SET | 
        ( ST7565R_PAGE_ADDRESS_SET_M & p ) );
}

// Set column address.  This is our equivalent of setting the x location of 
// the cursor, except it is a sub-character resolution.
static void setColumn( uint8_t a ) {
  writeCommand( ST7565R_COLUMN_ADDRESS_SET_HIGHBITS | 
        ( ST7565R_COLUMN_ADDRESS_SET_M & ( a >> 4 ) ) );
  writeCommand( ST7565R_COLUMN_ADDRESS_SET_LOWBITS |
        ( ST7565R_COLUMN_ADDRESS_SET_M & a ) );
}

// Delay t milliseconds
static void delayMS( uint32_t t ) {
  // Save current SysTick settings
  uint32_t ctrl = PPB[PPB_STCTRL];
  uint32_t reload = PPB[PPB_STRELOAD];

  // Configure to reload after 1 ms
  PPB[PPB_STCTRL] |= PPB_STCTRL_CLK_SRC | PPB_STCTRL_ENABLE;
  PPB[PPB_STRELOAD] = MAINOSCFREQ/1000-1;

  // Busy wait
  while( t-- )
    while( ( PPB[PPB_STCTRL] & PPB_STCTRL_COUNT ) == 0 );

  // Restore previous settings
  PPB[PPB_STCTRL] = ctrl;
  PPB[PPB_STRELOAD] = reload;
}

// Write data for a single character to the LCD
static void _charLCD( const uint8_t *index ) {
  if( rightSideUp ) {
    writeData( 0 ); // write a blank column before each char
    for( register uint8_t i = 0; i < CHAR_WIDTH-1; ++i ) writeData( *index++ );
  } else {
    index += CHAR_WIDTH-1;
    for( register uint8_t i = 0; i < CHAR_WIDTH-1; ++i ) {
      register uint8_t chr = *(--index);
      // Flip the bit pattern for the character
      asm(" rbit r0, r0\n lsr r0, #24");
      writeData( chr );
    }
    writeData( 0 ); // write a blank column between each char
  }
  // Update the column
  ++_col;
}

// --------------------------------------------
// Public procedures follow.
// --------------------------------------------

// Clear the LCD.
void clearLCD( void ) {
  uint8_t page, column;
  for( page = 0; page < LCD_HEIGHT / LINES_PER_PAGE; page++ ) {
    setPage( page );
    setColumn( 0 );
    for( column = 0; column < LCD_WIDTH; column++ ) {
      writeData( 0 ); // turn off all pixels
    }
  }
}

// Set address in character units.  0,0 is upper left corner.
void positionLCD( uint8_t y, uint8_t x ) {
  if( rightSideUp ) {
    setPage( y );
    setColumn( x * CHAR_WIDTH );
  } else {
    setPage( 7-y );
  }
  _col = x;
}

// Write data for a single character to the LCD
void charLCD( const uint8_t *index ) {
  // If we're in upside-down mode, update the column before displaying.
  if( !rightSideUp ) setColumn( LCD_WIDTH -1  - ( _col + 1 ) * CHAR_WIDTH );
  _charLCD( index );
}

// Display a NULL-terminated C string.
void stringLCD( uint8_t *str ) {
  if( rightSideUp ) {
    while( *str != 0 && _col < (LCD_WIDTH/CHAR_WIDTH) ) {
      const register uint8_t *index = (const uint8_t *)font[(*str - FIRST_CHAR)];
      _charLCD( index );
      ++str;
    }
  } else {
    // In upside-down mode, we write the string in reverse for better
    // efficiency.  However, we have to do more calculations up-front to
    // make it work the same as rightside-up mode.
    // 
    // Find the length of the string.
    uint8_t strl = strlen((const char *)str);
    // If there is enough space to fit the entire string on the
    // display, position the cursor at the end of the string.
    // Otherwise, position the cursor to the first position and adjust
    // the string length to the number of visible characters.
    if( _col + strl <= (LCD_WIDTH/CHAR_WIDTH) ) { 
      setColumn( LCD_WIDTH -1  - (_col + strl) * CHAR_WIDTH );
    } else {
      setColumn( 1 );
      strl = (LCD_WIDTH/CHAR_WIDTH) - _col;
    }
    // Move the pointer to the last visible character.
    str += strl - 1;
    // Display each character
    while( strl-- > 0 ) {
      register uint8_t *index = (uint8_t *)font[( *(char *)str-- - FIRST_CHAR)];
      _charLCD( index );
    }
  }
}

// Initialize the GPIO ports and LCD itself.
void initLCD( bool orientation ) {
  // Determine the orientation for the display.
  rightSideUp = orientation;

  // Enable Port N, M and Q
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTQ;
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTQ;

  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTN;
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTN;

  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTM;
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTM;

  // Set all pins on Port M to inputs initially, then enable.
  GPIO_PORTM[GPIO_DIR] &= ~(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIO_PORTM[GPIO_AFSEL] &= ~(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIO_PORTM[GPIO_DEN] |= (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

  // Enable all control pins on Port N and make them outputs.
  GPIO_PORTN[GPIO_DIR] |= (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
  GPIO_PORTN[GPIO_PUR] &= ~(GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
  GPIO_PORTN[GPIO_AFSEL] &= ~(GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
  GPIO_PORTN[GPIO_DEN] |= (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

  // Enable control pin on Port Q and make it an output.
  GPIO_PORTQ[GPIO_DIR] |= GPIO_PIN_4;
  GPIO_PORTQ[GPIO_PUR] &= ~GPIO_PIN_4;
  GPIO_PORTQ[GPIO_AFSEL] &= ~GPIO_PIN_4;
  GPIO_PORTQ[GPIO_DEN] |= GPIO_PIN_4;

  // Data sheet recommends reset be asserted for 20ms
  assertReset( true );
  delayMS( 20 );
  assertReset( false );

  // Deactivate all control pins
  assertCS( false );
  assertRD( false );
  assertWR( false );

  // Recommended initialization sequence.  The only change is the
  // argument for the Electronic Volume Mode Set; the recommended value
  // gave too low a contrast.

  const uint8_t init[] = { ST7565R_RESET,
                        ST7565R_LCD_BIAS_SET, 
                        ST7565R_ADC_SELECT_NORMAL,
                        ST7565R_COMMON_OUT_MODE_SELECT_REVERSE, 
                        ST7565R_DISPLAY_ON, 
                        ST7565R_DISPLAY_ALL_POINTS_OFF, 
                        ST7565R_DISPLAY_START_LINE_SET | 0, 
                        ST7565R_INTERNAL_RESISTOR_RATIO_SET | 5,
                        ST7565R_ELECTRONIC_VOLUME_MODE_SET, 48,
                        ST7565R_POWER_CONTROL_SET | 7,
                        ST7565R_READ_MODIFY_WRITE_END
                        };   
  const uint8_t *ptr = init;

  // ST7565R_READ_MODIFY_WRITE_END is use to mark the end of arguments
  // since it should not be used in the initialization.
  while( *ptr != ST7565R_READ_MODIFY_WRITE_END )
    writeCommand( *ptr++ );
}
