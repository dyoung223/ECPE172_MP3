/*
 * osc.c: configures the TM4C1294 Main oscillator for 120MHz
 */

#include <stdint.h>
#include "sysctl.h"

void initOsc( void ) {
  uint32_t tmp;

  // 1. Power up the MOSC by clearing the NOXTAL bit in the MOSCCTL register.
  SYSCTL[SYSCTL_MOSCCTL] &= ~(SYSCTL_MOSCCTL_NOXTAL | SYSCTL_MOSCCTL_PWRDN);

  if( !( SYSCTL[SYSCTL_MOSCCTL] & SYSCTL_MOSCCTL_OSCRNG) ) {
    SYSCTL[SYSCTL_MOSCCTL] |= SYSCTL_MOSCCTL_OSCRNG;

  // 2. If single-ended MOSC mode is required, the MOSC is ready to use. If crystal mode is required,
  //    clear the PWRDN bit and wait for the MOSCPUPRIS bit to be set in the Raw Interrupt Status
  //    (RIS), indicating MOSC crystal mode is ready.
    while( !( SYSCTL[SYSCTL_RIS] & SYSCTL_RIS_MOSCPUPRIS ) );
  }
  SYSCTL[SYSCTL_MISC] = SYSCTL_MISC_MOSCPUMIS;

  // 3. Set the OSCSRC field to 0x3 in the RSCLKCFG register at offset 0x0B0.
  SYSCTL[SYSCTL_RSCLKCFG] |= SYSCTL_RSCLKCFG_OSCSRC_MOSC | SYSCTL_RSCLKCFG_PLLSRC_MOSC;

  // 4. If the application also requires the MOSC to be the deep-sleep clock source, then program the
  //    DSOSCSRC field in the DSCLKCFG register to 0x3.

  // 6. Write the PLLFREQ0 and PLLFREQ1 registers with the values of Q, N, MINT, and MFRAC to
  //    the configure the desired VCO frequency setting.
  //    Frequency is 25MHz, MINT = 96, MFRAC = 0, N = 4, Q = 0 -->PLL = 480MHz
  SYSCTL[SYSCTL_PLLFREQ0] = (96 << SYSCTL_PLLFREQ0_MINT_S) | (0 << SYSCTL_PLLFREQ0_MFRAC_S);
  SYSCTL[SYSCTL_PLLFREQ1] = 4;

  // 6. Write the MEMTIM0 register to correspond to the new system clock setting.  There are a
  // number of reserved bits which need to be preserved
  tmp = SYSCTL[SYSCTL_MEMTIM0];
  tmp &= ~(SYSCTL_MEMTIM0_EBCHT_M | SYSCTL_MEMTIM0_EWS_M | SYSCTL_MEMTIM0_EBCE);
  tmp |= SYSCTL_MEMTIM0_FBCHT_3_5 | SYSCTL_MEMTIM0_EWS_6;
  tmp &= ~(SYSCTL_MEMTIM0_FBCHT_M | SYSCTL_MEMTIM0_FWS_M | SYSCTL_MEMTIM0_FBCE);
  tmp |= SYSCTL_MEMTIM0_FBCHT_3_5| SYSCTL_MEMTIM0_FWS_6;
  SYSCTL[SYSCTL_MEMTIM0] = tmp;
  SYSCTL[SYSCTL_PLLFREQ0] |= SYSCTL_PLLFREQ0_PLLPWR;

  // 7. Wait for the PLLSTAT register to indicate the PLL has reached lock at the new operating point
  //    (or that a timeout period has passed and lock has failed, in which case an error condition exists
  //    and this sequence is abandoned and error processing is initiated).
  SYSCTL[SYSCTL_RSCLKCFG] |= SYSCTL_RSCLKCFG_NEWFREQ;
  while( !SYSCTL[SYSCTL_PLLSTAT] );

  // 8. Write the RSCLKCFG register's PSYSDIV value, set the USEPLL bit to enabled, and MEMTIMU bit.
  // SYSCTL[SYSCTL_RSCLKCFG] |= SYSCTL_RSCLKCFG_USEPLL | SYSCTL_RSCLKCFG_MEMTIMU | SYSCTL_RSCLKCFG_PSYSDIV_3;
  tmp = SYSCTL[SYSCTL_RSCLKCFG];
  tmp = (tmp & ~SYSCTL_RSCLKCFG_PSYSDIV_M) | SYSCTL_RSCLKCFG_PSYSDIV_3;
  tmp |= SYSCTL_RSCLKCFG_USEPLL | SYSCTL_RSCLKCFG_MEMTIMU;
  SYSCTL[SYSCTL_RSCLKCFG] = tmp;
}
