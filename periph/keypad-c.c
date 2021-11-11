/*
 * keypad-c.c
 *
 *  Created on:  July 21, 2019
 *  Modified on: Aug 4, 2020
 *      Author: khughes
 *
 */

#include <stdint.h>
#include <stdbool.h>

// SYSCTL and GPIO definitions.
#include "ppb.h"
#include "sysctl.h"
#include "gpio.h"

// Prototypes for peripherals.
#include "osc.h"
#include "keypad.h"

const struct portinfo columndef = {
  {GPIO_PIN_3, GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_0},
  GPIO_PORTH
};

const struct portinfo rowdef = {
  {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7},
  GPIO_PORTC
};

// Local variables used by the procedures
const struct portinfo *cport;
const struct portinfo *rport;
uint8_t lastrow;

const uint8_t keymap[4][4] = {
#ifdef KEYPAD_ALPHA
   {'1','2','3','A'},
   {'4','5','6','B'},
   {'7','8','9','C'},
   {'0','F','E','D'},
#endif
#ifdef KEYPAD_PHONE
   {'1','2','3','A'},
   {'4','5','6','B'},
   {'7','8','9','C'},
   {'*','0','#','D'},
#endif
#ifdef KEYPAD_ABT
   {'0','1','2','3'},
   {'4','5','6','7'},
   {'8','9','A','B'},
   {'C','D','E','F'},
#endif
};

// Each peripheral base address is offset from the prior one by 0x1000.  Use 
// the base address to calculate which bit in the RCGCCPIO register needs 
// to be enabled.


static void initRCGCGPIO( volatile uint32_t * base ) {
  SYSCTL[SYSCTL_RCGCGPIO] |= 1 << (((uint32_t)base - (uint32_t)GPIO_PORTA) >> 12);
}


// Local procedure to change the row pins.
void setrow( uint8_t row) {
  // Get the four pins for a row
  uint8_t pins = rport->pins[0] | rport->pins[1] | 
        rport->pins[2] | rport->pins[3];

  // Update global row indicator
  lastrow = row & 0x03;

  // Set all pin high except active one
  rport->base[pins] = pins ^ rport->pins[lastrow]; //
}

// Initialize the specified row and column ports

void initKeypad( const struct portinfo *c, const struct portinfo *r ) {
  // temporary variable representing row or column port pins
  uint8_t pins;

  // Save base addresses for row and column ports for getKey()
  rport = r;
  cport = c;

  // enable port clocks
  initRCGCGPIO( cport->base );
  initRCGCGPIO( rport->base );

  // enable row pins as outputs
  pins = rport->pins[0] | rport->pins[1] | rport->pins[2] | rport->pins[3];
  // set direction as output
  rport->base[GPIO_DIR] |= pins;
  // turn off pull-ups
  rport->base[GPIO_PUR] &= ~pins;
  // ensure open-drain
  rport->base[GPIO_ODR] |= pins;
  // turn off alternate functions
  rport->base[GPIO_AFSEL] &= ~pins;
  // enable
  rport->base[GPIO_DEN] |= pins;

  // enable col pins as inputs
  pins = cport->pins[0] | cport->pins[1] | cport->pins[2] | cport->pins[3];
  // set direction as input
  cport->base[GPIO_DIR] &= ~pins;
  // enable pull-ups
  cport->base[GPIO_PUR] |= pins;
  // turn off open-drain
  cport->base[GPIO_ODR] &= ~pins;
  // turn off alternate functions
  cport->base[GPIO_AFSEL] &= ~pins;
  // enable
  cport->base[GPIO_DEN] |= pins;

  // set the initial row to scan
  setrow( 0 );
}


// Check for a key.  If a key is pressed, the procedure returns true and the
// row and column are set.  If a key is not pressed, the procedure returns
// false and row and columns are undefined.


bool getKey( uint8_t *col, uint8_t *row ) {
  // Get pin numbers for columns
  uint8_t pins = cport->pins[0] | cport->pins[1] | cport->pins[2] | cport->pins[3];

  // If all pins are high, change the row for the next time.
  if( cport->base[pins] == pins ) {
    setrow( lastrow + 1 );
    return false;
  }

  // Otherwise one column is low....

  // Return row number
  *row = lastrow;

  // Now figure out which column it is
  // Only check three columns; if none of them are low, it has to be the
  // fourth.
  for( uint8_t i = 0; i < 3; ++i ) {
    if( cport->base[cport->pins[i]] == 0 ) {
      *col = i;
      return true;
    }
  }
  *col = 3;
  return true;
}

