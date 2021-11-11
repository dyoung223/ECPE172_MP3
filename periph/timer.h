/*
 * timer.h
 *
 *  Created on: Aug 3, 2018
 *      Author: khughes
 *
 */

#ifndef _TIMER_H
#define _TIMER_H

// Configure the timer to roll over at a 200Hz frequency.
void initTimer( uint8_t size );

// Wait for the timer to roll over, then return.
void waitOnTimer( void );

#endif // _TIMER_H
