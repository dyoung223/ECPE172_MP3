#ifndef __LOOKUP_H__
#define __LOOKUP_H__

#include <stdint.h>

// Put the function prototype for your lookup() procedure here.

uint8_t lookup(uint8_t row, uint8_t column, uint8_t size, uint8_t *table);
//uint8_t lookup(uint8_t row, uint8_t column, SIZE, ADDR);

// In your definition of LOOKUP, use (not define) the macros ADDR and SIZE
// for the names of the look-up table and size variables used in your C code.
// The actual definitions of ADDR and SIZE go in your C code, not here.

#define LOOKUP lookup(row, column, size, (uint8_t *)ADDR)

/*
void hi(char name1, char name2);

#define HEY hi(jason, kevin)

HEY;
hi(jason, kevin);
*/
#endif // __LOOKUP_H__
