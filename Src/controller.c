
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

#include "uart.h"
#include "queue.h"
#include "lights.h"
#include "systick.h"
#include "controller.h"

// Buttons to simulation sensor for car detection
const uint32_t BUTTON[BUTTONS] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4};

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

// Monitor the GREEN light duration for the active light pair and transition when time expires
// Function periodically invoked by SysTick_Handler to determine if allocated time has elapsed
void checkGreenLightTimeout() {
	uint32_t currentTime = systickGetMillis();

	// Check if time allocated elapse
	if (timerActive && (currentTime - timerStartTime >= allocatedTime * 1000)) {
		LOG("Allocated time finished - Timer released\r\n");
		timerActive = false;
		activeLightPair = -1;

		// Check for waiting pairs in the queue
		uint32_t processPair = queue_dequeue();
		if (processPair != -1) {
			LOG("Processing waiting light pair %ld-%ld", processPair+1, processPair+3);
			changeLight(processPair, processPair+2);
		}
	}

	if (waitForTimer && (systickGetMillis() - yellowStartTime >= 1000)) {
		if (waitingLightPair == 0) {
			lights_set_red(1, 3);
			lights_set_green(0, 2);
		} else if (waitingLightPair == 1) {
			lights_set_red(0, 2);
			lights_set_green(1, 3);
		}

		waitForTimer = false;
		waitingLightPair = -1;
	}

}

// Handle the command to stop and release the flow of traffic for light change
void changeLight(uint32_t lightA, uint32_t lightB) {
	activeLightPair = lightA;	// Register the active light pair
	uint32_t currentTime = systickGetMillis();

	// Check which Light in the pair has higher carCount
	int carNums = (Light[lightA].carCount > Light[lightB].carCount) ? Light[lightA].carCount : Light[lightB].carCount;
	
	// Allocate time based on car count
	// 1 car = 2secs, 2 cars = 3secs, More than 3 = 5secs
	allocatedTime = (carNums >= 3) ? 5 : (carNums == 2) ? 3 : 2;
	LOG("Light %ld-%ld allocated timer: %ld", lightA+1, lightA+3, allocatedTime*1000);

	// Start timer for the GREEN light duration - timer handled by checkGreenLightTimeout()
	timerStartTime = systickGetMillis();
	timerActive = true;

	// Stop traffic for the current light pair and release for the next light pair
    if (lightA == 0 || lightA == 2) {
        if (lights_set_yellow(1, 3)) {				// Have to stop the current flow before releasing the next
			yellowStartTime = systickGetMillis();
			waitingLightPair = 0;
			waitForTimer = true;
			// LOG("Before delay\r\n");
			// systickDelayMs(1000);
			// LOG("After delay\r\n");
			// if (stop(1, 3)) {
			// 	LOG("Changed to YELLOW then RED\r\n");
			// 	go(0, 2);					// If stop succesful, release the next flow
			// }
		} else {
			LOG("Could not stop light 2-4.");
		}
    } else if (lightA == 1 || lightA == 3) {
        if (lights_set_yellow(0, 2)) {				// Check stop first
			yellowStartTime = systickGetMillis();
			waitingLightPair = 1;
			waitForTimer = true;
			// systickDelayMs(1000);
			// if (stop(0, 2)) {
			// 	go(1, 3);					// Release on successful stop
			// }
		} else {
			LOG("Could not stop light1-3");
		}
    }
	// Reset car counts
    Light[lightA].carCount = 0;			
    Light[lightB].carCount = 0;			
}

// Station 2
// Allow 3 seconds for user button input - Prevent processing after first press
// Function periodically invoked by SysTick_Handler to determine if 3secs window elapsed
void SysTick_CheckFirstPressTimeout(void) {
	uint32_t currentTime = systickGetMillis();

	/* Button press = Car detected
	** When more than 1 button pressed at once (ie cars detected at more than 1 Light),
	** queue the request such that the first button press is processed first.
	*/
	if (firstPress && (currentTime - firstPressTime >= 3000)) {
		
		// Determine which pairs need to be queued first
		if (firstPair == 0 || firstPair == 2) {				// If Light 1 or 3
			queue_enqueue(0);										// Queue Light pair 1-3
			LOG("Light 1-3 queued.");
			if (secondPair != -1) {							// Check if second pair requested
				queue_enqueue(1);									// Queue Light pair 2-4
				LOG("Light 2-4 queued.");
			}
		} else if (firstPair == 1 || firstPair == 3) {		// If Light 2 or 4
			queue_enqueue(1);
			LOG("Light 2-4 queued.");
			if (secondPair != -1) {							// Check if second pair requested
				queue_enqueue(0);
				LOG("Light 1-3 queued.");
			}
		}

		// Process the first request in the queue
		uint32_t processPair = queue_dequeue();
		if (processPair != -1) {
			LOG("Processing Light %ld-%ld.", processPair+1, processPair+3);
			changeLight(processPair, processPair+2);
		} else {
			LOG("Nothing to process.");
		}

		// Reset after processing
		firstPress = false;		
		firstPair = -1;				
		secondPair = -1;
	}
}

// Station 1
// Button input for car detection - Handle button press
void EXTI15_10_IRQHandler(void) {
	static uint32_t lastPressTime[BUTTONS] = {0};
	uint32_t currentTime = systickGetMillis();

	for (int i=0; i<BUTTONS; i++) {
		if ((EXTI->PR & BUTTON[i]) != 0) {
			// Check if after 100ms - Prevent debounce that result in consecutive presses
			if (currentTime - lastPressTime[i] >= DEBOUNCE_TIME) {
				lastPressTime[i] = currentTime;  	// Update last press time
				Light[i].carCount++;				// Increment car count
				LOG("Light %d car detected: %d", i+1, Light[i].carCount);

				// Record details of the first press - Use it to create 3secs delay to allow for user button input
				if (!firstPress) {					// If this is the first press this round
					firstPressTime = currentTime;	// Record the time of the first press
					firstPress = true;				// Place us in the waiting period
					firstPair = i;					// Record the first button press
				} else {
					if ((firstPair == 0 || firstPair == 2) && (i == 1 || i == 3)) {
						secondPair = i;
					}
					else if ((firstPair == 1 || firstPair == 3) && (i == 0 || i == 2)) {
						secondPair = i;
					}
				}
			}
			EXTI->PR |= BUTTON[i];		// Clear interrupt (PR) flag
		}
	}
}