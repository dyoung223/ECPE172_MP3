/*
 * keypad.h
 *
 *  Created on:  July 21, 2019
 *  Modified on: Oct 2, 2019 
 *  Modified on: Nov 18, 2020 
 *      Author: khughes
 */

#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include <stdint.h>
#include <stdbool.h>

struct portinfo {
   uint8_t pins[4];
   volatile uint32_t *base;
};

// Example definition of pin assignments for keypad rows.  The base address
// is GPIO_PORTK, and the pin assignments are row 0 is GPIO_PIN_7, row 1 is
// GPIO_PIN_6, row 2 is GPIO_PIN_2, and row 3 is GPIO_PIN_0.  The row number
// is the index into the pins[4] array.

extern const struct portinfo columndef;

extern const struct portinfo rowdef;

// Define the three keypad lookups here.  To declare yours, go into the
// properties for the lab04 project and under Build->ARM Compiler->
// Predefined Symbols, add the macro name in "Pre-define NAME".

extern const uint8_t keymap[4][4];
void setrow( uint8_t row );
void initKeypad( const struct portinfo *col, const struct portinfo *row );
bool getKey( uint8_t *col, uint8_t *row );

#endif //  __KEYPAD_H__
