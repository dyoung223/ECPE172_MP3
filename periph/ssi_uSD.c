/*
 * ssi_uSD.c
 *
 *  Created on:  Aug 8, 2018
 *  Modified on: Oct 7, 2019
 *  Modified on: Aug 1, 2020
 *      Author: khughes
 */

#include <stdint.h>
#include <stdbool.h>

// SYSCTL, GPIO, and QSSI definitions.
#include "osc.h"
#include "sysctl.h"
#include "gpio.h"
#include "qssi.h"

// Slow bus speed = 400 KHz
#define  SLOW_SPEED      400000

// Fast bus speed = 12.5 MHz
#define  FAST_SPEED      12500000

// Prescalar
#define  PRESCALAR       4   //your prescaler value

// Constants for setting slow and fast speeds
#define SLOW_BAUD ((MAINOSCFREQ/SLOW_SPEED/PRESCALAR)-1)
#define FAST_BAUD ((MAINOSCFREQ/FAST_SPEED/PRESCALAR)-1)

// Initializes the QSSI3 module on PORTQ for the MicroSD card.
void initSSI3( void ) {
  // Enable PORTQ and QSSI3 gate clocks.
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTQ;
  SYSCTL[SYSCTL_RCGCSSI] |= SYSCTL_RCGCSSI_SSI3;
  SYSCTL[SYSCTL_RCGCSSI] |= SYSCTL_RCGCSSI_SSI3;

  // Set alternate function on PORTQ
  GPIO_PORTQ[GPIO_AFSEL] |= (GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3);
  GPIO_PORTQ[GPIO_AFSEL] &= ~GPIO_PIN_1;
  GPIO_PORTQ[GPIO_PCTL]  &= ~((0xf << (4*0)) | (0xf << (4*2)) | (0xf << (4*3)));
  GPIO_PORTQ[GPIO_PCTL]  |=     (14 << (4*0)) | (14 << (4*2)) | (14 << (4*3));
  // Enable digital I/O for PQ1 on PORTQ
  GPIO_PORTQ[GPIO_DIR] |= GPIO_PIN_1;
  //GPIO_PORTQ[GPIO_DATA] |= GPIO_PIN_1;
  GPIO_PORTQ[GPIO_PIN_1] |= GPIO_PIN_1;
  GPIO_PORTQ[GPIO_DEN] |= (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
  // Disable SSI3
  QSSI3[QSSI_SSICR1] &= ~QSSI_CR1_SSE;
  // Set master mode
  QSSI3[QSSI_SSICR1] &= ~QSSI_CR1_MS;
  // Set prescaler
  QSSI3[QSSI_SSICPSR] |= PRESCALAR;
  // Set CR0 for Freescale frame format, Mode 0 (clock normally low, data 
  //     captured on first clock edge), 8-bit data
  QSSI3[QSSI_SSICR0] &= QSSI_CR0_FRF_MOTO;
  QSSI3[QSSI_SSICR0] &= ~QSSI_CR0_SPH;
  QSSI3[QSSI_SSICR0] |= QSSI_CR0_DSS_8;
  // Enable SSI3
  QSSI3[QSSI_SSICR1] |= QSSI_CR1_SSE;

}

// Sets the "baud rate" divisor to generate either a slow speed (~400KHz) 
// or high speed (~12.5MHz). If fast is false then low speed is configured; 
// this is used during card initialization.
void setSSI3Speed( bool fast ) {
  // Clear the current baud rate divisor value
  QSSI3[QSSI_SSICR0] &= ~QSSI_CR0_SCR_M;

  switch( fast ) {
  case false:
    QSSI3[QSSI_SSICR0] |= SLOW_BAUD << QSSI_CR0_SCR_S;// 400KHz speed
    break;
  default:
    QSSI3[QSSI_SSICR0] |= FAST_BAUD << QSSI_CR0_SCR_S;// 12.5MHz speed
    break;
  }
}

// Transmit the byte data to the slave over QSSI3.
void txByteSSI3( uint8_t data ) {
  // wait until TX FIFO is not full
  while ((QSSI3[QSSI_SR] & QSSI_SR_TNF) == 0){
      // wait
  }
  // write data to TX FIFO
  QSSI3[QSSI_DR] = data;
  // wait until RX FIFO is not empty
  while ((QSSI3[QSSI_SR] & QSSI_SR_RNE) == 0){
      // wait
  }
  // read value from RX FIFO and discard
  uint32_t discard_val = QSSI3[QSSI_DR];
}

// Receive a byte from the slave over QSSI3.
uint8_t rxByteSSI3( void ) {
  // wait until TX FIFO is not full
  while ((QSSI3[QSSI_SR] & QSSI_SR_TNF) == 0){
      //wait
  }
  // write to TX FIFO
  QSSI3[QSSI_DR] = 0xff;
  // wait until RX FIFO is not empty
  while ((QSSI3[QSSI_SR] & QSSI_SR_RNE) == 0){
        // wait
  }
  // read value from RX FIFO
  uint8_t data = QSSI3[QSSI_DR];
  return data;
}

// Set the chip select output level on PQ1. If on is true the pin is set 
// low. The pin must not be changed when QSSI3 is busy.
void assertCS( bool level ) {
    while ((QSSI3[QSSI_SR] & QSSI_SR_BSY) != 0){
        //wait until not busy
    }
    if (level == false) {
        GPIO_PORTQ[GPIO_PIN_1] = 0xff;
    }
    else if (level == true) {
        GPIO_PORTQ[GPIO_PIN_1] = 0;
    }
}
