

#include "stm32f446xx.h"
#include "uart.h"
#include "lights.h"
#include "systick.h"

TrafficLight Light[4];			// Instantiate 4 traffic light

// Populate Light and Map Light to Register addresses
void map_lights(void) {
	// fields: state, carCount, redPin, greenPin. TimeEnd field not included. 
	Light[0] = (TrafficLight){GREEN, 0, 10, 4};		// High traffic - start with GREEN
	Light[1] = (TrafficLight){RED, 0, 5, 3};		// Low traffic - start with RED
	Light[2] = (TrafficLight){GREEN, 0, 2, 1};		// High traffic - start with GREEN
	Light[3] = (TrafficLight){RED, 0, 14, 13};		// Low traffic - start with RED
}


// Update the appropriate LED through the GPIO output based on state
void updateLight(int lightNum) {
	switch (Light[lightNum].state) {
		case RED:
			GPIOB->BSRR = (1U << (Light[lightNum].redPin + 16));  	// RED LED ON
			GPIOB->BSRR = (1U << Light[lightNum].greenPin);			// GREEN LED OFF
			break;
		case YELLOW:
			// Turn ON both RED and GREEN to get YELLOW
			GPIOB->BSRR = ((1U << (Light[lightNum].redPin + 16)) | (1U <<(Light[lightNum].greenPin + 16)));
			break;
		case GREEN:
			GPIOB->BSRR = (1U << (Light[lightNum].greenPin + 16));  // GREEN LED ON
			GPIOB->BSRR = (1U << Light[lightNum].redPin);			// RED LED OFF
			break;
		case OFF:
			// Turn off both RED and GREEN to turn Light off
			GPIOB->BSRR = ((1U << Light[lightNum].redPin) | (1U << Light[lightNum].greenPin));
			break;
	}
}

// Release the currently stopped flow of Light pairs
void go(int lightNum1, int lightNum2) {
	// Check if the light pair is RED
	if (Light[lightNum1].state == RED || Light[lightNum2].state == RED) {
		// Transition directly from RED to GREEN
		Light[lightNum1].state = GREEN;
		Light[lightNum2].state = GREEN;
		printf("Light %d turned GREEN\n\r", lightNum1 + 1);
		printf("Light %d turned GREEN\n\r", lightNum2 + 1);
	} else {
		// Light already GREEN - Nothing to do
		printf("Light %d is already GREEN\n\r", lightNum1 + 1);
		printf("Light %d is already GREEN\n\r", lightNum2 + 1);
	}
	// Update the Light states
	updateLight(lightNum1);
	updateLight(lightNum2);
}

uint32_t amber(int lightNum1, int lightNum2) {
	if (Light[lightNum1].state == GREEN || Light[lightNum2].state == GREEN) {
		// Transition from GREEN to YELLOW
		Light[lightNum1].state = YELLOW;
		Light[lightNum2].state = YELLOW;
		printf("Light %d turned YELLOW\n\r", lightNum1 + 1);
		printf("Light %d turned YELLOW\n\r", lightNum2 + 1);
	} else {
		// Light already RED - Nothing to do
		printf("Light %d is already RED\n\r", lightNum1 + 1);
		printf("Light %d is already RED\n\r", lightNum2 + 1);
	}

	// Update the Light states
	updateLight(lightNum1);
	updateLight(lightNum2);

	return 1;
}

uint32_t stop(int lightNum1, int lightNum2) {
	// Check if the Light pair is YELLOW
	if (Light[lightNum1].state == YELLOW || Light[lightNum2].state == YELLOW) {
		// Transition from YELLOW to RED
		Light[lightNum1].state = RED;
		Light[lightNum2].state = RED;
		printf("Light %d turned RED\n\r", lightNum1 + 1);
		printf("Light %d turned RED\n\r", lightNum2 + 1);
	} 

	// Update the Light states
	updateLight(lightNum1);
	updateLight(lightNum2);

	return 1;
}

// Initialize the lights output pins
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

