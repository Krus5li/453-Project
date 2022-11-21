/*
 * lsm6dsrx.c
 *
 *  Created on: Oct 31, 2022
 *      Author: krusl
 */

#include "lsm6dsrx.h"
#include <stdlib.h>
#include "main.h"

/*
 * Desc: Basic Master write towards a register protocol
 *
 * INPUTS:
 * uint8_t reg: Register value to be written to
 * uint8_t value: Value to be written to register
 */
void LSM6DSRX_write_reg(uint8_t reg, uint8_t value) {
	uint8_t write_buffer[2];
	write_buffer[0] = reg;
	write_buffer[1] = value;

	cyhal_i2c_master_write	(	&i2c_master_obj,				// i2c object
								LSM6DSRX_SUB_ADDR,		// register address
								write_buffer,	     			// pointer to data
								2,								// data packet size
								0,
								false
							);
}
/*
 * Desc: Basic Master read towards a register protocol
 *
 * INPUTS:
 * uint8_t reg: Register that is to be read
 *
 * Returns: A byte of data from the requested register.
 */
uint8_t LSM6DSRX_read_reg(uint8_t reg) {
	uint8_t write_buffer[1];
	uint8_t read_buffer[1];

	write_buffer[0] = reg;

	/* Use cyhal_i2c_master_write to write the register address to the device. */
	/* Send the register address, do not generate a stop condition.  This will result in */
	/* a restart condition. */
	cyhal_i2c_master_write	(		&i2c_master_obj,				// i2c object
									LSM6DSRX_SUB_ADDR,		// register address
									write_buffer,    				// pointer to data
									1,								// data packet size
									0,
									false
								);

	/* ADD CODE */
	/* Use cyhal_i2c_master_read to read the required data from the device. */
	// The register address has already been set in the write above, so read a single byte
	// of data.
	cyhal_i2c_master_read	(	&i2c_master_obj,
								LSM6DSRX_SUB_ADDR,
								read_buffer,
								1,
								0,
								false
							);
	// Return the data
	return read_buffer[0];
}

/*
 * Desc: Reads data from the lower X-axis register of the FIFO registers
 *
 * Returns: Lower X-axis data
 */
uint8_t LSM6DSRX_read_X_L() {
	uint8_t XL_data_X;													// X data of accelerometer
	XL_data_X = LSM6DSRX_read_reg(LSM6DSRX_DATA_OUT_X_L);
	return XL_data_X;
}

/*
 * Desc: Reads data from the higher X-axis register of the FIFO registers
 *
 * Returns: Higher X-axis data
 */
uint8_t LSM6DSRX_read_X_H() {
	uint8_t XL_data_X;													// X data of accelerometer
	XL_data_X = LSM6DSRX_read_reg(LSM6DSRX_DATA_OUT_X_H);
	return XL_data_X;
}

/*
 * Desc: Reads data from the lower Y-axis register of the FIFO registers
 *
 * Returns: Lower Y-axis data
 */
uint8_t LSM6DSRX_read_Y_L() {
	uint8_t XL_data_Y;													// Y data of accelerometer
	XL_data_Y = LSM6DSRX_read_reg(LSM6DSRX_DATA_OUT_Y_L);
	return XL_data_Y;
}

/*
 * Desc: Reads data from the higher Y-axis register of the FIFO registers
 *
 * Returns: Higher Y-axis data
 */
uint8_t LSM6DSRX_read_Y_H() {
	uint8_t XL_data_Y;													// Y data of accelerometer
	XL_data_Y = LSM6DSRX_read_reg(LSM6DSRX_DATA_OUT_Y_H);
	return XL_data_Y;
}

/*
 * Desc: Reads data from the lower Z-axis register of the FIFO registers
 *
 * Returns: Lower Z-axis data
 */
uint8_t LSM6DSRX_read_Z_L() {
	uint8_t XL_data_Z;													// Z data of accelerometer
	XL_data_Z = LSM6DSRX_read_reg(LSM6DSRX_DATA_OUT_Z_L);
	return XL_data_Z;
}

