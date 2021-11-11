/*
 * ppb.h
 *
 *  Created on: Sep 8, 2021
 *      Author: kszet
 */

#ifndef PPB_H_
#define PPB_H_

#include <stdint.h>
#include <stdbool.h>

#define PPB                     ((volatile uint32_t *) 0xe000e000)

enum {
  PPB_STCTRL = (0x10 >> 2),
#define   PPB_STCTRL_COUNT   (1<<16)
#define   PPB_STCTRL_CLK_SRC (1<<2)
#define   PPB_STCTRL_INTEN   (1<<1)
#define   PPB_STCTRL_ENABLE  (1<<0)
  PPB_STRELOAD = (0x14 >> 2),
  PPB_STCURRENT = (0x18 >> 2),
  PPB_EN0      = (0x100 >> 2), // NVIC enable register
#define   PPB_EN0_TIMER2A    (1<<23)
};


#endif /* PPB_H_ */
