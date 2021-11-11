/*
 * gptm.h
 *
 *  Created on: July 29, 2018
 *      Author: khughes
 *
 * General-purpose Timer Module register definitions.  See Chapter 13 of 
 * the TM4C1294 datasheet for complete information.
 */

#ifndef _GPTM_H
#define _GPTM_H

#include <stdint.h>

// Peripheral base addresses.
#define GPTM_TIMER2             ((volatile uint32_t *)0x40032000)
#define GPTM_TIMER3             ((volatile uint32_t *)0x40033000)

// Peripheral register offsets and special fields
enum {
  GPTM_CFG =    (0x000 >> 2),
#define GPTM_CFG_M              (0b111 << 0)    // GPTM Configuration
#define GPTM_CFG_32BITTIMER       (0 << 0)        // 32-bit timer
#define GPTM_CFG_32BITRTC         (1 << 0)        // 32-bit RTC
#define GPTM_CFG_16BITTIMERS      (4 << 0)        // 2 16-bit timers
  GPTM_TAMR =   (0x004 >> 2),
#define GPTM_TAMR_TAMR_M        (0b11 << 0)     // GPTM Timer A Mode
#define GPTM_TAMR_TAMR_ONESHOT    (1 << 0)        // One-Shot Timer mode
#define GPTM_TAMR_TAMR_PERIODIC   (2 << 0)        // Periodic Timer mode
#define GPTM_TAMR_TAMR_CAP        (3 << 0)        // Capture mode
  GPTM_CTL =    (0x00c >> 2),                   // GPTM Control
#define GPTM_CTL_TASTALL          (1 << 1)        // GPTM Timer A Stall Enable
#define GPTM_CTL_TAEN             (1 << 0)        // GPTM Timer A Enable
  GPTM_RIS =    (0x01c >> 2),                   // GPTM Raw Interrupt Status
#define GPTM_RIS_TATORIS          (1 << 0)        // GPTM Timer A Time-Out Interrupt Mask
  GPTM_ICR =    (0x024 >> 2),                   // GPTM Interrupt Clear
#define GPTM_ICR_TATOCINT       GPTM_RIS_TATORIS
  GPTM_TAILR =  (0x028 >> 2),                   // GPTM Timer A Load Interval
  GPTM_TAPR =   (0x038 >> 2),                   // GPTM TImer A Prescale
#define GPTM_TAPR_TAPSR_M         (0xff << 0)     // GPTM Timer A Prescale Mask
  GPTM_IMR =    (0x018 >> 2),                     // GPTM Timer Interrupt Mask
#define GPTM_IMR_TATOIM           (1 << 0)        // GPTM Timer A Time-out Interrupt Mask
};

#endif // _GPTM_H
