/*
 * control.c
 *
 *  Created on:  Aug 13, 2018
 *  Modified on: May 29, 2020
 *      Author: khughes
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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




//static uint16_t *buffer;
//static uint16_t *InPtr;
//static uint16_t *OutPtr;
static volatile uint16_t bufferLength;
//static uint16_t queuebufsize;
enum {queuePADDING = 30};
static uint8_t queueArr[30];
uint8_t numSongsinQueue = 0;
uint8_t songPlaying;

enum new_cmds {
  //PLAY_PAUSE    = 'A',
  //SHUFFLE       = 'B',
  //VOLUME_UP     = 'C',
  //VOLUME_DOWN   = 'D',
  //SKIP_BACKWARD = '*', //#
  End_Mode      = '#', //0
};

static const uint8_t new_keymap[4][4] = {

#ifdef KEYPAD_ALPHA
                              {'D', 'C', 'B', 'A'},
                              {'E', '9', '8', '7'},
                              {'F', '8', '5', '4'},
                              {'0', '7', '2', '1'},
#endif
#ifdef KEYPAD_PHONE
                              {'1', '2', '3', 'A'},
                              {'4', '5', '6', 'B'},
                              {'7', '8', '9', 'C'},
                              {'*', '0', '#', 'D'},

#endif
#ifdef KEYPAD_ABT
                              {'0', '1', '2', '3'},
                              {'4', '5', '6', '7'},
                              {'8', '9', 'E', 'D'},
                              {'C', 'B', 'F', 'A'},
#endif

};

// Private procedure for selecting the next song in shuffle mode.
static uint8_t getShuffle( uint8_t song ) {
<<<<<<< HEAD
  song = rand() % getNumSongs(); //Destiny Shuffle mode
  return song;
=======
  //return rand() % numSong;
    return song;
>>>>>>> 78efc86ff05763185a66e9accd71459864a26276
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
    //song = getShuffle( song );
      song = rand() % numSongs;
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

}/*
void initQueueBuffer(void){

    static uint16_t *buffer;
    static uint16_t *InPtr;
    static uint16_t *OutPtr;

    bufferLength = 0;
    free( buffer );
    buffer = NULL;

}*/
void enterQueueMode(void) {
    static enum {NOT_PRESSED, PRESSED} state = NOT_PRESSED;
      uint8_t column, row, key;
      bool firstDigitFlag = true;
      uint8_t songNum = 0;
      songPlaying = 0;
    #define ADDR new_keymap
    #define SIZE 4
      key = 'A';
      while(key != End_Mode && numSongsinQueue < 30){
      switch( state ) {
      case NOT_PRESSED:
        if( getKey( &column, &row ) == true ) {
          key = lookup(row, column, SIZE, (uint8_t *)ADDR);
          state = PRESSED;
          if(firstDigitFlag == true){
              songNum = 0;
              firstDigitFlag = false;
              if(key == '1'){
                  songNum = songNum + 10;
              }else if(key == '2'){
                  songNum = songNum + 20;
              }else if(key == '3'){
                  songNum = songNum + 30;
              }else{
                  songNum = songNum;
              }
          }else{
              firstDigitFlag = true;
              if(key == '1'){
                    songNum = songNum + 1;
                }else if(key == '2'){
                    songNum = songNum + 2;
                }else if(key == '3'){
                    songNum = songNum + 3;
                }else if(key == '4'){
                    songNum = songNum + 4;
                }else if(key == '5'){
                    songNum = songNum + 5;
                }else if(key == '6'){
                    songNum = songNum + 6;
                }else if(key == '7'){
                    songNum = songNum + 7;
                }else if(key == '8'){
                    songNum = songNum + 8;
                }else if(key == '9'){
                    songNum = songNum + 9;
                }
                else{
                    songNum = songNum;
                }
             if(songNum > 30){ //protect against song values that don't exist
                 songNum = songNum % 30;
             }
              //push song into circular buffer
             queueArr[numSongsinQueue] = songNum - 1;
             numSongsinQueue++;

          }
          //return (uint16_t)key;
        }
        //break;
      case PRESSED:
        if( getKey( &column, &row ) == false )
          state = NOT_PRESSED;
        //break;
         }

      }
}

uint8_t playQueueSongs(void){

    if(numSongsinQueue > 0){
        //play queueArr[songPlaying]
        song = queueArr[songPlaying];
        songPlaying++;
        numSongsinQueue = numSongsinQueue - 1;
        return song;

    }else{
        //return -1 to indicate that the queue has finished playing.

        return 255;
    }
    /*
    while(bufferLength > 0){
        //send next song at *OutPtr++ to be played

        bufferLength -= 1;
        if((buffer + queuebufsize + queuePADDING) == OutPtr){
            OutPtr = buffer;
        }

    }

    bufferLength = 0;
    */
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

