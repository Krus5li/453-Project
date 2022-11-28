/*
 * usr_btn.c
 *
 *  Created on: Sep 9, 2022
 *      Author: Joe Krachey
 */

#include "usr_btn.h"

cyhal_timer_t timer_obj;
int zeroCnt = 0;

//volatile bool ALERT_PUSH_BUTTON = false;

/*****************************************************************************/
/* Interrupt Service Routine for Timer used to Debounce the USR_BTN          */
/*****************************************************************************/
static void isr_usr_btn_timer(void* callback_arg, cyhal_timer_event_t event)
{

	/* ADD Code to debounce the USR_BTN */
	if (!cyhal_gpio_read(USER_BTN)) {
		zeroCnt++;
	} else
		zeroCnt = 0;

	if (zeroCnt == 7) BTN_COUNT++;
}


/*****************************************************************************
/* This function should accomplish the following:
 *
 *		USR_BTN
 *     	- Configure the USR_BTN as an INPUT pin
 *
 *     	TIMER
 * 		- Configure an Timer resource to run at a rate of 10KHz
 * 		- Configure the same timer to generate an interrupt every 10mS.
 *
 * 		TIMER ISR
 * 		-Debounce the USR_BTN so that you only detect the falling edge of the
 * 		push button.
 * 		- A falling edge will occur if the ISR detects the IO pin
 * 		is a value of 1 followed by seven consecutive calls to the ISR where
 * 		the IO pins is a value of 0.
 * 		- When a button press is detected, increment BTN_COUNT
 *
 */
/*****************************************************************************/
cy_rslt_t usr_btn_init(void)
{
	cy_rslt_t rslt;

	/* Configure USER_BTN as an INPUT pin */
	cyhal_gpio_init(USER_BTN,
					CYHAL_GPIO_DIR_INPUT,
					CYHAL_GPIO_DRIVE_PULLUP,
					true);

	// Initialize the timer object. */

	const cyhal_timer_cfg_t timer_cfg =
	    {
	        .compare_value = 0,                  // Timer compare value, not used
	        .period        = 100,               // Defines the timer period
	        .direction     = CYHAL_TIMER_DIR_UP, // Timer counts up
	        .is_compare    = false,              // Don't use compare mode
	        .is_continuous = true,               // Run the timer indefinitely
	        .value         = 0                   // Initial value of counter
	    };

	    // Initialize the timer object. Does not use pin output ('pin' is NC) and does not use a
	    // pre-configured clock source ('clk' is NULL).
	    rslt = cyhal_timer_init(&timer_obj, NC, NULL);

	    // Apply timer configuration such as period, count direction, run mode, etc.
	    if (CY_RSLT_SUCCESS == rslt)
	    {
	        rslt = cyhal_timer_configure(&timer_obj, &timer_cfg);
	    }

	    // Set the frequency of timer to 10000 Hz
	    if (CY_RSLT_SUCCESS == rslt)
	    {
	        rslt = cyhal_timer_set_frequency(&timer_obj, 10000);
	    }

	    if (CY_RSLT_SUCCESS == rslt)
	    {
	        // Assign the ISR to execute on timer interrupt
	        cyhal_timer_register_callback(&timer_obj, isr_usr_btn_timer, NULL);

	        // Set the event on which timer interrupt occurs and enable it
	        cyhal_timer_enable_event(&timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3, true);

	        // Start the timer with the configured settings
	        rslt = cyhal_timer_start(&timer_obj);
	    }

	return rslt;
}

