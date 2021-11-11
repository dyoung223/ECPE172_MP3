/*
 */

#include <stdint.h>
#include <stdbool.h>
#include <strings.h>

static bool deciphering = false;

void disableDecipher() {
  deciphering = false;
}

void enableDecipher() {
  deciphering = true;
}

// Unscramble MP3 file contents
void decipher( uint8_t *ptr, uint16_t size ) {
  if( deciphering ) {
    register uint8_t *tmpptr;
    register uint16_t count;
    for( tmpptr = ptr, count = size; count; --count, ++tmpptr ) {
      *tmpptr ^= 0xa5;
    }
  }
}

