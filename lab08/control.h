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
//extern bool veryFirstPlay;

uint8_t getNumSongs( void );
void setNumSongs( uint8_t num );
uint8_t getSong( void );
void setSong( uint8_t v );
void setDone( void );
bool isDone( void );
void setPaused( bool v );
bool isPaused( void );
void setShuffle( bool v );
bool isShuffle( void );
void enterQueueMode(void);
void initQueueBuffer(void);
uint8_t playQueueSongs(void);


#endif // _CONTROL_H
