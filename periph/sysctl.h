/*
 * sysctl.h
 *
 *  Created on: July 29, 2018
 *      Author: khughes
 *
 * System Control register definitions.  See Chapter 5 of the TM4C1294 
 * datasheet for complete information.
 */

#ifndef _SYSCTL_H
#define _SYSCTL_H

#include <stdint.h>

// Peripheral base addresses.
#define SYSCTL          (((volatile uint32_t *)0x400fe000))

// Peripheral register offsets and special fields.
enum {
  SYSCTL_RCGCGPIO =       (0x608 >> 2),
#define   SYSCTL_RCGCGPIO_PORTD (1 << 3)  // Port D
#define   SYSCTL_RCGCGPIO_PORTE (1 << 4)   // Port E = Bit 4 for SYSCTL_RCGCGPIO
#define   SYSCTL_RCGCGPIO_PORTJ (1 << 8)
#define   SYSCTL_RCGCGPIO_PORTL (1 << 10)
#define   SYSCTL_RCGCGPIO_PORTM (1 << 11)
#define   SYSCTL_RCGCGPIO_PORTN (1 << 12)
#define   SYSCTL_RCGCGPIO_PORTQ (1 << 14)

  SYSCTL_RCGCTIMER =      (0x604 >> 2),
#define   SYSCTL_RCGCTIMER_TIMER2 (1 << 2)
#define   SYSCTL_RCGCTIMER_TIMER3A (1 << 3)

  SYSCTL_RCGCSSI =        (0x61c >> 2),
#define   SYSCTL_RCGCSSI_SSI3     (1 << 3)

  SYSCTL_RIS =            (0x050 >> 2),
#define   SYSCTL_RIS_MOSCPUPRIS   (1<<8)  // MOSC Power Up Raw Interrupt Status

  SYSCTL_MISC =           (0x058 >> 2),
#define   SYSCTL_MISC_MOSCPUMIS   (1<<8)  // MOSC Power Up Masked Interrupt Status

  SYSCTL_MOSCCTL =        (0x07C >> 2),
#define   SYSCTL_MOSCCTL_OSCRNG   (1<<4) // Oscillator Range
#define   SYSCTL_MOSCCTL_PWRDN    (1<<3) // Power Down
#define   SYSCTL_MOSCCTL_NOXTAL   (1<<2) // No Crystal Connected
#define   SYSCTL_MOSCCTL_MOSCIM   (1<<1) // MOSC Failure Action
#define   SYSCTL_MOSCCTL_CVAL     (1<<0) // Clock Validation for MOSC

  SYSCTL_RSCLKCFG =       (0x0B0 >> 2),
#define   SYSCTL_RSCLKCFG_MEMTIMU (1<<31)  // Memory Timing Register Update
#define   SYSCTL_RSCLKCFG_NEWFREQ (1<<30)  // New PLLFREQ Accept
#define   SYSCTL_RSCLKCFG_USEPLL  (1<<28)  // Use PLL
#define   SYSCTL_RSCLKCFG_PLLSRC_M     (0b1111 << 24)  // MOSC is the PLL input clock
#define   SYSCTL_RSCLKCFG_PLLSRC_MOSC  (3 << 24)  // MOSC is the PLL input clock
#define   SYSCTL_RSCLKCFG_OSCSRC_M     (0b1111 << 20)  // Oscillator Source
#define   SYSCTL_RSCLKCFG_OSCSRC_MOSC  (3 << 20)  // MOSC is oscillator source
#define   SYSCTL_RSCLKCFG_OSYSDIV_M    (0x3ff << 10)  // Oscillator System Clock Divisor
#define   SYSCTL_RSCLKCFG_PSYSDIV_M    (0x3ff << 0)  // PLL System Clock Divisor
#define   SYSCTL_RSCLKCFG_PSYSDIV_3    3

   SYSCTL_MEMTIM0 =       (0x0C0 >> 2),
#define   SYSCTL_MEMTIM0_EBCHT_M   (0b1111 << 22)  // EEPROM Clock High Time
#define   SYSCTL_MEMTIM0_EBCHT_3_5 (6 << 22)  // 3.5 system clock periods
#define   SYSCTL_MEMTIM0_EBCE      (1 <<21)  // EEPROM Bank Clock Edge
#define   SYSCTL_MEMTIM0_EWS_M     (0b1111 << 16)  // EEPROM Wait States
#define   SYSCTL_MEMTIM0_EWS_6     (6 << 16)  // 6 wait states
#define   SYSCTL_MEMTIM0_FBCHT_M   (0b1111 << 6)  // EEPROM Clock High Time
#define   SYSCTL_MEMTIM0_FBCHT_3_5 (6 << 6)  // 3.5 system clock periods
#define   SYSCTL_MEMTIM0_FBCE      (1 << 5)  // EEPROM Bank Clock Edge
#define   SYSCTL_MEMTIM0_FWS_M     (0b1111 << 0)  // EEPROM Wait States
#define   SYSCTL_MEMTIM0_FWS_6     (6 << 0)  // 6 wait states

   SYSCTL_PLLFREQ0 =      (0x160 >> 2),
#define   SYSCTL_PLLFREQ0_PLLPWR   (1 << 23)  // PLL Power
#define   SYSCTL_PLLFREQ0_MFRAC_M  (0x3ff << 10)  // PLL M Fractional Value
#define   SYSCTL_PLLFREQ0_MFRAC_S  10
#define   SYSCTL_PLLFREQ0_MINT_M   (0x3ff << 0) // PLL M Integer Value
#define   SYSCTL_PLLFREQ0_MINT_S   0

   SYSCTL_PLLFREQ1 =      (0x164 >> 2),
#define   SYSCTL_PLLFREQ1_Q_M      (0x1f << 8)  // PLL Q Value
#define   SYSCTL_PLLFREQ1_Q_S      8
#define   SYSCTL_PLLFREQ1_N_M      (0x1f << 0)  // PLL N Value
#define   SYSCTL_PLLFREQ1_N_S      0

  SYSCTL_PLLSTAT =        (0x168 >> 2),
};

#endif // _SYSCTL_H
