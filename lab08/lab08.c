
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
#include "timer.h"
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

void displayHome ( uint8_t song ) {
    FIL fp;

    // Get the file from the MicroSD card.
    findMP3( song, &fp );

    // Process ID3 header (if any).
    getID3Tags( &fp , &tags );

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
    uint8_t * volStr = globalVol; // default is 16
    uint8_t * songStr[30] = {
                "01", "02", "03", "04", "05", "06", "07", "08", "09", "10",
                "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
                "21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
    };

    positionLCD(4,0);
    stringLCD(playStr);
    positionLCD(4,11);
    stringLCD(shufStr);
    positionLCD(5,0);
    stringLCD("Volume:");
    stringLCD(volStr);

    positionLCD(5,11);
    stringLCD("Song: ");
    stringLCD(songStr[song]);
}
void displayMainMenu( uint8_t song) {

    uint8_t startSong = song - 2;
    uint8_t pos = 0;

    for (uint8_t i = startSong; i <= startSong + 5; i++){
        FIL fp;

        // Get the file from the MicroSD card.
        findMP3( song, &fp );

        // Process ID3 header (if any).
        getID3Tags( &fp , &tags );

        clearLCD();
        positionLCD(pos,0);
        stringLCD(tags.title);
        pos++;
    }

    clearLCD();
    positionLCD(0,0);
    stringLCD(tags.title);
    positionLCD(1,0);
    stringLCD(tags.artist);
    positionLCD(2,0);
    stringLCD(tags.album);
}

void playSong( uint8_t song  ) {
  FIL fp;

  // Get the file from the MicroSD card.
  findMP3( song, &fp );

  // Process ID3 header (if any).
  getID3Tags( &fp , &tags );



  
  if (isHomeMode() == true) {
      displayHome(song);
  }
  else if (isMenuMode() == true) {
      displayMainMenu(song);
  }
  
  //displayHome(song);

  // Prepare for sound output.
  initSound();

   // Decode and play the song.
  MP3decode( &fp );

   // Wait for the last data to be sent to the DACs.
  while( isSoundFinished() == false ) __asm( " wfi" );

  // Stop the DAC timer.
  enableTimer2A( false );

}

main() {
  // Initialize clock, SSIs, and Timer
  initOsc();
  initSSI3();
  initLCD( false );
  initSSI1();
  //initTimer(32); // for displaying time elapsed
  initTimer2A();
  initTimer5A();
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
