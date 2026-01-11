/**
 * @file systick.c
 * @brief SysTick timer configuration and millisecond utilities.
 * 
 * This module configures the Cortex-M4 SysTick timer to generate a
 * 1 ms tick interrupt. It maintains a global millisecond counter 
 * ('systickMillis`) and provides functions with time retrieval and
 * busy-wait delays.
*/

#include "uart.h"
#include "systick.h"
#include "controller.h"

#include <stdio.h>
#include <stdint.h>
#include "stm32f446xx.h"

#define SYSTICK_LOAD_VAL		16000
#define CTRL_ENABLE				(1U<<0)
#define CTRL_CLKSRC				(1U<<2)
#define CTRL_COUNTFLAG			(1U<<16)

volatile uint32_t systickMillis = 0;		// Global variable to store milliseconds

/**
 * @brief SysTick interrupt handler called every 1 millisecond.
 * 
 * Increments the global milliseocnd counter and calls application-specific
 * timeout functions:
 * 	- checkGreenLightTimeout() to release green light after timeout 
 * 	- SysTick_CheckFirstPressTimeout() to handle first button press delay
 * 
 * @note This interrupt handler is invoked by the Cortex-M4 SysTick
 *       hardware every millisecond (or configured tick period)
*/
void SysTick_Handler(void) {
	systickMillis++;						// Increment milliseconds counter
	checkGreenLightTimeout();				 
	SysTick_CheckFirstPressTimeout();		 
}

/**
 * @brief Initialize the SysTick timer for 1ms tick interrupts.
 * 
 * Configures the SysTick LOAD register and control register to generate
 * interrupts every millisecond based on the system clock.
 */
void systick_init(void) {

	SysTick->LOAD = SYSTICK_LOAD_VAL;		// Reload with number of clocks per ms
	SysTick->VAL = 0;						// Clear current SysTick counter value

	// Enable, set clock source, and enable interrupt
	SysTick->CTRL = CTRL_ENABLE | CTRL_CLKSRC | (1U << 1);
}

/**
 * @brief Get the current system uptime in milliseconds.
 * 
 * Returns the number of milliseconds elapsed since SysTick initialization.
 * 
 * @return Current millisecond count
 */
uint32_t systickGetMillis(void) {
	return systickMillis;					// Return the current milliseconds count
}

/**
 * @brief Busy-wait delay for a specified number of milliseconds.
 * 
 * Uses `systickGetMillis()` to implement a simple blocking delay.
 * 
 * @param delay  Number of milliseconds to wait
 * 
 * @note This is a blocking function and will halt CPU execution.
 * 		 Do not use in time-critical or interrupt-sensitive code.
 */
void systickDelayMs(int delay) {
	uint32_t start = systickGetMillis();
	while (systickGetMillis() - start < delay) {}	// Busy-wait for the specified delay
}
