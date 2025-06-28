
#include <stdint.h>
#include <stdbool.h>


#ifndef LIGHTS_H_
#define LIGHTS_H_

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

void lights_init(void);

#endif /* LIGHTS_H_ */

