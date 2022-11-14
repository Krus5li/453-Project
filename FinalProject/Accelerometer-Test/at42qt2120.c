/*
 * opt3001.c
 *
 *  Created on: Oct 20, 2020
 *      Author: Joe Krachey
 */

#include "at42qt2120.h"


volatile bool ALERT_AT42QT2120_CHANGE = false;

/* CapSense Handler Struct */
cyhal_gpio_callback_data_t capsense_cb_data =
{
    .callback     = cap_sense_handler,
    .callback_arg = (void*)NULL
};

/*****************************************************
* Function Name: cap_sense_io_init
******************************************************
* Summary:
* Initializes the IO pin used to control AT42
* change pin
*
* Parameters:
*  void
*
* Return:
*
*
*****************************************************/
void cap_sense_io_init(void)
{
    /* ADD CODE to configure CapSense Change Pin as an input */
	cyhal_gpio_init(PIN_CT_IRQ,
					CYHAL_GPIO_DIR_INPUT,
					CYHAL_GPIO_DRIVE_PULLUP,
					true);
}

/***********************************************************
* Initialize CapSense Change Interrupt IO pin for
* falling edge interrupts
************************************************************/
void cap_sense_irq_init(void)
{
	/* Register the call back function */
	cyhal_gpio_register_callback(
			    PIN_CT_IRQ, 		    // Pin
				&capsense_cb_data);		// Handler Callback Info

    /* Enable the event */
	cyhal_gpio_enable_event(
				PIN_CT_IRQ,	       	// Pin
				CYHAL_GPIO_IRQ_FALL, 	    // Event
				INT_PRIORITY_CT,   // Priority
				true);
}





/*******************************************************************************
* Function Name: cap_sense_handler
********************************************************************************
* Summary:
* Handler for the CT Change line from the CapSense controller
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void cap_sense_handler(void* handler_arg, cyhal_gpio_event_t event)
{
	ALERT_AT42QT2120_CHANGE = true;
}

/** Write a register on the AT42QT2120
 *
 * @param reg The reg address to read
 * @param value The value to be written
 *
 */
void AT42QT2120_write_reg(uint8_t reg, uint8_t value)
{
	uint8_t write_buffer[2];
	/* Initialize write_buffer[] so that the register address and value are transmitted */
	write_buffer[0] = reg;
	write_buffer[1] = value;

	/* Use cyhal_i2c_master_write to write the required data to the device. */
	cyhal_i2c_master_write	(	&i2c_master_obj,				// i2c object
								AT42QT2120_SUBORDINATE_ADDR,	// register address
								write_buffer,	     			// pointer to data
								2,								// data packet size
								0,
								false
							);
}

/** Read a register on the AT42QT2120
 *
 * @param reg The reg address to read
 *
 */
uint8_t AT42QT2120_read_reg(uint8_t reg)
{
	uint8_t write_buffer[1];
	uint8_t read_buffer[1];

	write_buffer[0] = reg;

	/* Use cyhal_i2c_master_write to write the register address to the device. */
	/* Send the register address, do not generate a stop condition.  This will result in */
	/* a restart condition. */
	cyhal_i2c_master_write	(		&i2c_master_obj,				// i2c object
									AT42QT2120_SUBORDINATE_ADDR,	// register address
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
								AT42QT2120_SUBORDINATE_ADDR,
								read_buffer,
								1,
								0,
								false
							);
	// Return the data
	return read_buffer[0];
}

/** Read Chip ID Register
 *
 *
 *
 */
uint8_t AT42QT2120_read_chip_id(void)
{
    return AT42QT2120_read_reg(AT42QT2120_CHIP_ID_REG);
}

/** Read Chip Status Low Register
 *
 *
 *
 */
uint8_t AT42QT2120_read_key_status_lo(void)
{
    return AT42QT2120_read_reg(AT42QT2120_KEY_STATUS_LO_REG);
}

/** Read Chip Status Hi Register
 *
 *
 *
 */
uint8_t AT42QT2120_read_key_status_hi(void)
{
    return AT42QT2120_read_reg(AT42QT2120_KEY_STATUS_HI_REG);
}

/**
 *
 *
 *
 */
uint8_t AT42QT2120_read_detection_status(void)
{
    return AT42QT2120_read_reg(AT42QT2120_DET_STATUS_REG);
}

/**
 *
 *
 *
 */
uint8_t AT42QT2120_read_slider_status(void)
{
    return AT42QT2120_read_reg(AT42QT2120_KEY_SLIDER_POS_REG);
}

uint8_t AT42QT2120_read_buttons(void)
{
	uint8_t junk;
    junk = AT42QT2120_read_key_status_hi();
    junk = AT42QT2120_read_detection_status();
    junk = AT42QT2120_read_slider_status();
    return AT42QT2120_read_key_status_lo();
}


void AT42QT2120_init(void)
{
	cap_sense_io_init();
	cap_sense_irq_init();
}




