/*
 * microSD.h
 *
 *  Created on:  Aug 8, 2018
 *  Modified on: Oct 7, 2019
 *  Modified on: Aug 16, 2020
 *      Author: khughes
 */

#ifndef _MICROSD_H
#define _MICROSD_H

#include <stdint.h>
#include <stdbool.h>

uint8_t sendCommand( uint8_t cmd, uint32_t args, uint8_t *response );
bool initMicroSD( uint16_t retries );
bool readBlock( uint32_t blockaddr, uint8_t *buffer );

#endif  // _MICROSD_H
