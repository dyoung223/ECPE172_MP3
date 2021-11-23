
///// TEST //////

/*
 * lab08.c
 *
 *  Created on:  Aug 13, 2018
 *  Modified on: Jul 19, 2019
 *  Modified on: May 29, 2020
 *  Author: khughes
 */


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// Peripherals
#include "osc.h"
#include "ssi_uSD.h"
#include "lcd.h"
#include "ssi1_DAC.h"
#include "timer2A.h"
#include "timer5A.h"
#include "UI.h"

// FatFs
#include "ff.h"

// Utilities
#include "MP3utils.h"
#include "ID3.h"

// Other project files
#include "mp3.h"
#include "sound.h"
#include "control.h"

// ID3 tags
struct id3tags tags;



// Decode and play an MP3 file.
void playSong( uint8_t song  ) {
  FIL fp;

  // Get the file from the MicroSD card.
  findMP3( song, &fp );

  // Process ID3 header (if any).
  getID3Tags( &fp , &tags );


  // display song contents
  clearLCD();
  positionLCD(0,0);
  stringLCD(tags.title);
  positionLCD(1,0);
  stringLCD(tags.artist);
  positionLCD(2,0);
  stringLCD(tags.album);

  // add displayLCD to show additional status information (basic function #6)
  // display additional contents
  uint8_t * playStr = "Playing |>"; // default is playing
  uint8_t * shufStr = "Shuff:OFF"; // default is OFF
  uint8_t * volStr = "16"; // default is 16
  uint8_t * songStr = "01"; // default is 1

  positionLCD(4,0);
  stringLCD(playStr);
  positionLCD(4,11);
  stringLCD(shufStr);
  positionLCD(5,0);
  stringLCD("Volume:");
  stringLCD(volStr);

  positionLCD(5,11);
  stringLCD("Song:");
  stringLCD(songStr);

  // Prepare for sound output.
  initSound();

   // Decode and play the song.
  MP3decode( &fp );

   // Wait for the last data to be sent to the DACs.
  while( isSoundFinished() == false ) __asm( " wfi" );

  // Stop the DAC timer.
  enableTimer2A( false );
}



void displayElapsedTime( void ){
    
}

main() {
  // Initialize clock, SSIs, and Timer
  initOsc();
  initSSI3();
  initLCD( false );
  initSSI1();
  initTimer2A();
  //initTimer5A();
  initUI();

  // Initialize structure.
  FATFS fs; f_mount( &fs, "", 0 );

  // Find out how many MP3 files are on the card.
  uint8_t numSongs = countMP3();
  numSongs = countMP3();
  setNumSongs( numSongs );

  // Obligatory endless loop.
  while( true ) {
    // Get the next file from the MicroSD card.
    uint8_t song = getSong();

    // Send the file to the MP3 decoder
    playSong( song );
  }
}
