/*
 * ssi_uSD.h
 *
 *  Created on: Aug 8, 2018
 *  Modified on: Aug 1, 2020
 *      Author: khughes
 */

#ifndef _SSI_USD_H
#define _SSI_USD_H

#include <stdint.h>
#include <stdbool.h>

// Initializes the QSSI3 module on PORTQ for the MicroSD card.
void initSSI3( void );

// Sets the "baud rate" divisor to generate either a slow speed (~400KHz) 
// or high speed (~12.5MHz). If fast is false then low speed is configured; 
// this is used during card initialization.
void setSSI3Speed( bool fast );

// Transmit the byte data to the slave over QSSI3.
void txByteSSI3( uint8_t data );

// Receive a byte from the slave over QSSI3.
uint8_t rxByteSSI3( void );

// Set the chip select output level on PQ1. If on is true the pin is set 
// low. The pin must not be changed when QSSI3 is busy.
void assertCS( bool on );

#endif // _SSI_USD_H
