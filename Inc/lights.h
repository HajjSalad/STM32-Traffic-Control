
#ifndef LIGHTS_H_
#define LIGHTS_H_

#include <stdint.h>
#include <stdbool.h>

#define PIN_LIGHT1_RED		10
#define PIN_LIGHT1_GREEN	4    
#define PIN_LIGHT2_RED      5
#define PIN_LIGHT2_GREEN	3
#define PIN_LIGHT3_RED      2
#define PIN_LIGHT3_GREEN	1
#define PIN_LIGHT4_RED      14
#define PIN_LIGHT4_GREEN	13

#define NUM_LIGHTS			4

// Traffic Light states
typedef enum {
	RED,				// Stop
	YELLOW,				// Prepare to stop
	GREEN,				// Go
	OFF,				// Light OFF
} LightState;

// Traffic light structure
typedef struct {
	LightState state;			// Current state of the light
	int carCount;				// Cars detected at the light
	int redPin;					// GPIO pin for RED light
	int greenPin;				// GPIO pin for GREEN light
	uint32_t timerEnd;			// Timer based on car counts
} TrafficLight;

extern TrafficLight Light[4];

void go(int lightNum1, int lightNum2);
uint32_t amber(int lightNum1, int lightNum2);
uint32_t stop(int lightNum1, int lightNum2);
void lights_set_initial_state(void);
void map_lights(void);
void lights_init(void);

#endif /* LIGHTS_H_ */

