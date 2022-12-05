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

#define FIFO_BUFF_SIZE								0x20 	//Buffer size of 32 events

#define LSM6DSRX_SUB_ADDR							0x6A
#define LSM6DSRX_CTRL1_XL_SUB_ADDR					0x10
#define LSM6DSRX_CTRL1_XL_NORMAL					0xA3
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

#define LSM6DSRX_FIFO_CTRL1							0x07

#define LSM6DSRX_FIFO_CTRL2							0x08
#define LSM6DSRX_FIFO_CONFIG						0xC6		//Set FIFO to stop getting data when full, batch per 32

#define LSM6DSRX_FIFO_CTRL3							0x09
#define LSM6DSRX_FIFO_CTRL3_XL						0x0A

#define LSM6DSRX_FIFO_CTRL4							0x0A
#define LSM6DSRX_FIFO_CTRL4_CONT_MODE				0x06
#define LSM6DSRX_FIFO_CTRL4_FIFO_MODE				0x01
#define LSM6DSRX_FIFO_BUFFER_RESET					0x00

#define LSM6DSRX_FIFO_STATUS1						0x3A

#define LSM6DSRX_FIFO_STATUS2						0x3B

#define LSM6DSRX_INT_ADDR							0x1A


void LSM6DSRX_write_reg(uint8_t reg, uint8_t value);

uint8_t LSM6DSRX_read_reg(uint8_t reg);

uint8_t LSM6DSRX_read_X_L(void);

uint8_t LSM6DSRX_read_X_H(void);

uint8_t LSM6DSRX_read_Y_L(void);

uint8_t LSM6DSRX_read_Y_H(void);

uint8_t LSM6DSRX_read_Z_L(void);

uint8_t LSM6DSRX_read_Z_H(void);

uint16_t hexConcat(uint8_t val_L, uint8_t val_H);

int16_t LSM6DSRX_read_data_X(void);

int16_t LSM6DSRX_read_data_Y(void);

int16_t LSM6DSRX_read_data_Z(void);

bool LSM6DSRX_check_freefall(void);

void LSM6DSRX_buffer_reset(void);

int16_t getBatch(void);

void LSM6DSRX_config(void);

#endif /* LSM6DSRX_H_ */
