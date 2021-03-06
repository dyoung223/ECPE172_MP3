/*
 * UI.c
 *
 *  Created on:  Nov 1, 2018
 *  Modified on: Aug 9, 2019
 *  Modified on: Oct 27, 2020
 *      Author: khughes
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Peripherals
#include "osc.h"
#include "gptm.h"
#include "gpio.h"
#include "ppb.h"
#include "sysctl.h"
#include "keypad.h"
#include "lookup.h"
#include "lcd.h"
#include "timer5A.h"

// Various hooks for MP3 player
#include "control.h"
#include "UI.h"
#include "sound.h"

// Definitions for function keys
enum keycmds_t {
  PLAY_PAUSE    = 'A',
  SHUFFLE       = 'B',
  VOLUME_UP     = 'C',
  VOLUME_DOWN   = 'D',
  SKIP_BACKWARD = '*', //#
  SKIP_FORWARD  = '#', //0
  MENU          = '1',
  QUEUE         = '3',
  PLAYLIST      = '2',

};

// Your keypad key assignments from Lab 4.
static const uint8_t keymap[4][4] = {

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


uint8_t * vols[33] = {
    " 00 ", " 01 ", " 02 ", " 03 ", " 04 ", " 05 ", " 06 ", " 07 ", " 08 ",
    " 09 ", " 10 ", " 11 ", " 12 ", " 13 ", " 14 ", " 15 ", " 16 ",
    " 17 ", " 18 ", " 19 ", " 20 ", " 21 ", " 22 ", " 23 ", " 24 ",
    " 25 ", " 26 ", " 27 ", " 28 ", " 29 ", " 30 ", " 31 ", " 32 ",
};

uint8_t entrySong;
uint16_t count = 0;

// Your keypad pin assignments from Lab 4.
const struct portinfo rowdef = {
    {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7},
    GPIO_PORTC
};

const struct portinfo columndef = {
   {GPIO_PIN_3, GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_0},
   GPIO_PORTH
};

// Return key value upon initial key press.  Since keys are
// ASCII, they cannot have a value greater than 0xff, so when
// no new key is pressed the procedure returns UINT16_MAX.
static uint16_t UIKey( void ) { 
  static enum {NOT_PRESSED, PRESSED} state = NOT_PRESSED;
  uint8_t column, row, key;

#define ADDR keymap
#define SIZE 4

  switch( state ) {
  case NOT_PRESSED:
    if( getKey( &column, &row ) == true ) {
      key = lookup(row, column, SIZE, (uint8_t *)ADDR);
      state = PRESSED;
      return (uint16_t)key;
    }
    break;
  case PRESSED:
    if( getKey( &column, &row ) == false )
      state = NOT_PRESSED;
    break;
  }
  return UINT16_MAX;
}

// Interrupt handler for user interface.  When called it scans the
// keypad and if a new key is detected, performs necessary actions.
void UIHandler( void ) {
  uint16_t key = UIKey( ); 
  uint8_t * playStr;
  uint8_t * shufStr;
  char vol[4];
  uint8_t iter = getCurSong();
  //setSong(getSong()-1); // because everytime getSong() is called, song is iterated up

  if (isPaused() == false) { // count while song is playing

      char second[4];
      char minute[4];

      if ((GPTM_TIMER3[GPTM_RIS] & GPTM_RIS_TATORIS) == 1){
          GPTM_TIMER3[GPTM_ICR] = GPTM_ICR_TATOCINT;
          count++;

          if (count%10 == 0) {
              uint8_t seconds = (count / 10) % 60;
              uint8_t minutes = count / 600;
              positionLCD(7,0);

              sprintf(second, "%d", seconds);
              sprintf(minute, "%d", minutes);

              stringLCD("Time: ");
              stringLCD("       ");
              positionLCD(7,6);
              stringLCD(minute);
              stringLCD("m ");
              stringLCD(second);
              stringLCD("s");
          }

      }

      if (isDone() == true) {
          count = 0;
      }
  }


  if( key != UINT16_MAX ) {
    switch( (enum keycmds_t)key ) {
    case PLAY_PAUSE:    // 'A'
      // setPaused( isPaused() == false ); moved to inside if. do not want to be able to play during menu screen
      // display pause state of MP3
      if (isMenuMode() == false){
          setPaused( isPaused() == false );
          positionLCD(4,0);
          if (isPaused()) {
              playStr = "PAUSED  ||";
          }
          else {
              playStr = "PLAYING |>";
          }
          stringLCD(playStr);
      }
      break;
    case SHUFFLE:       // 'B'
      setShuffle( isShuffle() == false );
      positionLCD(4,11);
      stringLCD("Shuff:");
      if (isShuffle()){
          shufStr = " ON";
      }
      else {
          shufStr = "OFF";
      }
      stringLCD(shufStr);
      break;
    case VOLUME_UP:     // 'C'
      upVolume(); // change display for vol here
      positionLCD(5,8);
      displayVol();
      break;
    case VOLUME_DOWN:   // 'D'
      downVolume();
      positionLCD(5,8);
      displayVol();
      break;

    case SKIP_BACKWARD: // '*' maybe #
  /*
        if(secondMenuFlag == false){
            secondMenuFlag = true;
        }else{
            secondMenuFlag = false;
            playPreviousSong();
        }

*/
      if (isMenuMode() == true){
          if (getCurSong() == 0) { // protection against negative integers when going back on first song
              enterMenuMode(getNumSongs() - 1);
              setSong(getNumSongs()); // setSong already decrements
          }
          else {
              enterMenuMode(getCurSong() - 1);
              setSong(getCurSong());
              //setSong(getSong() - 2);
          }
      }
      else {
          playPreviousSong();
      }
      break;

    case SKIP_FORWARD:  // '#' maybe 0
      if (isMenuMode() == true) {
          enterMenuMode(getSong());
          setSong(getSong());
      }
      else {
          setDone();
      }
      break;



    //case SKIP_BACKWARD: // '*'
