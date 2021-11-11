#ifndef _UI_H
#define _UI_H

// Place these definitions in your sysctl.h file, if missing:
#define SYSCTL_RCGCTIMER_TIMER5 (1<<5)
// Place these definitions in your gptm.h and gptm.h file, if missing:
#define GPTM_TIMER5 ((volatile uint32_t *)0x40035000)
// Merge these definitions iyour ppb.h file:
enum {
  PPB_EN2 = (0x108 >> 2),
#define   PPB_EN2_TIMER5A    (1<<(65-2*32))
  PPB_PRI16 = (0x440 >> 2),         // Interrupt 64-67 Priority (PRI16) for Timer5A
#define PPB_PRI_INTB_S  13              // Interrupt [4n+1]  INTB Shift value
#define PPB_PRI_INTB_M (0b111<<PPB_PRI_INTB_S) // Interrupt [4n+1]  INTB Mask
};

void initUI( void );

#endif // _UI_H