/*
 * Desc: Reads data from the higher Z-axis register of the FIFO registers
 *
 * Returns: Higher Z-axis data
 */
uint8_t LSM6DSRX_read_Z_H() {
	uint8_t XL_data_Z;													// Z data of accelerometer
	XL_data_Z = LSM6DSRX_read_reg(LSM6DSRX_DATA_OUT_Z_H);
	return XL_data_Z;
}

/*
 * Desc: A function that concatenates two hexes together, in the purpose of this project it is to combine the
 * low axis data with the high axis data to make a 16 bit piece of data
 *
 * INPUTS:
 * uint8_t val_L: The lower axis value
 * uint8_t val_H: The higher axis value
 *
 * Returns: Unsigned 16 bit integer which is the result of the concatenation.
 */
uint16_t hexConcat(uint8_t val_L, uint8_t val_H) {
	uint16_t res = 0x0000;
	res = val_H << 8;
	res = res + val_L;
	return res;
}


int16_t LSM6DSRX_read_data_X() {
	int16_t XL_data_X = hexConcat(LSM6DSRX_read_X_L(), LSM6DSRX_read_X_H());
	return XL_data_X;
}

int16_t LSM6DSRX_read_data_Y() {
	int16_t XL_data_Y = hexConcat(LSM6DSRX_read_Y_L(), LSM6DSRX_read_Y_H());
	return XL_data_Y;
}

int16_t LSM6DSRX_read_data_Z() {
	int16_t XL_data_Z = hexConcat(LSM6DSRX_read_Z_L(), LSM6DSRX_read_Z_H());
	return XL_data_Z;
}

void LSM6DSRX_buffer_reset() {
	LSM6DSRX_write_reg(LSM6DSRX_FIFO_CTRL4, LSM6DSRX_FIFO_BUFFER_RESET);		//Empty/Reset FIFO buffer.
	LSM6DSRX_write_reg(LSM6DSRX_FIFO_CTRL4, LSM6DSRX_FIFO_CTRL4_FIFO_MODE);		//Configure FIFO Mode
}

int16_t* getBatchAvg(int16_t* result) {
	int16_t X_avg = 0;
	int16_t Y_avg = 0;
	int16_t Z_avg = 0;

	int i = 0;
	for(i = 0; i < FIFO_BUFF_SIZE; i++) {
		X_avg += LSM6DSRX_read_data_X();
		Y_avg += LSM6DSRX_read_data_Y();
		Z_avg += LSM6DSRX_read_data_Z();
	}
	result[0] = X_avg / FIFO_BUFF_SIZE;
	result[1] = Y_avg / FIFO_BUFF_SIZE;
	result[2] = Z_avg / FIFO_BUFF_SIZE;
	LSM6DSRX_buffer_reset();

	return result;
}

void LSM6DSRX_config() {
	LSM6DSRX_write_reg(LSM6DSRX_CTRL1_XL_SUB_ADDR, LSM6DSRX_CTRL1_XL_NORMAL); 	//Activate Accelerometer at Normal Mode
	LSM6DSRX_write_reg(LSM6DSRX_CTRL2_G_SUB_ADDR, 0x00);						//Power Down gyroscope
	LSM6DSRX_write_reg(LSM6DSRX_CTRL6_SUB_ADDR, LSM6DSRX_CTRL6_CONFIG); 		//Configure Accelerometer
	LSM6DSRX_write_reg(LSM6DSRX_FIFO_CTRL3, LSM6DSRX_FIFO_CTRL3_XL);			//Configure Accelerometer write baud rate
	LSM6DSRX_buffer_reset();													//Reset the contents of the FIFO buffer
	LSM6DSRX_write_reg(LSM6DSRX_FIFO_CTRL1, FIFO_BUFF_SIZE);					//Configure FIFO Buffer Size to be size 32
	LSM6DSRX_write_reg(LSM6DSRX_FIFO_CTRL2, LSM6DSRX_FIFO_CONFIG);				//Interrupt FIFO when Full, batch per 16

}

