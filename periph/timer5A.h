/*
 * timer5A.h
 *

 */

#ifndef _TIMER5A_H
#define _TIMER5A_H

#include <stdint.h>
#include <stdbool.h>

void initTimer5A( void );
void enableTimer5A( bool enabled );
void setTimer5ARate( uint16_t rate );
void clearTimer5A( void );

#endif // _TIMER5A_H
