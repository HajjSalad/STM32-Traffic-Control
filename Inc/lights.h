/**
 * @file lights.h
 * @brief Public API for Traffic Light control and GPIO management.
*/

#ifndef LIGHTS_H_
#define LIGHTS_H_

#include <stdint.h>
#include <stdbool.h>

/** @brief GPIO pin assignments for traffic light lEDs */
#define PIN_LIGHT1_RED		10
#define PIN_LIGHT1_GREEN	4    
#define PIN_LIGHT2_RED      5
#define PIN_LIGHT2_GREEN	3
#define PIN_LIGHT3_RED      2
#define PIN_LIGHT3_GREEN	1
#define PIN_LIGHT4_RED      14
#define PIN_LIGHT4_GREEN	13

/** @brief Total number of traffic light in the system */
#define NUM_LIGHTS			4

/** @brief Enumeration of possible traffic light states */
typedef enum {
	RED,        			/**< Stop */
	YELLOW,     			/**< Prepare to stop */
	GREEN,      			/**< Go */
	OFF         			/**< Light turned off */
} LightState;

/** @brief Traffic light configuration and runtime state */
typedef struct {
	LightState state;    	/**< Current state of the light */
	int carCount;        	/**< Number of cars detected */
	int redPin;          	/**< GPIO pin for RED LED */
	int greenPin;        	/**< GPIO pin for GREEN LED */
	uint32_t timerEnd;  	/**< Timer based on car count */
} TrafficLight;

/** @brief Global array of traffic light instances */
extern TrafficLight Light[NUM_LIGHTS];

// Function Prototypes
void map_lights(void);
void lights_update(const TrafficLight *light);
void lights_set_green(int lightNum1, int lightNum2);
uint32_t lights_set_yellow(int lightNum1, int lightNum2);
uint32_t lights_set_red(int lightNum1, int lightNum2);
void lights_set_initial_state(void);
void lights_init(void);

#endif /* LIGHTS_H_ */

