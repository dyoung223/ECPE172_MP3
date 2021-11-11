/*
 * microSD.c
 *
 *  Created on:  Aug 8, 2018
 *  Modified on: Oct 7, 2019
 *  Modified on: Aug 16, 2020
 *      Author: khughes
 */

#include <stdint.h>
#include <stdio.h>

// Prototypes for peripherals.  SysTick is used for a delay.
#include "ssi_uSD.h"
#include "ppb.h"
#include "osc.h"

// Prototypes for utilities.
#include "microSD.h"

// MicroSD CMD/ACMD values: all commands are in the range 0-63, with bit
// 6 set to 1
#define CMD0            (0x40 | 0)
#define CMD8            (0x40 | 8)
#define CMD16           (0x40 | 16)
#define CMD17           (0x40 | 17)
#define ACMD41          (0x40 | 41)
#define CMD55           (0x40 | 55)
#define CMD58           (0x40 | 58)

#define CMD8_ARG        0x000001AA

// the bit which defines the type of card
#define CCS             (1 << 30)

// The number of bits to "scale" block addresses.  Byte-addressing card 
// (< 4GB) need to scale the address by 512 (1<<9) bytes.  Block-addressing
// cards don't need this (1<<0).
uint8_t blockScaling = 0xff;

// Define size of a block
#define SD_BLOCKSIZE	512

// Send a command to the MicroSD card.  CRC only matters for CMD0 and
// possibly CMD58.  Returns the R1 code, and if applicable the rest of the
// response in the buffer provided.
uint8_t sendCommand( uint8_t cmd, uint32_t args, uint8_t *response ) {
  uint8_t respLen, resp, crc;

  // Make sure CS is high before we start
  assertCS( false );

  // Specs call for 8 clocks after each transfer.  It's easier to do it here.
  resp = rxByteSSI3(  );

  // based on command type, determine the CRC and response length.
  switch( cmd ) {
  case CMD58 :
    respLen = 4;
    crc = 0;
    break;
  case CMD8 :
    respLen = 4;
    crc = 0x87;
    break;
  case CMD0 :
  default :
    respLen = 0;
    crc = 0x95;
    break;
  }

  // Initiate the transfer
  assertCS( true );

  // Send command 
  txByteSSI3( cmd );

  // Send argument.  Because Cortex-4M is little endian and data is big
  // endian, transmit in reverse order.
  txByteSSI3( ((uint8_t *)&args)[3] );
  txByteSSI3( ((uint8_t *)&args)[2] );
  txByteSSI3( ((uint8_t *)&args)[1] );
  txByteSSI3( ((uint8_t *)&args)[0] );

  // Send CRC
  txByteSSI3( crc );

  // now wait for non-0xFF byte
  while( ( resp = rxByteSSI3() ) == 0xff );

  // If there are other bytes being returned, place them in the response.
  // Again, convert from big endian to little endian as we do this.
  while( respLen > 0 )
    response[--respLen] = rxByteSSI3();

  // Return response.
  return resp;
}

// Initialize the MicroSD card.
bool initMicroSD( uint16_t retries ) {
  uint8_t i, reply;
  uint32_t response;
  uint32_t oldtick, oldreload;

  // Save current SysTick settings
  oldtick = PPB[PPB_STCTRL];
  oldreload = PPB[PPB_STRELOAD];

  // Delay about 10ms (120MHz clock)
  PPB[PPB_STRELOAD] = MAINOSCFREQ/100-1;
  PPB[PPB_STCTRL] |= PPB_STCTRL_CLK_SRC | PPB_STCTRL_ENABLE;

  // Make sure the SSI clock speed is low (<400KHz)
  setSSI3Speed( false );

  // Force CS high
  assertCS( false );

  // Perform delay.
  while( ( PPB[PPB_STCTRL] & PPB_STCTRL_COUNT ) == 0);

  // Restore previous SysTick settings
  PPB[PPB_STRELOAD] = oldreload;
  PPB[PPB_STCTRL] = oldtick;

  // Send a bunch of clocks with DI and CS high
  for( i = 0; i < 32; ++i )
    txByteSSI3( 0xff );

  // Send CMD0 until we get a response without errors
  do {
    --retries;  // Count how many times we try this
    // it too many retries, fail
    if( retries == 0 ) return false;
    reply = sendCommand( CMD0, 0x0, (uint8_t *)&response );
  } while( reply != 0x01 );

  // send CMD8
  reply = sendCommand( CMD8, CMD8_ARG, (uint8_t *)&response );
  if( !( reply & ~0x01 ) && ( response & 0x3ff ) == CMD8_ARG ) {
    // send ACM41 (have to send CMD55 prior)
    do {
      sendCommand( CMD55, 0x0, (uint8_t *)&response );
      reply = sendCommand( ACMD41, CCS, (uint8_t *)&response );
    } while( reply == 0x01 );
    // if reply is not an error, continue
    if( reply == 0x00 ) {
      // send CMD58
      reply = sendCommand( CMD58, 0x0, (uint8_t *)&response );
      // If reply is good, proceed
      if( reply == 0x00 ) {
        // Set the block scaling factor based on the CCS bit
        blockScaling = ((response & CCS) != CCS) * 9;
        // Switch to high speed mode
        setSSI3Speed( true );
        return true;
      }
    }
  }
  return false;
}

// Read one block from the MicroSD card using CMD17.  
//    blockaddr is the sector/block address
//    buffer is address to store data.
bool readBlock( uint32_t blockaddr, uint8_t *buffer ) {

  // Adjust the address based on the type of card
  blockaddr <<= blockScaling;

  // declare variables needed
  uint8_t reply;
  uint8_t resp;
  uint16_t resLen = 512;
  uint32_t response;

  // send CMD17 to read single block and wait until 0xfe
  reply = sendCommand(CMD17, blockaddr, (uint8_t*)&response );
  if (reply != 0x00) { // make sure reply is not an error. if error, then exit out of function
      return false;
  }

  while ((resp = rxByteSSI3()) != 0xfe);
      for (uint16_t i = 0; i < resLen; i++) {// loop through all the data
          resp = rxByteSSI3();
          buffer[i] = resp; // store each byte at each byte address
      }
      resp = rxByteSSI3();
      resp = rxByteSSI3();
      // Return true for a successful read.
      return true;
}
