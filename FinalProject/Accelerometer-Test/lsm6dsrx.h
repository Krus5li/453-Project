/*
 * lsm6dsrx.h
 *
 *  Created on: Oct 31, 2022
 *      Author: krusl
 */

#ifndef LSM6DSRX_H_
#define LSM6DSRX_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "i2c.h"

#define LSM6DSRX_SUB_ADDR							0x6A
#define LSM6DSRX_CTRL1_XL_SUB_ADDR					0x10
#define LSM6DSRX_CTRL1_XL_NORMAL					0xA0
#define LSM6DSRX_CTRL1_XL_HIGH						0x60
#define LSM6DSRX_CTRL2_G_SUB_ADDR					0x11
#define LSM6DSRX_CTRL6_SUB_ADDR						0x15
#define LSM6DSRX_CTRL6_CONFIG						0x00

#define LSM6DSRX_DATA_OUT_X_L						0x79
#define LSM6DSRX_DATA_OUT_X_H						0x7A

#define LSM6DSRX_DATA_OUT_Y_L						0x7B
#define LSM6DSRX_DATA_OUT_Y_H						0x7C

#define LSM6DSRX_DATA_OUT_Z_L						0x7D
#define LSM6DSRX_DATA_OUT_Z_H						0x7E

#define LSM6DSRX_FIFO_CTRL3							0x09
#define LSM6DSRX_FIFO_CTRL4							0x0A
#define LSM6DSRX_FIFO_CTRL3_XL						0x04
#define LSM6DSRX_FIFO_CTRL4_CONT					0x06
#define LSM6DSRX_FIFO_STATUS1						0x3A

void LSM6DSRX_write_reg(uint8_t reg, uint8_t value);

uint8_t LSM6DSRX_read_reg(uint8_t reg);

uint8_t LSM6DSRX_read_XL(void);

int16_t LSM6DSRX_read_X();

int16_t LSM6DSRX_read_Y();

int16_t LSM6DSRX_read_Z();

void LSM6DSRX_config(void);

#endif /* LSM6DSRX_H_ */
