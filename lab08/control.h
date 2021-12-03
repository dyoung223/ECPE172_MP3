/*
 * control.h
 *
 *  Created on: Aug 13, 2018
 *      Author: khughes
 *
 */

#ifndef _CONTROL_H
#define _CONTROL_H

#include <stdint.h>
#include <stdbool.h>

extern bool initialPlay;
extern bool queueModeOn;
extern bool queueModeSelection;
//extern bool veryFirstPlay;

uint8_t getNumSongs( void );
void setNumSongs( uint8_t num );
uint8_t getSong( void );
uint8_t getCurSong( void );
void setSong( uint8_t v );
void setDone( void );
uint8_t playPreviousSong( void );
bool isDone( void );
void setPaused( bool v );
bool isPaused( void );
void setShuffle( bool v );
bool isShuffle( void );

bool isMenuMode( void );
void setMenuMode( bool m );
bool isQueueMode( void );
void setQueueMode( bool q );
bool isHomeMode( void );
void setHomeMode( bool h );
void displayVol ( void );

void enterMenuMode( uint8_t curSong );
void exitMenuMode( uint8_t curSong );

void enterQueueMode(void);
void initQueueBuffer(void);
uint8_t playQueueSongs(void);
uint8_t playPreviousSong( void );
bool isPrevious( void );
void setPrevious( bool v );

#endif // _CONTROL_H
