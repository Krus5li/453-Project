/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the Empty PSoC6 Application
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
*******************************************************************************
* (c) 2019-2021, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#define ENABLE_I2C 1
#define ENABLE_HEARTRATE_TEST 1

/******************************************************************************
 * These MACROS where defined using information for the AT42QT2120 datasheet.
 * and the ECE453 Development Platform schematics.
 *****************************************************************************/
#define BLUE_LED_REG_ADDR				(28+4)
#define BLUE_LED_REG_VAL_ON				(0x3)
#define BLUE_LED_REG_VAL_OFF			(0x1)

#define GREEN_LED_REG_ADDR				(28+5)
#define GREEN_LED_REG_VAL_ON			(0x3)
#define GREEN_LED_REG_VAL_OFF			(0x1)

#define YELLOW_LED_REG_ADDR				(28+6)
#define YELLOW_LED_REG_VAL_ON			(0x3)
#define YELLOW_LED_REG_VAL_OFF			(0x1)

#define RED_LED_REG_ADDR				(28+7)
#define RED_LED_REG_VAL_ON				(0x3)
#define RED_LED_REG_VAL_OFF				(0x1)



int main(void)
{
	uint8_t button_status;

    console_init();

    printf("\x1b[2J\x1b[;H");

    printf("******************\n\r");
    printf("* ECE453 Dev Platform\n\r");

    printf("* -- Initializing user push button\n\r");
	push_button_init();

    printf("* -- Initializing user LED\n\r");
    leds_init();

#if ENABLE_I2C
    printf("* -- Initializing I2C Bus\n\r");
    i2c_init();

    printf("* -- Initializing LSM6DSRX\n\r");
    LSM6DSRX_config();


    printf("* -- Initializing BH1792GLC\n\r");
    BH1792GLC_config();

    button_status = AT42QT2120_read_buttons();
    bool polling_mode = false;
    bool alert_mode = false;

#endif


    printf("* -- Enabling Interrupts\n\r");
	/* Enable global interrupts */
	__enable_irq();

	printf("* -- Setting Up Timer\n\r");
	cyhal_timer_t timer_obj;
	cyhal_timer_t timer_1s;
	const cyhal_timer_cfg_t timer_cfg =
	    {
	        .compare_value = 0,                 	 	// Timer compare value, not used
	        .period        = 20000,              		// Timer period set to a large enough value	compared to event being measured
	        .direction     = CYHAL_TIMER_DIR_UP, 		// Timer counts up
	        .is_compare    = false,              		// Don't use compare mode
	        .is_continuous = false,              		// Do not run timer indefinitely
	        .value         = 0                   		// Initial value of counter
	    };
    printf("****************** \r\n\n");

/*
#if ENABLE_HEARTRATE_TEST
    while(!ALERT_PUSH_BUTTON) {
    	printf("HeartRate: %d", BH1792GLC_read_data());
    	cyhal_system_delay_ms(200);
    }
#endif
*/
#if ENABLE_I2C
while(1) {
	if(ALERT_PUSH_BUTTON) {
		ALERT_PUSH_BUTTON = false;
		/*
		cy_rslt_t rslt_1s;
		rslt_1s = cyhal_timer_init(&timer_1s, NC, NULL);
		cyhal_timer_configure(&timer_1s, &timer_cfg);
		cyhal_timer_set_frequency(&timer_1s, 10000);
		cyhal_timer_start(&timer_1s);
		uint32_t read_val_1s = 0;
		uint32_t read_val_1s_old = read_val_1s;
		*/
		while(ALERT_PUSH_BUTTON == false) {
			/*
			read_val_1s_old = read_val_1s;
			read_val_1s = cyhal_timer_read(&timer_1s);
			if(read_val_1s - read_val_1s_old  >= 9000) {
				BH1792GLC_write_reg(BH1792GLC_SYNC, 0x01);
			}
			*/
			printf("FIFO CNT: %d\n\r", BH1792GLC_read_reg(BH1792GLC_FIFO_CNT));
			printf("HeartRate: %d\n\r", BH1792GLC_read_data());

			if(BH1792GLC_read_reg(BH1792GLC_FIFO_CNT) == 0) {
				BH1792GLC_write_reg(BH1792GLC_SYNC, 0x01);
			}

			cyhal_system_delay_ms(50);
		}
		ALERT_PUSH_BUTTON = false;
		while(!polling_mode) {
			if(LSM6DSRX_check_freefall()) {
				printf("===== FALL DETECTED =====\n\r");
				LSM6DSRX_write_reg(LSM6DSRX_FIFO_CTRL4, LSM6DSRX_FIFO_BUFFER_RESET);		//Empty FIFO buffer/Bypass Mode.
				polling_mode = true;
				ALERT_PUSH_BUTTON = false;
			}

		}
		polling_mode = false;
		// Initialize the timer object. Does not use pin output ('pin' is NC) and does not use a
		// pre-configured clock source ('clk' is NULL).
		cy_rslt_t rslt;
		rslt = cyhal_timer_init(&timer_obj, NC, NULL);
		cyhal_timer_configure(&timer_obj, &timer_cfg);
		cyhal_timer_set_frequency(&timer_obj, 10000);
		cyhal_timer_start(&timer_obj);
		uint32_t read_val = 0;
		ALERT_PUSH_BUTTON = false;
		while(!alert_mode) {
			cyhal_system_delay_ms(500);
			printf("IN ALERT MODE\n\r");
			// Read the current timer value, which should be close to the amount of delay in ms * 10 (5000)
			read_val = cyhal_timer_read(&timer_obj);
			printf("Time: %d\n\r", read_val);
			if(ALERT_PUSH_BUTTON) {
				printf("===== ALERT CALLED OFF =====\n\r");
				cyhal_timer_stop(&timer_obj);
				alert_mode = true;
				ALERT_PUSH_BUTTON = false;
				break;
			}
			if(read_val >= 50000) {
				cyhal_timer_stop(&timer_obj);
				while(1) {
					printf("==== CALL FOR HELP ====\n\r");
					cyhal_system_delay_ms(500);
				}
			}
		}
		alert_mode = false;
	}
	printf("WAITING TO START\n\r");
	cyhal_system_delay_ms(500);
}
#endif
    	/*
    	if(ALERT_PUSH_BUTTON)
    	{
    		ALERT_PUSH_BUTTON = false;
    		printf("ALERT_PUSH_BUTTON Pressed!\n\r");

    	}

    	if(ALERT_CONSOLE_RX)
		{
			ALERT_CONSOLE_RX = false;
			printf("\r\nRx --> %s\r\n", pcInputString);
			cInputIndex = 0;
			memset(pcInputString,0,DEBUG_MESSAGE_MAX_LEN);
		}
		*/
}



/* [] END OF FILE */
