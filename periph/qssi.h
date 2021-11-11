/*
 * qssi.h
 *
 *  Created on: Aug 8, 2018
 *      Author: khughes
 *
 * QSSI register definitions.  See Chapter 17 of the TM4C1294 datasheet
 * for complete information.
 */

#ifndef _QSSI_H
#define _QSSI_H

#include <stdint.h>

// Peripheral base addresses.
#define QSSI0                   ((volatile uint32_t *)0x40008000)
#define QSSI3                   ((volatile uint32_t *)0x4000b000)

// Peripheral register offsets and special fields
enum {
  QSSI_SSICR0 = (0x000 >> 2),           // QSSI Control 0
#define QSSI_CR0_SCR_S          8         // SSI Serial Clock Rate Shift
#define QSSI_CR0_SCR_M          (0xff << QSSI_CR0_SCR_S)  // SSI Serial Clock Rate
#define QSSI_CR0_SPH            (1 << 7)  // SSI Serial Clock Phase
#define QSSI_CR0_SPO            (1 << 6)  // SSI Serial Clock Polarity
#define QSSI_CR0_FRF_M          (0b11 << 4)  // SSI Frame Format Select
#define QSSI_CR0_FRF_MOTO         (0 << 4)  // Freescale SPI Frame Format
#define QSSI_CR0_FRF_TI           (1 << 4)  // Synchronous Serial Frame Format
#define QSSI_CR0_DSS_M          (0b1111 << 0)  // SSI Data Size Select
#define QSSI_CR0_DSS_4            (3 << 0)  // 4-bit data
#define QSSI_CR0_DSS_5            (4 << 0)  // 5-bit data
#define QSSI_CR0_DSS_6            (5 << 0)  // 6-bit data
#define QSSI_CR0_DSS_7            (6 << 0)  // 7-bit data
#define QSSI_CR0_DSS_8            (7 << 0)  // 8-bit data
#define QSSI_CR0_DSS_9            (8 << 0)  // 9-bit data
#define QSSI_CR0_DSS_10           (9 << 0)  // 10-bit data
#define QSSI_CR0_DSS_11           (10 << 0)  // 11-bit data
#define QSSI_CR0_DSS_12           (11 << 0)  // 12-bit data
#define QSSI_CR0_DSS_13           (12 << 0)  // 13-bit data
#define QSSI_CR0_DSS_14           (13 << 0)  // 14-bit data
#define QSSI_CR0_DSS_15           (14 << 0)  // 15-bit data
#define QSSI_CR0_DSS_16           (15 << 0)  // 16-bit data
  QSSI_SSICR1 = (0x004 >> 2),           // QSSI Control 1
#define QSSI_CR1_EOM            (1 << 11)  // Stop Frame (End of Message)
#define QSSI_CR1_FSSHLDFRM      (1 << 10)  // FSS Hold Frame
#define QSSI_CR1_HSCLKEN        (1 << 9)   // High Speed Clock Enable
#define QSSI_CR1_DIR            (1 << 8)   // SSI Direction of Operation
#define QSSI_CR1_MODE_M         (0b11 << 6)// SSI Mode
#define QSSI_CR1_MODE_LEGACY     (0 << 6)  // Legacy SSI mode
#define QSSI_CR1_MODE_BI         (1 << 6)  // Bi-SSI mode
#define QSSI_CR1_MODE_QUAD       (2 << 6)  // Quad-SSI Mode
#define QSSI_CR1_EOT            (1 << 4)  // End of Transmission
#define QSSI_CR1_MS             (1 << 2)  // SSI Master/Slave Select
#define QSSI_CR1_SSE            (1 << 1)  // SSI Serial Port Enable
#define QSSI_CR1_LBM            (1 << 0)  // SSI Loopback Mode
  QSSI_DR = (0x008 >> 2),               // QSSI Data
  QSSI_SR = (0x00c >> 2),               // QSSI Status
#define QSSI_SR_BSY             (1 << 4)  // SSI Busy Bit
#define QSSI_SR_RFF             (1 << 3)  // SSI Receive FIFO Full
#define QSSI_SR_RNE             (1 << 2)  // SSI Receive FIFO Not Empty
#define QSSI_SR_TNF             (1 << 1)  // SSI Transmit FIFO Not Full
#define QSSI_SR_TFE             (1 << 0)  // SSI Transmit FIFO Empty
  QSSI_SSICPSR = (0x010 >> 2),          // QSSI Clock Prescale
};

#endif // _QSSI_H
