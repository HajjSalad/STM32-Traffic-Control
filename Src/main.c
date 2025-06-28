
#include "uart.h"
#include "exti.h"
#include "main.h"
#include "queue.h"
#include "lights.h"
#include "systick.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

bool timerActive = false;           // Flag to track if commonTimer is running
uint32_t timerStartTime = 0;        // Start time of active timer
uint32_t yellowStartTime = 0;
uint32_t allocatedTime = 0;         // Time allocated for green light
uint32_t activeLightPair = -1;		// Track which light pair has the timer

bool waitingForProcess = false;		// 
bool waitForTimer = false;			// Indicate another light Pair is waiting for the timer
uint32_t waitingLightPair = -1;  	// Store the next subsequent pairs waiting to run
bool firstPress = false;
uint32_t firstPressTime = 0;
uint32_t firstPair = -1;			// Store the pair that was pressed first
uint32_t secondPair = -1;			// Store the pair that was pressed second

int main() {
	lights_init();					// Initialize light GPIO registers
	exti_init();					// Initialize the input interrupts
	uart2_init();					// Initialize UART
	systick_init();					// Initialize SysTick
	map_lights();					// Map the lights

	printf("\n\r** Program Start **\n\r");

	// Set the initial states of the lights
	printf("Set initial light states\n\r");
	for (int i=0; i<4; i++) {
		updateLight(i);
		printf("Light %d is %s\n\r", i + 1, (Light[i].state == GREEN) ? "GREEN" : "RED");
	}

	while(1) {
		__WFI();  // Enter low-power mode until EXTI triggers
	}
}
