/*
 * ssi_DAC.h
 *
 *  Created on:  Aug 13, 2018
 *  Modified on: Jul 19, 2010
 *      Author: khughes
 *
 */

#ifndef _SSI_DAC_H
#define _SSI_DAC_H

#include <stdint.h>
#include <stdbool.h>

void initSSI1( void );
void txDataSSI1( uint8_t dac, uint16_t data );

/*
 * MCP49x2 pin-outs
 * Pin  Symbol  Function
 *  1   VDD     Supply Voltage Input (2.7V to 5.5V)
 *  2   NC
 *  3   CS      Chip Select Input
 *  4   SCK     Serial Clock Input
 *  5   SDI     Serial Data Input
 *  6   NC
 *  7   NC
 *  8   LDAC    Synchronization Input. This pin is used to transfer DAC 
                settings (Input Registers) to the output registers (V OUT )
 *  9   SHDN    Hardware Shutdown Input
 *  10  VoutB   DAC B Output
 *  11  VrefB   DAC B Reference Voltage Input (V SS to V DD )
 *  12  Vss     Ground reference point for all circuitry on the device
 *  13  VrefA   DAC A Reference Voltage Input (V SS to V DD )
 *  14  VoutA   DAC A Output
 *
 * Data Output for MPC49220 (SPI Mode 0)
 *  b15: A/B: 1 = DACB, 0 = DACA
 *  b14: BUF: 1 = buffered
 *  b13: GA:  1 = 1x (Vout = Vref * D/4096)
 *            0 = 2x (Vout = 2*Vref * D/4096)
 *  b12: SHDN:1 = Vout is active, 0 = disabled
 *  b11-b0: data
 */

#endif // _SSI_DAC_H
