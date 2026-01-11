/**
 * @file lights.c
 * @brief Traffic light control and GPIO management.
 * 
 * This module implements the logic and GPIO control for a multi-direction
 * traffic system.
 * It:
 * 	- Manages traffic light states
 * 	- Handles state transitions
 * 	- Updates LED outputs using atomic GPIO operations
 * 
 * The module operates on a global array of `TrafficLight` structures, where
 * each element represents one traffic light at the intersection.
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

#include "uart.h"
#include "lights.h"
#include "systick.h"
/**
 * @brief Array of Traffic light structures.
 * 
 * @note The array size is defined by NUM_LIGHTS (4 in this case)
*/
TrafficLight Light[NUM_LIGHTS];			// Instantiate 4 traffic light

/**
 * @brief Initialize and map traffic light configuration
 * 
 * Populates the global 'Light' array with initial traffic light states,
 * car counts, and GPIO pin mappings. 
 * Each traffic light is assigned its corresponding RED and GREEN GPIO pins
 * and an initial state based on expected traffic flow.
 * 
 * High-traffic directions are initialized to GREEM, while low-traffic 
 * directions start at RED.
*/
void map_lights(void) 
{
	// fields: state, carCount, redPin, greenPin. 
	Light[0] = (TrafficLight){GREEN, 0, PIN_LIGHT1_RED, PIN_LIGHT1_GREEN};		// High traffic - start with GREEN
	Light[1] = (TrafficLight){RED,   0, PIN_LIGHT2_RED, PIN_LIGHT2_GREEN};		// Low traffic - start with RED
	Light[2] = (TrafficLight){GREEN, 0, PIN_LIGHT3_RED, PIN_LIGHT3_GREEN};		// High traffic - start with GREEN
	Light[3] = (TrafficLight){RED,   0, PIN_LIGHT4_RED, PIN_LIGHT4_GREEN};		// Low traffic - start with RED
}

// Update the appropriate LED through the GPIO output based on state
/**
 * @brief Update traffic light LEDs based on the current light state.
 * 
 * Sets or resets the RED and GREEN GPIO outputs for a single traffic
 * light using the GPIOB BSRR register. 
 * The LED behavior is determined by the `state` field of the provided 
 * TrafficLight structure. 
 *
 * @param light Pointer to a TrafficLight structure containing the current
 * 				state and GPIO pin mappings
*/
void lights_update(const TrafficLight *light) 
{
	switch (light->state) {
		case RED:
			GPIOB->BSRR = (1U << (light->redPin + 16));  	// RED LED ON
			GPIOB->BSRR = (1U << light->greenPin);			// GREEN LED OFF
			break;
		case YELLOW:
			// Turn ON both RED and GREEN to get YELLOW 
			GPIOB->BSRR = ((1U << (light->redPin + 16)) | (1U <<(light->greenPin + 16)));
			break;
		case GREEN:
			GPIOB->BSRR = (1U << (light->greenPin + 16));   // GREEN LED ON
			GPIOB->BSRR = (1U << light->redPin);			// RED LED OFF
			break;
		case OFF:
			// Turn off both RED and GREEN to turn Light off
			GPIOB->BSRR = ((1U << light->redPin) | (1U << light->greenPin));
			break;
	}
}

/**
 * @brief Transition a pair of traffic lights to GREEN.
 * 
 * Transitions the specified pair of traffic lights to GREEN state if 
 * currently RED. If GREEN, no state change is performed.
 * 
 * After updating the logical state, the corresponding GPIO outputs are 
 * updated via `lights_update()`.
 * 
 * @param lightNum1 Index of the first traffic light in the pair
 * @param lightNum2 Index of the second traffic light in the pair
*/
void lights_set_green(int lightNum1, int lightNum2) 
{
	// Check if the light pair is RED
	if (Light[lightNum1].state == RED || Light[lightNum2].state == RED) {
		// Transition directly from RED to GREEN
		Light[lightNum1].state = GREEN;
		Light[lightNum2].state = GREEN;
		LOG("Light %d turned GREEN", lightNum1 + 1);
		LOG("Light %d turned GREEN", lightNum2 + 1);
	} else {
		// Light already GREEN - Nothing to do
		LOG("Light %d is already GREEN", lightNum1 + 1);
		LOG("Light %d is already GREEN", lightNum2 + 1);
	}
	// Update the Light states
	lights_update(&Light[lightNum1]);
	lights_update(&Light[lightNum2]);
}

