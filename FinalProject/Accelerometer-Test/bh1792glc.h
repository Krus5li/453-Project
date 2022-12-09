/*
 * bh1792glc.h
 *
 *  Created on: Nov 26, 2022
 *      Author: krusl
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "i2c.h"

#ifndef BH1792GLC_H_
#define BH1792GLC_H_

#define BH1792GLC_SUB_ADDR				0x5B

#define BH1792GLC_RESET					0x40

#define BH1792GLC_CTRL1_ADDR			0x41
#define BH1792GLC_CTRL1_CONFIG			0x80

#define BH1792GLC_CTRL2_ADDR			0x42
#define BH1792GLC_CTRL2_CONFIG			0xCF		// Set LED1 and LED2 to be on 10mA mode

#define BH1792GLC_CTRL3_ADDR			0x43
#define BH1792GLC_CTRL3_CONFIG			0x0F		// Set LED1 and LED2 to be on 10mA mode and LED3 to be off

#define BH1792GLC_CTRL4_LO_ADDR			0x44
#define BH1792GLC_CTRL4_HI_ADDR			0x45

#define BH1792GLC_CTRL5_ADDR			0x46
#define BH1792GLC_CTRL5_CONFIG			0x01		// Watermark Interrupt enable  when sample reaches 32 or more.

#define BH1792GLC_START					0x47
#define BH1792GLC_SYNC					0x48

#define BH1792GLC_FIFO_CNT				0x4B

#define BH1792GLC_FIFO_LO_0				0x4C
#define BH1792GLC_FIFO_HI_0				0x4D

#define BH1792GLC_FIFO_LO_1				0x4E
#define BH1792GLC_FIFO_HI_1				0x4F


void BH1792GLC_write_reg(uint8_t reg, uint8_t value);\

uint8_t BH1792GLC_read_reg(uint8_t reg);

uint16_t hexCat(uint8_t val_L, uint8_t val_H);

uint16_t BH1792GLC_read_data();

uint16_t BH1792GLC_burstread_reg(uint8_t reg);

uint16_t BH1792GLC_AvgCalib();

bool BH1792GLC_get_hrm_raw(uint16_t avg);

void BH1792GLC_config(void);

#endif /* BH1792GLC_H_ */
