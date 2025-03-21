/*
 * main.c - Entry point and main flow of the program
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "exti.h"
#include "lights.h"
#include "systick.h"
#include "stm32f446xx.h"

#define THRESHOLD		3
#define DEBOUNCE_TIME  100

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
} TrafficLight;

TrafficLight Light[4];			// Instantiate 4 traffic light

// Map Light to Register addresses
void map_lights(void) {
	Light[0] = (TrafficLight){GREEN, 0, 10, 4};	// High traffic - start with GREEN
	Light[1] = (TrafficLight){RED, 0, 5, 3};		// Low traffic - start with RED
	Light[2] = (TrafficLight){GREEN, 0, 2, 1};
	Light[3] = (TrafficLight){RED, 0, 14, 13};
}

// Button input interrupt Handler
void EXTI15_10_IRQHandler(void) {
	static uint32_t lastPressTime[4] = {0};
	uint32_t currentTime = systickGetMillis();

	if ((EXTI->PR & BUTTON1) != 0) {
		// Check if after 100ms - Prevent debounce that result in consecutive presses
		if (currentTime - lastPressTime[0] >= DEBOUNCE_TIME) {
			lastPressTime[0] = currentTime;  	// Update last press time
			Light[0].carCount++;				// Increment car count
			printf("Light 1 car detected: %d\n\r", Light[0].carCount);
		}
		EXTI->PR |= BUTTON1;		// Clear interrupt (PR) flag
	} else if ((EXTI->PR & BUTTON2) != 0) {
		if (currentTime - lastPressTime[1] >= DEBOUNCE_TIME) {
			lastPressTime[1] = currentTime;
			Light[1].carCount++;
			printf("Light 2 car detected: %d\n\r", Light[1].carCount);
		}
		EXTI->PR |= BUTTON2;
	} else if ((EXTI->PR & BUTTON3) != 0) {
		if (currentTime - lastPressTime[2] >= DEBOUNCE_TIME) {
			lastPressTime[2] = currentTime;
			Light[2].carCount++;
			printf("Light 3 car detected: %d\n\r", Light[2].carCount);
		}
		EXTI->PR |= BUTTON3;
	} else if ((EXTI->PR & BUTTON4) != 0) {
		if (currentTime - lastPressTime[3] >= DEBOUNCE_TIME) {
			lastPressTime[3] = currentTime;
			Light[3].carCount++;
			printf("Light 4 car detected: %d\n\r", Light[3].carCount);
		}
		EXTI->PR |= BUTTON4;
	}
}

// Update the appropriate LED based on state
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

void turnYellow(int lightA, int lightB) {
	Light[lightA].state = YELLOW;
	Light[lightB].state = YELLOW;
	updateLight(lightA);
	updateLight(lightB);
}

void changeLightState(int lightNum) {
    static uint32_t yellowStartTime[4];  // Store timers for each light

    if (Light[lightNum].state == RED) {
        // Transition directly from RED to GREEN
        Light[lightNum].state = GREEN;
        printf("Light %d turned GREEN\n\r", lightNum + 1);
    } else if (Light[lightNum].state == GREEN) {
        // Transition from GREEN to YELLOW
        if (lightNum == 0 || lightNum == 1) {
            turnYellow(lightNum, (lightNum == 0) ? 1 : 0);  // Light 0 with 1, or 1 with 0
        } else if (lightNum == 2 || lightNum == 3) {
            turnYellow(lightNum, (lightNum == 2) ? 3 : 2);  // Light 2 with 3, or 3 with 2
        }
        yellowStartTime[lightNum] = systickGetMillis();  // Store the yellow start time
    } else if (Light[lightNum].state == YELLOW) {
        // Check if 1 second has passed for YELLOW
        if (systickGetMillis() - yellowStartTime[lightNum] >= 1000) {
            Light[lightNum].state = RED;  // After 1 second, transition to RED
            printf("Light %d turned RED\n\r", lightNum + 1);
            updateLight(lightNum);
        }
    }
}

//void changeLightState(int lightNum) {
//
//    if (Light[lightNum].state == RED) {
//        // Transition directly from RED to GREEN
//        Light[lightNum].state = GREEN;
//        printf("Light %d turned GREEN\n\r", lightNum + 1);
//    } else if (Light[lightNum].state == GREEN) {
//    	if (lightNum == 0 || lightNum == 1) {
//			turnYellow(lightNum, (lightNum == 0) ? 1 : 0);  // Light 0 with 1, or 1 with 0
//		} else if (lightNum == 2 || lightNum == 3) {
//			turnYellow(lightNum, (lightNum == 2) ? 3 : 2);  // Light 2 with 3, or 3 with 2
//		}
//    	uint32_t yellowStartTime = systickGetMillis();
//    	if (systickGetMillis() - yellowStartTime >= 1000) {
//    		Light[lightNum].state = RED;  // After 1 second, transition to RED
//    		printf("Light %d turned RED\n\r", lightNum + 1);
//    	}
//    }
//}

//void changeLightState(int lightNum) {
//    static uint32_t yellowStartTime[4];  // Store separate timers for each light
//
//    if (Light[lightNum].state == RED) {
//        Light[lightNum].state = GREEN;
//        printf("Light %d turned GREEN\n\r", lightNum + 1);
//    } else if (Light[lightNum].state == GREEN) {
//        Light[lightNum].state = YELLOW;
//        printf("Light %d turned YELLOW\n\r", lightNum + 1);
//        yellowStartTime[lightNum] = systickGetMillis();  // Store yellow start time for this light
//    } else if (Light[lightNum].state == YELLOW) {
//        if (systickGetMillis() - yellowStartTime[lightNum] >= 1000) {
//            Light[lightNum].state = RED;
//            printf("Light %d turned RED\n\r", lightNum + 1);
//        }
//    }
//}

//void changeLightState(int lightNum) {
//    if (Light[lightNum].state == RED) {
//        Light[lightNum].state = GREEN;
//        printf("Light %d turned GREEN\n\r", lightNum + 1);
//    } else if (Light[lightNum].state == GREEN) {
//    	// Turn YELLOW ON for 1sec
//        Light[lightNum].state = YELLOW;
//        systickDelayMs(1000);
//        printf("Light %d turned YELLOW\n\r", lightNum + 1);
//        Light[lightNum].state = RED;
//        printf("Light %d turned RED\n\r", lightNum + 1);
//    }
//}

void syncAndChangeLights(int lightA, int lightB, int lightC, int lightD) {
    // Lights to be serviced
    changeLightState(lightA);
    changeLightState(lightB);
    // Lights being serviced
    changeLightState(lightC);
    changeLightState(lightD);
}



int main() {
	lights_init();					// Initialize light GPIO registers
	exti_init();					// Initialize the input interrupts
	uart2_init();					// Initialize UART
	systick_init();					// Initialize SysTick
	map_lights();					// Map the lights

	printf("\n\r ** Program Start **\n\r");

	// Set the initial states of the lights
	printf("Set initial light states\n\r");
	for (int i=0; i<4; i++) {
		updateLight(i);
		printf("Light %d is %s\n\r", i + 1, (Light[i].state == GREEN) ? "GREEN" : "RED");
	}

	bool firstCarDetect[4] = {false, false, false, false};
	uint32_t firstCarTime[4] = {0};

	while(1) {
		// Check if any light needs state update
		for (int i=0; i<4; i++) {
			// Condition 1
			bool thresholdReached = (Light[i].carCount >= THRESHOLD);
			// Condition 2:
			bool fiveSecElapsed = (firstCarDetect[i] && (systickGetMillis() - firstCarTime[i] >= 10000));

			if (thresholdReached || fiveSecElapsed) {
				// Wait for the previuosly set timer countdown to finish before you change the lights
				// Synchronize light pairs
				if (i == 0 || i == 2) {			// Light 1 and 3
					syncAndChangeLights(0, 2, 1, 3);
					// Set the timer countdown based on cars detected
				} else if (i == 1 || i == 3) {
					syncAndChangeLights(1, 3, 0, 2);	// Light 2 and 4
				}
				Light[i].carCount = 0;			// Reset car count
				firstCarDetect[i] = false;		// Reset first car detect flag
			}

			// If a car is detected and it's the first car, record the time
			if (Light[i].carCount > 0 && !firstCarDetect[i]) {
				firstCarDetect[i] = true;
				firstCarTime[i] = systickGetMillis();
			}
		}
		// update all lights
		for (int i=0; i<4; i++) {
			updateLight(i);
		}
		//printf("Done cycle.\n\r\n\r");
		systickDelayMs(1000);		// Delay for 2secs
	}
}

// Timer countdown for car detected
//void startCountdown(int lightNum) {
//	int carNums = Light[lightNum].carCount;
//	// 1 car = 2secs, 2 cars = 3secs, More than 3 = 5secs
//	int duration = (carNums >= 3) ? 5000 : (carNums == 2) ? 3000 : 2000;
//
//	Light[lightNum].timerEnd = systickGetMillis() + duration;
//	Light[lig

//int main() {
//	lights_init();					// Initialize light GPIO registers
//	exti_init();					// Initialize the input interrupts
//	uart2_init();					// Initialize UART
//	systick_init();					// Initialize SysTick
//	map_lights();					// Map the lights
//
//	printf("\n\r ** Program Start **\n\r");
//
//	// Set the initial states of the lights
//	printf("Set initial light states\n\r");
//	for (int i=0; i<4; i++) {
//		updateLight(i);
//		printf("Light %d is %s\n\r", i + 1, (Light[i].state == GREEN) ? "GREEN" : "RED");
//	}
//
//	while(1) {
//
//		// Check if any light needs state update
//		for (int i=0; i<4; i++) {
//			bool timerElapsed = (Light[i].timerActive && (systickGetMillis() >= Light[i].timerEnd));
//
//			if (Light[i].carCount > 0) {
//				if (!Light[i].timerActive) {
//					if (Light[i].state == RED) {
//						// Switch immediately if no active countdown
//						if (i == 0 || i == 2) {
//							syncAndChangeLights(0, 2, 1, 3);
//						} else {
//							syncAndChangeLights(1, 3, 0, 2);
//						}
//						Light[i].carCount = 0;
//						startCountdown(i);
//					}
//				} else if (timerElapsed){
//					// Timer expired, check for waiting cars
//					if (i == 0 || i == 2) {
//						syncAndChangeLights(0, 2, 1, 3);
//					} else {
//						syncAndChangeLights(1, 3, 0, 2);
//					}
//					Light[i].carCount = 0;
//					Light[i].timerActive = false;
//				}
//			}
//		}
//		// update all lights
//		for (int i=0; i<4; i++) {
//			updateLight(i);
//		}
//		printf("Done cycle.\n\r\n\r");
//		systickDelayMs(10000);		// Delay for 2secs
//	}
//}


// Check if any light needs state update
//for (int i=0; i<4; i++) {
//	// Condition 1
//	bool thresholdReached = (Light[i].carCount >= THRESHOLD);
//	// Condition 2:
//	bool fiveSecElapsed = (firstCarDetect[i] && (systickGetMillis() - firstCarTime[i] >= 10000));
//
//	if (thresholdReached || fiveSecElapsed) {
//		// Wait for the previuosly set timer countdown to finish before you change the lights
//		// Synchronize light pairs
//		if (i == 0 || i == 2) {			// Light 1 and 3
//			syncAndChangeLights(0, 2, 1, 3);
//			// Set the timer countdown based on cars detected
//		} else if (i == 1 || i == 3) {
//			syncAndChangeLights(1, 3, 0, 2);	// Light 2 and 4
//		}
//		Light[i].carCount = 0;			// Reset car count
//		firstCarDetect[i] = false;		// Reset first car detect flag
//	}
//
//	// If a car is detected and it's the first car, record the time
//	if (Light[i].carCount > 0 && !firstCarDetect[i]) {
//		firstCarDetect[i] = true;
//		firstCarTime[i] = systickGetMillis();
//	}
//}

//
//printf("Red reset bit: %d\n\r", Light[lightNum].redPin + 16);
//printf("Yellow reset bit: %d %d\n\r",(Light[lightNum].redPin + 16), (Light[lightNum].greenPin + 16));
//printf("Green reset bit: %d\n\r", Light[lightNum].greenPin+16);
//printf("Reset bit: %d\n\r", Light[lightNum].redPin+16);

//		// Light 1
//		GPIOB->BSRR = (1U << 26);   // Turn on RED
//		GPIOB->BSRR = (1U << 4);	// Turn off GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 10) | (1U << 4));	  // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = (1U << 20);   // Turn on GREEN
//		GPIOB->BSRR = (1U << 10);	// Turn off RED
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 10) | (1U << 4));    // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 26) | (1U << 20));   // Turn on YELLOW
//		//GPIOB->BSRR = (1U << 22);	// Turn ON GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 10) | (1U << 4));	   // Turn all OFF
//		systickDelayMs(1000);
//
//		//Light 2
//		GPIOB->BSRR = (1U << 21);   // Turn on RED
//		GPIOB->BSRR = (1U << 3);	// Turn off GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 5) | (1U << 3));	  // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = (1U << 19);   // Turn on GREEN
//		GPIOB->BSRR = (1U << 5);	// Turn off RED
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 5) | (1U << 3));    // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 21) | (1U << 19));   // Turn on RED
//		//GPIOB->BSRR = (1U << 28);	// Turn ON GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 5) | (1U << 3));	   // Turn all OFF
//		systickDelayMs(1000);
//
//		// Light 3
//		GPIOB->BSRR = (1U << 18);   // Turn on RED
//		GPIOB->BSRR = (1U << 1);	// Turn off GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 2) | (1U << 1));	  // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = (1U << 17);   // Turn on GREEN
//		GPIOB->BSRR = (1U << 2);	// Turn off RED
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 2) | (1U << 1));    // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 18) | (1U << 17));   // Turn on RED
//		//GPIOB->BSRR = (1U << 17);	// Turn ON GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 2) | (1U << 1));	   // Turn all OFF
//		systickDelayMs(1000);
//
//		// Light 4
//		GPIOB->BSRR = (1U << 30);   // Turn on RED
//		GPIOB->BSRR = (1U << 13);	// Turn off GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 14) | (1U << 13));	  // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = (1U << 29);   // Turn on GREEN
//		GPIOB->BSRR = (1U << 14);	// Turn off RED
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 14) | (1U << 13));    // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 30) | (1U << 29));   // Turn on RED
//		//GPIOB->BSRR = (1U << 29);	// Turn ON GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 14) | (1U << 13));	   // Turn all OFF
//		systickDelayMs(1000);

//void updateLight(int lightNum) {
////	// Turn OFF all LEDs for this traffic light
////	GPIOB->BSRR = (Light[lightNum].redPin |
////                   Light[lightNum].yellowPin |
////                   Light[lightNum].greenPin);
//
//	// Turn ON the appropriate LED based on state
//	switch (Light[lightNum].state) {
//		case RED:
//			GPIOB->BSRR = (1U << (Light[lightNum].redPin + 16));  		// Reset bit turns LED ON
//			break;
//		case YELLOW:
//			GPIOB->BSRR = (Light[lightNum].yellowPin << 16);  	// Reset bit turns LED ON
//			break;
//		case GREEN:
//			GPIOB->BSRR = (Light[lightNum].greenPin << 16);  	// Reset bit turns LED ON
//			break;
//		case OFF:
//			GPIOB->BSRR = (Light[lightNum].yellowPin);			// set bits of YELLOW is Light OFF
//			break;
//	}
//}

//void map_lights(void) {
//	// Traffic Light 1
//	Light[0].redPin = 10;
//	Light[0].yellowPin = ((1U<<10) | (1U<<4));
//	Light[0].greenPin = (1U<<4);
//	Light[0].state = GREEN;
//	Light[0].carCount = 0;
//
//	// Traffic Light 2
//	Light[1].redPin = (1U<<5);
//	Light[1].yellowPin = ((1U<<5) | (1U<<3));
//	Light[1].greenPin = (1U<<3);
//	Light[1].state = RED;						// Low traffic through
//	Light[1].carCount = 0;
//
//	// Traffic Light 3
//	Light[2].redPin = (1U<<2);
//	Light[2].yellowPin = ((1U<<1) | (1U<<2));
//	Light[2].greenPin = (1U<<1);
//	Light[2].state = GREEN;						// High traffic though
//	Light[2].carCount = 0;
//
//	// Traffic Light 4
//	Light[3].redPin = (1U<<14);
//	Light[3].yellowPin = ((1U<<14) | (1U<<13));
//	Light[3].greenPin = (1U<<13);
//	Light[3].state = RED;
//	Light[3].carCount = 0;
//}

//	uint32_t firstCarTime[4] = {0};		// Timestap for first car detected
//	bool firstCarDetect[4] = {false};	// Flag to indicate first car detected
//
//	while(1) {
//		// Check if any light needs state update
//		for (int i=0; i<4; i++) {
//
//			// Condition 1: car count >= threshold
//			bool thresholdReached = (Light[i].carCount >= THRESHOLD);
//			// Condition 2: 5 sec elapsed since first car detect
//			bool fiveSecElapsed = (firstCarDetect[i] && (systickGetMillis() - firstCarTime[i] >= 5000));
//
//			if (thresholdReached || fiveSecElapsed) {
//				// Change the corresponding light
//				if (Light[i].state == RED) {
//					Light[i].state = GREEN;
//				} else if (Light[i].state == YELLOW) {
//					Light[i].state = RED;
//				} else if (Light[i].state == GREEN) {
//					Light[i].state = YELLOW;
//				}
//				Light[i].carCount = 0;			// Reset car count
//				firstCarDetect[i] = false;		// Reset first car detect flag
//			}
//
//			// If a car is detected and it's the first car, record the time
//			if (Light[i].carCount > 0 && !firstCarDetect[i]) {
//				firstCarDetect[i] = true;
//				firstCarTime[i] = systickGetMillis();
//			}
//		}
//		// update all lights
//		for (int i=0; i<4; i++) {
//			updateLight(i);
//		}
//		systickDelayMs(100);		// Delay for 100ms
//	}











//
//if (!(GPIOB->IDR & BUTTON1)) {
//			Light1.carCount++;
//			printf("Light 1 Car detected: %d", Light1.carCount);
//		} else if (!(GPIOB->IDR & BUTTON2)) {
//			Light2.carCount++;
//			printf("Light 2 Car detected: %d", Light2.carCount);
//		} else if (!(GPIOC->IDR & BUTTON3)) {
//			Light3.carCount++;
//			printf("Light 3 Car detected: %d", Light3.carCount);
//		} else if (!(GPIOB->IDR & BUTTON4)) {
//			Light4.carCount++;
//			printf("Light 4 Car detected: %d", Light4.carCount);
//		}



//		if (!(GPIOC->IDR & (1U << 10))) {
//			button1_press++;
//			printf("Button 1 press: %d\n\r", button1_press);
//		} else if (!(GPIOC->IDR & (1U << 11))) {
//			button2_press++;
//			printf("Button 2 press: %d\n\r", button2_press);
//		} else if (!(GPIOC->IDR & (1U << 4))) {
//			button3_press++;
//			printf("Button 3 press: %d\n\r", button3_press);
//		} else if (!(GPIOB->IDR & (1U << 9))) {
//			button4_press++;
//			printf("Button 4 press: %d\n\r", button4_press);
//		}
//		systickDelayMs(1000);

//		// Light 1
//		GPIOC->BSRR = (1U << 24);   // Turn on RED
//		GPIOC->BSRR = (1U << 6);	// Turn off GREEN
//		systickDelayMs(1000);
//		GPIOC->BSRR = ((1U << 8) | (1U << 6));	  // Turn all OFF
//		systickDelayMs(1000);
//		GPIOC->BSRR = (1U << 22);   // Turn on GREEN
//		GPIOC->BSRR = (1U << 8);	// Turn off RED
//		systickDelayMs(1000);
//		GPIOC->BSRR = ((1U << 8) | (1U << 6));    // Turn all OFF
//		systickDelayMs(1000);
//		GPIOC->BSRR = ((1U << 24) | (1U << 22));   // Turn on YELLOW
//		//GPIOC->BSRR = (1U << 22);	// Turn ON GREEN
//		systickDelayMs(1000);
//		GPIOC->BSRR = ((1U << 8) | (1U << 6));	   // Turn all OFF
//		systickDelayMs(1000);
//
//		//Light 2
//		GPIOB->BSRR = (1U << 28);   // Turn on RED
//		GPIOA->BSRR = (1U << 11);	// Turn off GREEN
//		systickDelayMs(1000);
//		GPIOA->BSRR = ((1U << 12) | (1U << 11));	  // Turn all OFF
//		systickDelayMs(1000);
//		GPIOA->BSRR = (1U << 27);   // Turn on GREEN
//		GPIOA->BSRR = (1U << 12);	// Turn off RED
//		systickDelayMs(1000);
//		GPIOA->BSRR = ((1U << 12) | (1U << 11));    // Turn all OFF
//		systickDelayMs(1000);
//		GPIOA->BSRR = ((1U << 27) | (1U << 28));   // Turn on RED
//		//GPIOA->BSRR = (1U << 28);	// Turn ON GREEN
//		systickDelayMs(1000);
//		GPIOA->BSRR = ((1U << 12) | (1U << 11));	   // Turn all OFF
//		systickDelayMs(1000);
//
//		// Light 3
//		GPIOB->BSRR = (1U << 18);   // Turn on RED
//		GPIOB->BSRR = (1U << 1);	// Turn off GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 2) | (1U << 1));	  // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = (1U << 17);   // Turn on GREEN
//		GPIOB->BSRR = (1U << 2);	// Turn off RED
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 2) | (1U << 1));    // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 18) | (1U << 17));   // Turn on RED
//		//GPIOB->BSRR = (1U << 17);	// Turn ON GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 2) | (1U << 1));	   // Turn all OFF
//		systickDelayMs(1000);
//
//		// Light 4
//		GPIOB->BSRR = (1U << 30);   // Turn on RED
//		GPIOB->BSRR = (1U << 13);	// Turn off GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 14) | (1U << 13));	  // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = (1U << 29);   // Turn on GREEN
//		GPIOB->BSRR = (1U << 14);	// Turn off RED
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 14) | (1U << 13));    // Turn all OFF
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 30) | (1U << 29));   // Turn on RED
//		//GPIOB->BSRR = (1U << 29);	// Turn ON GREEN
//		systickDelayMs(1000);
//		GPIOB->BSRR = ((1U << 14) | (1U << 13));	   // Turn all OFF
//		systickDelayMs(1000);

/*** Good Code ****/
//
//	if (!(GPIOC->IDR & (1U << 10))) {
//		GPIOC->BSRR = (1U << 24);   // Turn on RED
//		GPIOC->BSRR = (1U << 6);	// Turn off GREEN
//		systickDelayMs(1000);
//		GPIOC->BSRR = ((1U << 8) | (1U << 6));	  // Turn all OFF
//		systickDelayMs(1000);
//		GPIOC->BSRR = (1U << 22);   // Turn on GREEN
//		GPIOC->BSRR = (1U << 8);	// Turn off RED
//		systickDelayMs(1000);
//		GPIOC->BSRR = ((1U << 8) | (1U << 6));    // Turn all OFF
//		systickDelayMs(1000);
//		GPIOC->BSRR = (1U << 24);   // Turn on RED
//		GPIOC->BSRR = (1U << 22);	// Turn ON GREEN
//		systickDelayMs(1000);
//		GPIOC->BSRR = ((1U << 8) | (1U << 6));	   // Turn all OFF
//		systickDelayMs(1000);
//	} else {
//		GPIOC->BSRR = ((1U << 8) | (1U << 6));	   // Turn all OFF
//	}


