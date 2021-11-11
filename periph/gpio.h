/*
 * gpio.h
 *
 *  Created on: July 29, 2018
 *      Author: khughes
 *
 * GPIO register definitions.  See Chapter 10 of the TM4C1294 datasheet 
 * for complete information.
 */

#ifndef _GPIO_H
#define _GPIO_H

#include <stdint.h>

// Peripheral base addresses.
#define GPIO_PORTA              ((volatile uint32_t *)0x40058000)
#define GPIO_PORTB              ((volatile uint32_t *)0x40059000)
#define GPIO_PORTC              ((volatile uint32_t *)0x4005a000)
#define GPIO_PORTD              ((volatile uint32_t *)0x4005b000)
#define GPIO_PORTE              ((volatile uint32_t *)0x4005c000)
#define GPIO_PORTF              ((volatile uint32_t *)0x4005d000)
#define GPIO_PORTG              ((volatile uint32_t *)0x4005e000)
#define GPIO_PORTH              ((volatile uint32_t *)0x4005f000)
#define GPIO_PORTJ              ((volatile uint32_t *)0x40060000)
#define GPIO_PORTK              ((volatile uint32_t *)0x40051000)
#define GPIO_PORTL              ((volatile uint32_t *)0x40062000)
#define GPIO_PORTM              ((volatile uint32_t *)0x40063000)
#define GPIO_PORTN              ((volatile uint32_t *)0x40064000)
#define GPIO_PORTP              ((volatile uint32_t *)0x40065000)
#define GPIO_PORTQ              ((volatile uint32_t *)0x40066000)

// Peripheral register offsets and special fields
enum {
#define   GPIO_PIN_0              (1 << 0) // pin 0
#define   GPIO_PIN_1              (1 << 1) // pin 1
#define   GPIO_PIN_2              (1 << 2) // pin 2
#define   GPIO_PIN_3              (1 << 3) // pin 3
#define   GPIO_PIN_4              (1 << 4) // pin 4
#define   GPIO_PIN_5              (1 << 5) // pin 5
#define   GPIO_PIN_6              (1 << 6) // pin 6
#define   GPIO_PIN_7              (1 << 7) // pin 7
#define   GPIO_ALLPINS            0b11111111 // pins 0 to 7
  GPIO_DATA  =   (0x000 >> 2),
  GPIO_DIR   =   (0x400 >> 2),
  GPIO_PUR   =   (0x510 >> 2),
  GPIO_DEN   =   (0x51c >> 2),
  GPIO_LOCK  =   (0x520 >> 2),
  GPIO_CR    =   (0x524 >> 2),
  GPIO_ODR   =   (0x50c >> 2),
  GPIO_AFSEL =   (0x420 >> 2),
  GPIO_PCTL  =   (0x52c >> 2),
};

#endif // _GPIO_H