/**
 * @brief Transition a pair of traffic lights from GREEN to YELLOW
 * 
 * If light in the specified pair is currently GREEN, they are transitioned 
 * to YELLOW state. If RED, no state change is performed.
 * 
 * After updating the logical state, the corresponding GPIO outputs are 
 * updated via `lights_update()`.
 * 
 * @param lightNum1 Index of the first traffic light in the pair
 * @param lightNum2 Index of the second traffic light in the pair
 * 
 * @return 
*/
uint32_t lights_set_yellow(int lightNum1, int lightNum2) 
{
	if (Light[lightNum1].state == GREEN || Light[lightNum2].state == GREEN) {
		// Transition from GREEN to YELLOW
		Light[lightNum1].state = YELLOW;
		Light[lightNum2].state = YELLOW;
		LOG("Light %d turned YELLOW", lightNum1 + 1);
		LOG("Light %d turned YELLOW", lightNum2 + 1);
	} else {
		// Light already RED - Nothing to do
		LOG("Light %d is already RED", lightNum1 + 1);
		LOG("Light %d is already RED", lightNum2 + 1);
	}

	// Update the Light states
	lights_update(&Light[lightNum1]);
	lights_update(&Light[lightNum2]);

	return 1;
}

/**
 * @brief Transition a pair of traffic lights from YELLOW to RED
 * 
 * If light in the specified pair is currently YELLOW, they are transitioned 
 * to RED state.
 * 
 * After updating the logical state, the corresponding GPIO outputs are 
 * updated via `lights_update()`.
 * 
 * @param lightNum1 Index of the first traffic light in the pair
 * @param lightNum2 Index of the second traffic light in the pair
 * 
 * @return 
*/
uint32_t lights_set_red(int lightNum1, int lightNum2) {
	// Check if the Light pair is YELLOW
	if (Light[lightNum1].state == YELLOW || Light[lightNum2].state == YELLOW) {
		// Transition from YELLOW to RED
		Light[lightNum1].state = RED;
		Light[lightNum2].state = RED;
		LOG("Light %d turned RED", lightNum1 + 1);
		LOG("Light %d turned RED", lightNum2 + 1);
	} 

	// Update the Light states
	lights_update(&Light[lightNum1]);
	lights_update(&Light[lightNum2]);

	return 1;
}

/** @brief Set all traffic lights to their initial states */
void lights_set_initial_state(void) {
	for (int i=0; i<NUM_LIGHTS; i++) {
		lights_update(&Light[i]);
		LOG("Light %d is %s", i + 1, (Light[i].state == GREEN) ? "GREEN" : "RED");
	}
}

/**
 * @brief Initializes GPIO output pins
 * 
 * This function enables the required GPIO peripheral clocks and configures 
 * the GPIO pins connected to the traffic light LEDs as digital outputs.
 * 
 * @note Pins are configured in push-pull output mode with default speed 
 * 		 and no internal pull-up or pull-down resistors.
*/
void lights_init(void) {
	RCC->AHB1ENR |= (1U<<0);		// Enable clock GPIOA
	RCC->AHB1ENR |= (1U<<1);		// Enable clock GPIOB
	RCC->AHB1ENR |= (1U<<2);		// Enable clock GPIOC

	// Light 1
	GPIOB->MODER |= (1U<<20);		// PB10 output mode - R
	GPIOB->MODER &= ~(1U<<21);
	GPIOB->MODER |= (1U<<8);		// PB4 output mode - G
	GPIOB->MODER &= ~(1U<<9);

	// Light 2
	GPIOB->MODER |= (1U<<10);		// PB5 output mode - R
	GPIOB->MODER &= ~(1U<<11);
	GPIOB->MODER |= (1U<<6);		// PB3 output mode - G
	GPIOB->MODER &= ~(1U<<7);

	// Light 3
	GPIOB->MODER |= (1U<<4);		// PB2 output mode - R
	GPIOB->MODER &= ~(1U<<5);
	GPIOB->MODER |= (1U<<2);		// PB1 output mode - G
	GPIOB->MODER &= ~(1U<<3);

	// Light 4
	GPIOB->MODER |= (1U<<28);		// PB14 output mode - R
	GPIOB->MODER &= ~(1U<<29);
	GPIOB->MODER |= (1U<<26);		// PB13 output mode - G
	GPIOB->MODER &= ~(1U<<27);
}

	