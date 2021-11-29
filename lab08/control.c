/*
 * control.c
 *
 *  Created on:  Aug 13, 2018
 *  Modified on: May 29, 2020
 *      Author: khughes
 */

#include <stdint.h>
#include <stdbool.h>

#include "control.h"

// The total number of songs on the MicroSD card.
static uint8_t numSongs;
// The next song to play.
static uint8_t song = 0;
// Indicates termination of current song by user.
static bool done = false;
// Indicates whether decoding/playing is playing or paused.
static bool paused = false;
// Indicates if the MP3 player is in song shuffle mode.
static bool shuffle = false;
//Indicates if the MP3 player is in Menu mode. Menu mode displays library and queue. Can select songs from here.
static bool menu = false;
//Indicates if the MP3 player is in Queue mode. Queue mode allows user to add songs to the queue. If not songs in queue, go in numerical order.
static bool queue = false;
//Indicates if the MP3 player is in Home mode. Home mode displays
static bool home = true;

// Private procedure for selecting the next song in shuffle mode.
static uint8_t getShuffle( uint8_t song ) {
  return song;
}

// Return the number of the song to play.  Initially, just
// return the next sequential song, and wrap back to 0 when all
// songs have been returned.
uint8_t getSong( void ) {
  static bool initial = false;

  // On the first call, always return the first song.
  if( initial == false ) {
    initial = true;
    return song = 0;
  }

  // Otherwise pick the next song to play.
  if( shuffle == false ) {
    song = ( song + 1 ) % numSongs;
  } else {
    song = getShuffle( song );
  }

  // Return song number.
  return song;
}

// Store the total number of songs (needed by getSong()).
void setNumSongs( uint8_t num ) {
  numSongs = num;
}

// Store the total number of songs (needed by getSong()).
uint8_t getNumSongs( void ) {
  return numSongs;
}

// Set next song explicitly.  Since getSongs() will advance the song by 1 in
// sequential mode after the first call, "decrement" the value before 
// storing.
void setSong( uint8_t v ) {
  song = v - 1;
  if( song >= numSongs ) {
    song = numSongs - 1;
  }
}

// Indicates whether the current MP3 song should be terminated early.
bool isDone( void ) {
  bool tmp = done;
  done = false;
  return tmp;
}

// Set song to terminate.
void setDone( void ) {
  done = true;
}

uint8_t playPreviousSong( void ){
    //End Current Song
    setDone();


    if (song == 0 ){
        song = numSongs - 2;
    }else{
        song = ( song - 2 ) % numSongs;
    }


    // Return song number.
    return song;

}

// Indicates whether the MP3 player is paused.
bool isPaused( void ) {
  return paused;
}

// Set state of pause.
void setPaused( bool v ) {
  paused = v;
}

// Indicates state of shuffle mode.
bool isShuffle( void ) {
  return shuffle;
}

// Set state of shuffle mode.
void setShuffle( bool v ) {
  shuffle = v;
}

// public function to see if MP3 player is in menu mode
bool isMenuMode( void ) {
    return menu;
}
// public function to toggle menu mode
void setMenuMode( bool m ) {
    menu = m;
}
//
bool isQueueMode( void ) {
    return queue;
}
//
void setQueueMode( bool q ){
    queue = q;
}
// public function to see if MP3 player is in home display mode (song and additional info)
bool isHomeMode( void ) {
    return home;
}
// public function to toggle home mode
void setHomeMode( bool h ) {
    home = h;
}

