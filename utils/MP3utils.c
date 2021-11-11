/*
 * MP3 File utility procedures
 *
 *  Created on: Aug 9, 2018
 *  Modified on: Aug 1, 2020
 *      Author: khughes
 */

#include <stdint.h>
#include <stdbool.h>
#include <strings.h>

// FatFS
#include "ff.h"

// This procedure counts the number of MP3 files on the MicroSD card.
// It only looks in the root folder; it does not recurse into subfolders.
uint8_t countMP3( void ) {
  uint8_t numSongs = 0;
  FRESULT res;
  DIR dp;
  FILINFO fno;

  // Open the root directory
  res = f_opendir( &dp, "/" );
  while( true ) {
    // Read a directory item.
    res = f_readdir( &dp, &fno );

    // If error or end of directory, exit the loop.
    if ( res != FR_OK || fno.fname[0] == 0 ) break;

    // If entry is not a directory, check the extension for "MP3"
    if ( !( fno.fattrib & AM_DIR ) ) {
      // If extension matches, strncasecmp() returns 0.  If this is true, then
      // we increment the count; if not, the count remains unchanged.
      numSongs += strncasecmp( fno.fname + strlen( fno.fname )-3, "MP3", 3 ) == 0;
    }
  }
  f_closedir( &dp );

  return numSongs;
}

// This procedure opens a particular MP3 file on the MicroSD card.  It 
// sequentially searches the root folder and counts until the index 
// matches.  On success, it returns the opened file pointer fp and true.
bool findMP3( uint8_t index, FIL *fp ) {
  FRESULT res;
  DIR dp;
  FILINFO fno;
  bool found = false;

  // Open the root directory
  res = f_opendir( &dp, "/" );
  while( true ) {
    // Read a directory item.
    res = f_readdir( &dp, &fno );

    // If error or end of directory, exit the loop.
    if ( res != FR_OK || fno.fname[0] == 0 ) break;

    // If entry is not a directory, check the extension for "MP3"
    if ( !( fno.fattrib & AM_DIR ) ) {
      if( !strncasecmp( fno.fname + strlen( fno.fname )-3, "MP3", 3 ) ) {

        // If it is an MP3 file, check for the correct index and if so
        // open that file and return successfully.  Otherwise, skip.
        if( index == 0 ) {
          f_open( fp, fno.fname, FA_READ );
          found = true;
          break;
        } else {
          --index;
        }
      }
    }
  }
  f_closedir( &dp );

  return found;
}
