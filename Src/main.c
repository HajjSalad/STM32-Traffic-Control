/**
 * @file main.c
 * @brief Application Entry point for the Traffic Control System
 * 
 * This file contains the system initialization sequence and the 
 * main execution loop.
*/

#include <stdio.h>
#include <stdint.h>
#include "stm32f446xx.h"

#include "uart.h"
#include "exti.h"
#include "queue.h"
#include "lights.h"
#include "systick.h"

/**
 * @brief Initializes all core system peripherals.
 * 
 * This function performs all required hardware and software initialization
 * before the application enters its main execution loop.
 * It must be called once at startup before any application logic is executed.
 * 
 * Initialization order:
 * 	- GPIO configuration for traffic lights
 * 	- External interrupt configuration (EXTI)
 * 	- UART2 initialization for logging output
 * 	- SysTick timer Initialization
 * 	- Logical mapping of traffic light instances
*/
static void system_init(void) {
	lights_init();					// Initialize light GPIO registers
	exti_init();					// Initialize the input interrupts
	uart2_init();					// Initialize UART
	systick_init();					// Initialize SysTick
	map_lights();					// Map the lights
}

/**
 * @brief Main application entry point.
 * 
 * This function initializes the system, sets the initial traffic light 
 * states, and enters an infinite low-power loop.
 * 
 * All runtime behavior is interrupt-driven. Application control flow 
 * transitions to the external interrupt handler @ref EXTI15_10_IRQHandler().
 */
int main() {
	
	// Initialize system peripherals 
	system_init();
	LOG("\n\r** Program Start **");

	// Set the initial traffic light states 
	LOG("Set initial light states");
	lights_set_initial_state();
	
	while(1) {
		__WFI();  // Wait for interrupt (low power mode)
	}
}
