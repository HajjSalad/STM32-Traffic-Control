/*
 * main.h
 *
 *  Created on: Mar 28, 2025
 *      Author: abdirahmanhajj
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdbool.h>

#define BUTTON1				(1U<<10)
#define BUTTON2				(1U<<11)
#define BUTTON3				(1U<<12)
#define BUTTON4				(1U<<13)
#define BUTTONS			       4

#define THRESHOLD			   3
#define DEBOUNCE_TIME  		  100

#define MAX_WAITING_PAIR	   2

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

void SysTick_Runner(void);
void commonTimer(void);

#endif /* MAIN_H_ */