/*        if(secondMenuFlag == false){
            secondMenuFlag = true;
        }else{
            secondMenuFlag = false;

            playPreviousSong();
        }*/
        //setPrevious( isPrevious() == false );
        //setDone(); //******added per Dr. Basha to use getSong()
      /* not how to do this
        while(true){
          key = UIKey( );
          if(key == SKIP_BACKWARD){
              playPreviousSong()
              break;
          }else if(key == SKIP_FORWARD){
              setDone();
              break;
          }
      }
      */
      //playPreviousSong();
      break;


   // case SKIP_FORWARD:  // '#'
        //setDone(); //******added per Dr. Basha to use getSong()

       /* if(secondMenuFlag == true){
            secondMenuFlag = false;
            queueModeOn = true;
            queueModeSelection = true;

        }else{
            //setDone();
            //enterQueueMode();
        }
        */

      //setDone();

     // break;
    case QUEUE:
        queueModeOn = true;
        queueModeSelection = true;
        setDone();
        break;
    case MENU:
        if (isMenuMode() == false) {
            setMenuMode(true);
            setPaused(true);
            entrySong = getCurSong();
            enterMenuMode(getCurSong());
        }
        else {
            setMenuMode(false);
            exitMenuMode(getCurSong());
            if ((getCurSong()) != entrySong){
                setPaused(false);
                if (getCurSong() == 0) { // protection against negative integers when going back on first song
                    setSong(getNumSongs()); // setSong already decrements
                }
                else if (getCurSong() == getNumSongs() - 1) {
                    setSong(getCurSong());
                }
                else {
                    setSong(getCurSong()-1);
                }
                setDone();
            }
            else {
                setPaused(false);
            }
        }
        break;
    case PLAYLIST:
        /*
        if (isMenuMode() == true && queueModeSelection == false && playlistModeOn == false) { // in menu
            setMenuMode(false); // set flag low so that go back to home display
        }
        else if (isMenuMode() == true && queueModeSelection == true && playlistModeOn == false) { // in
            queueModeSelection = false; // set flag low to go back to regular menu
        }
        else if (isMenuMode() == true && queueModeSelection == false && playlistModeOn == true) {
            playlistModeOn = false; // set flag low to go back to regular menu
        }
        */
        break;
    default:            // Numeric keys
      break;
    }
  }

  // Clear the time-out.
  clearTimer5A();
  //GPTM_TIMER5[GPTM_ICR] |= GPTM_ICR_TATOCINT;

}

void initUI( void ) {
  // Prep the keypad.
  initKeypad ( &columndef, &rowdef );

  // Enable Run Clock Gate Control
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER5;
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER5;

  // 32-bit periodic timer.
  GPTM_TIMER5[GPTM_CFG] &= ~GPTM_CFG_M;
  GPTM_TIMER5[GPTM_CFG] |= GPTM_CFG_32BITTIMER;
  GPTM_TIMER5[GPTM_TAMR] &= ~GPTM_TAMR_TAMR_M;
  GPTM_TIMER5[GPTM_TAMR] |= GPTM_TAMR_TAMR_PERIODIC;

  // Set reload value for 20Hz
  GPTM_TIMER5[GPTM_TAILR] = (MAINOSCFREQ/20)-1;

  // Enable the interrupt.
  GPTM_TIMER5[GPTM_IMR] |= GPTM_IMR_TATOIM;

  // Enable interrupt for timer.
  PPB[PPB_EN2] |= PPB_EN2_TIMER5A;

  // Set priority level to 1 (lower priority than Timer2A).
  PPB[PPB_PRI16] = ( PPB[PPB_PRI16] & ~PPB_PRI_INTB_M ) | ( 1 << PPB_PRI_INTB_S );

  // Clear the time-out.
  GPTM_TIMER5[GPTM_ICR] |= GPTM_ICR_TATOCINT;

  // Enable the timer.
  GPTM_TIMER5[GPTM_CTL] |= GPTM_CTL_TAEN | GPTM_CTL_TASTALL;
}
