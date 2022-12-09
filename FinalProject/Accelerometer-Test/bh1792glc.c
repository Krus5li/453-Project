/*
 * bh1792glc.c
 *
 *  Created on: Nov 26, 2022
 *      Author: krusl
 */

#include "main.h"
#include <stdlib.h>
#include "bh1792glc.h"
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

/*
 * Desc: Basic Master write towards a register protocol
 *
 * INPUTS:
 * uint8_t reg: Register value to be written to
 * uint8_t value: Value to be written to register
 */


uint16_t hexCat(uint8_t val_L, uint8_t val_H) {
	uint16_t res = 0x0000;
	res = val_H << 8;
	res = res + val_L;
	return res;
}

void BH1792GLC_write_reg(uint8_t reg, uint8_t value) {
	uint8_t write_buffer[2];
	write_buffer[0] = reg;
	write_buffer[1] = value;

	cyhal_i2c_master_write	(	&i2c_master_obj,				// i2c object
								BH1792GLC_SUB_ADDR,				// register address
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
uint8_t BH1792GLC_read_reg(uint8_t reg) {
	uint8_t write_buffer[1];
	uint8_t read_buffer[1];

	write_buffer[0] = reg;

	/* Use cyhal_i2c_master_write to write the register address to the device. */
	/* Send the register address, do not generate a stop condition.  This will result in */
	/* a restart condition. */
	cyhal_i2c_master_write	(		&i2c_master_obj,				// i2c object
									BH1792GLC_SUB_ADDR,				// register address
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
								BH1792GLC_SUB_ADDR,
								read_buffer,
								1,
								0,
								false
							);
	// Return the data
	return read_buffer[0];
}

uint16_t BH1792GLC_burstread_reg(uint8_t reg) {
	uint8_t write_buffer[1];
	uint8_t read_buffer[2];

	write_buffer[0] = reg;

	/* Use cyhal_i2c_master_write to write the register address to the device. */
	/* Send the register address, do not generate a stop condition.  This will result in */
	/* a restart condition. */
	cyhal_i2c_master_write	(		&i2c_master_obj,				// i2c object
									BH1792GLC_SUB_ADDR,				// register address
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
								BH1792GLC_SUB_ADDR,
								read_buffer,
								2,
								0,
								false
							);
	// Return the data
	return hexCat(read_buffer[0], read_buffer[1]);
}
// Should read one piece of 16-bit heart rate data.
uint16_t BH1792GLC_read_data() {

	uint16_t h_rate = 0;
	h_rate = BH1792GLC_burstread_reg(BH1792GLC_FIFO_LO_0);
	return h_rate;
}

uint16_t BH1792GLC_findAvg() {
	cyhal_timer_t timer_obj1;
	const cyhal_timer_cfg_t timer_cfg1 =
	    {
	        .compare_value = 0,                 	 	// Timer compare value, not used
	        .period        = 200000,              		// Timer period set to a large enough value	compared to event being measured
	        .direction     = CYHAL_TIMER_DIR_UP, 		// Timer counts up
	        .is_compare    = false,              		// Don't use compare mode
	        .is_continuous = false,              		// Do not run timer indefinitely
	        .value         = 0                   		// Initial value of counter
	    };
	cy_rslt_t rslt;
	rslt = cyhal_timer_init(&timer_obj1, NC, NULL);
	cyhal_timer_configure(&timer_obj1, &timer_cfg1);
	cyhal_timer_set_frequency(&timer_obj1, 10000);
	cyhal_timer_start(&timer_obj1);
	uint32_t hrm_total = 0;
	uint16_t samples = 0;
	while(cyhal_timer_read(&timer_obj1) > 30000) {
		hrm_total += BH1792GLC_read_data();
		samples += 1;
	}
	uint16_t hrm_avg = hrm_total/samples;
	return hrm_avg;

}

uint16_t BH1792GLC_AvgCalib() {
	int i = 0;
	uint32_t val = 0;
	for(i = 0; i < 5; i++) {
		val += BH1792GLC_read_data();
	}
	return val/5;
}
// True are signal peaks
// False are signal troughs
bool BH1792GLC_get_hrm_raw(uint16_t avg) {
	if(BH1792GLC_read_data() <  avg) {
		return false;
	}
	else {
		return true;
	}
}


void BH1792GLC_config() {
	BH1792GLC_write_reg(BH1792GLC_RESET, 0x80);		//Reset the heart rate sensor register values.
	BH1792GLC_write_reg(BH1792GLC_CTRL1_ADDR, BH1792GLC_CTRL1_CONFIG);
	BH1792GLC_write_reg(BH1792GLC_CTRL2_ADDR, BH1792GLC_CTRL2_CONFIG);
	BH1792GLC_write_reg(BH1792GLC_CTRL3_ADDR, BH1792GLC_CTRL3_CONFIG);
	BH1792GLC_write_reg(BH1792GLC_CTRL5_ADDR, BH1792GLC_CTRL5_CONFIG);
	BH1792GLC_write_reg(BH1792GLC_START, 0x01);
	BH1792GLC_write_reg(BH1792GLC_SYNC, 0x01);
}


