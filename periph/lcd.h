/*
 * lcd.h
 *
 *  Created on:  Aug 12, 2018
 *  Modified on: May 29, 2020
 *  Modified on: Aug 4, 2020
 *      Author: khughes
 */

#ifndef _LCD_H
#define _LCD_H

#include <stdint.h>

void initLCD( bool orientation );
void clearLCD ( void );
void positionLCD ( uint8_t y, uint8_t x );
void charLCD( const uint8_t *index );
void stringLCD( uint8_t *str );

#endif // _LCD_H
