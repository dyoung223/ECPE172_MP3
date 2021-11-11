/*
 * cipher.h
 *
 *  Created on: Aug 1, 2020
 *      Author: khughes
 */

#ifndef _CIPHER_H
#define _CIPHER_H

#include <stdint.h>

void disableDecipher();
void enableDecipher();
void decipher( uint8_t *ptr, uint16_t size );

#endif // _CIPHER_H
