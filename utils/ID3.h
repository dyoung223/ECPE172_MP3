/*
 * ID3.h
 *
 *  Created on: Aug 9, 2018
 *      Author: khughes
 */

#ifndef _ID3_H
#define _ID3_H

#include <stdint.h>
#include <stdbool.h>

// FatFs data type definitions.
#include "ff.h"

// ID3v2 header is 10 bytes
#define ID3v2_HEADER_SIZE 10

#define MAXTAGSIZE 128

struct id3tags {
  uint8_t title[MAXTAGSIZE];
  uint8_t artist[MAXTAGSIZE];
  uint8_t album[MAXTAGSIZE];
};

uint32_t getID3Tags( FIL *fp, struct id3tags * fields );

#endif // _ID3_H
