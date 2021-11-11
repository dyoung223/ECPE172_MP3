/*
 * timer2A.h
 *
 *  Created on: Aug 13, 2018
 *      Author: khughes
 *
 */

#ifndef _TIMER2A_H
#define _TIMER2A_H

#include <stdint.h>
#include <stdbool.h>

void initTimer2A( void );
void enableTimer2A( bool enabled );
void setTimer2ARate( uint16_t rate );
void clearTimer2A( void );

#endif // _TIMER2A_H