//		GPIOC->BSRR = (1U << 6);   // Turn on GREEN
//		GPIOC->BSRR = ((1U << 8) | (1U << 6) | (1U << 5));
//		systickDelayMs(1000);
//
//		GPIOC->BSRR = (1U << 5);   // Turn on BLUE
//		GPIOC->BSRR = ((1U << 8) | (1U << 6) | (1U << 5));
//		systickDelayMs(1000);

//		if (!(GPIOC->IDR & (1U << 10))) {
//			press++;
//			GPIOC->BSRR = (1U << 24);
//			printf("%d press\n\r", press);
//			//systickDelayMs(1000);
//		} else {
//			GPIOC->BSRR = ((1U << 8) | (1U << 6));
//		}

//		if (!(GPIOB->IDR & (1U << 7))) {
//		    GPIOA->BSRR = (1U << 29);
//		    systickDelayMs(1000);
//		    GPIOA->BSRR = (1U << 30);
//		    systickDelayMs(1000);
//		} else {
//		    GPIOA->BSRR = ((1U << 13) | (1U << 14));
//		}

		// Check BUTTON2 (PA6) -> Control Light 2 (PA12 - Red)
//		if (!(GPIOA->IDR & (1U << 6))) {
//		    GPIOA->BSRR = (1U << 28);  // Turn Light 2 RED (PA12)
//		} else {
//		    GPIOA->BSRR = ((1U << 12) | (1U << 11));  // Turn Light 2 OFF
//		    GPIOB->BSRR = (1U << 7);  // Ensure PA7 (Blue) is off
//		}
//
//		// Check BUTTON3 (PC2) -> Control Light 3 (PC13 - Red)
//		if (!(GPIOC->IDR & (1U << 2))) {
//		    GPIOC->BSRR = (1U << 29);  // Turn Light 3 RED (PC13)
//		} else {
//		    GPIOC->BSRR = ((1U << 13) | (1U << 14) | (1U << 15));  // Turn Light 3 OFF
//		}
//
//		// Check BUTTON4 (PB7) -> Control Light 4 (PA13 - Red)
//		if (!(GPIOB->IDR & (1U << 7))) {
//		    GPIOA->BSRR = (1U << 29);  // Turn Light 4 RED (PA13)
//		} else {
//		    GPIOA->BSRR = ((1U << 13) | (1U << 14) | (1U << 15));  // Turn Light 4 OFF
//		}


		// If Button1 or Button3 pressed
//		if ((GPIOB->IDR & BUTTON1) | (GPIOB->IDR & BUTTON10)) {
//			GPIOC->BSRR = ((1U<<24) | (1U<<6) | (1U<<21));	// Light 1 GREEN
//			GPIOB->BSRR = ((1U<<18) | (1U<<0) | (1U<<31));	// Light 3 GREEN
//		} else if ((GPIOA->IDR & BUTTON2) | (GPIOB->IDR & BUTTON4)) {
//			GPIOA->BSRR = ((1U<<28) | (1U<<11) | (1U<<23));	// Light 2 GREEN
//			GPIOB->BSRR = ((1U<<30) | (1U<<13) | (1U<<21));	// Light 4 GREEN
//		}
