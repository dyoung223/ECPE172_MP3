/*
 * timer2A.c
 *
 *  Created on:  Aug 13, 2018
 *  MOdified on: Jul 19, 2019
 *      Author: khughes
 */

#include <stdint.h>
#include <stdbool.h>

// SYSCTL, GPIO, PPB, and Timer2a definitions.
#include "osc.h"
#include "sysctl.h"
#include "gptm.h"
#include "ppb.h"
#include "timer2A.h"

// Configure Timer2A as a 32-bit periodic timer which will generate an 
// interrupt on time-out. Note: this procedure DOES NOT enable the timer 
// or set the reload value; that is done by the procedures enableTimer2A() 
// and setTimer2ARate().
void initTimer2A( void ) {
  // Enable Run Clock Gate Control
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER2;
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER2;

  // 32-bit timer
  GPTM_TIMER2[GPTM_CFG] &= ~GPTM_CFG_M;
  GPTM_TIMER2[GPTM_CFG] |= GPTM_CFG_32BITTIMER;

  // periodic timer
  GPTM_TIMER2[GPTM_TAMR] &= ~GPTM_TAMR_TAMR_M;
  GPTM_TIMER2[GPTM_TAMR] |= GPTM_TAMR_TAMR_PERIODIC;

  // Set the time-out interrupt mask.
  // INSERT CODE HERE
  GPTM_TIMER2[GPTM_IMR] |= GPTM_IMR_TATOIM;

  // Enable timer interrupt in NVIC.
  // INSERT CODE HERE
  PPB[PPB_EN0] |= PPB_EN0_TIMER2A;

}

// Enables/disables Timer2A. If enabled is true, the timer is enabled.
void enableTimer2A( bool enabled ) {
  clearTimer2A();

  // Enable or disable the timer.
  // INSERT CODE HERE
  GPTM_TIMER2[GPTM_CTL] |= (GPTM_CTL_TAEN | GPTM_CTL_TASTALL);

}

// Sets Timer2A to reload at the frequency rate (in Hz). 
void setTimer2ARate( uint16_t rate ) {
  GPTM_TIMER2[GPTM_TAILR] = (MAINOSCFREQ/rate)-1;
}

// Clears the time-out flag for Timer2A (which is set on a time-out).
void clearTimer2A( void ) {
  GPTM_TIMER2[GPTM_ICR] |= GPTM_ICR_TATOCINT;
}
