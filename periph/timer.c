/*
 * timer.c
 *
 *  Created on:  Aug 3, 2020
 *  Modified on: Nov 6, 2020
 *      Author: khughes
 *
 */

#include <stdint.h>
#include <stdbool.h>

// The code relies on a symbol "USING_TIMER3A" that controls whether to 
// compile the SYSTICK code or your Timer3A code.  You will need to define 
// this symbol within the peripheral project configuration.  DO NOT define
// within this file!

#ifdef USING_TIMER3A

#include "sysctl.h"
#include "gptm.h"


#else

// Move these definitions into a "ppb.h" header file.  Create it using the
// same format as sysctl.h and gpio.h.

/*
#define PPB                     ((volatile uint32_t *) 0xe000e000)

enum {
  PPB_STCTRL = (0x10 >> 2),
#define   PPB_STCTRL_COUNT   (1<<16)
#define   PPB_STCTRL_CLK_SRC (1<<2)
#define   PPB_STCTRL_INTEN   (1<<1)
#define   PPB_STCTRL_ENABLE  (1<<0)
  PPB_STRELOAD = (0x14 >> 2),
  PPB_STCURRENT = (0x18 >> 2),
};
*/

#include "ppb.h"

#endif


// osc.h defines MAINOSCFREQ
#include "osc.h"

// Frequency of switching between left and right LED display
#define   SWITCHFREQ    200

// Add the calculation for your reload value here.
#define RELOAD_VALUE    (MAINOSCFREQ/SWITCHFREQ)

#define PRESCALE    10

#define RELOAD_VALUE_16    (MAINOSCFREQ/SWITCHFREQ)/PRESCALE
// bits required is 20 --> 32 bit timer
// Configure Timer3A as a periodic timer with a period of 1/200th second.
// The names of each register you will need are defined in gptm.h; you do not 
// need to use any registers not named there.  You will need to define the 
// offsets for each bit or field in the registers.
//
// The "size" parameter should be either 16 or 32, depending on whether
// Timer3A should be configured as a 16-bit or 32-bit timer.
// 
// ALSO: remember to enable the Run Mode Clock Gating for the timer before
// attempting to access any of its registers!
void initTimer( uint8_t size ) {
#ifdef USING_TIMER3A
    // Enable Run Clock Gate Control
    SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER3A;
    SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER3A;

    // Disable timer (not needed on first initialization)
    GPTM_TIMER3[GPTM_CTL] &= ~GPTM_CTL_TAEN;

    if (size == 32) {
        // Configure as 32-bit timer
        GPTM_TIMER3[GPTM_CFG] &= ~GPTM_CFG_M;
        GPTM_TIMER3[GPTM_CFG] |= GPTM_CFG_32BITTIMER;
        // Configure as periodic timer
        GPTM_TIMER3[GPTM_TAMR] = ( GPTM_TIMER3[GPTM_TAMR] & ~GPTM_TAMR_TAMR_M) | GPTM_TAMR_TAMR_PERIODIC;

        // Set reload value (assume 120 MHz clock)
        GPTM_TIMER3[GPTM_TAILR] = (RELOAD_VALUE-1);
    }

    else if (size == 16) {
        // Configure as 16-bit timer
        GPTM_TIMER3[GPTM_CFG] &= ~GPTM_CFG_M;
        GPTM_TIMER3[GPTM_CFG] |= GPTM_CFG_16BITTIMERS;
        GPTM_TIMER3[GPTM_TAPR] &= ~GPTM_TAPR_TAPSR_M;
        GPTM_TIMER3[GPTM_TAPR] |= PRESCALE;
        GPTM_TIMER3[GPTM_TAMR] = ( GPTM_TIMER3[GPTM_TAMR] & ~GPTM_TAMR_TAMR_M) | GPTM_TAMR_TAMR_PERIODIC;
        GPTM_TIMER3[GPTM_TAILR] = (RELOAD_VALUE_16-1);
    }

    // Clear timerA time-out flag
    GPTM_TIMER3[GPTM_ICR] = GPTM_ICR_TATOCINT;

    // Enable timer
    GPTM_TIMER3[GPTM_CTL] |= GPTM_CTL_TAEN | GPTM_CTL_TASTALL;

#else

  PPB[PPB_STCTRL] = 0;
  PPB[PPB_STRELOAD] = RELOAD_VALUE-1;
  PPB[PPB_STCTRL] = PPB_STCTRL_CLK_SRC | PPB_STCTRL_ENABLE;

#endif
}

// Wait for Timer3A to reload, then return.  Perform any necessary actions to
// clear the reload condition.  DO NOT turn off the timer!
void waitOnTimer( void ) {
#ifdef USING_TIMER3A
    // wait for timerA time-out flag to set
    while((GPTM_TIMER3[GPTM_RIS] & GPTM_RIS_TATORIS) == 0);

    // clear timerA time-out flag
    GPTM_TIMER3[GPTM_ICR] = GPTM_ICR_TATOCINT;

#else

  while( ( PPB[PPB_STCTRL] & PPB_STCTRL_COUNT ) == 0 );

#endif
}

