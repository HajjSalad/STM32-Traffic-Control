#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

#define BUTTON1				(1U<<10)
#define BUTTON2				(1U<<11)
#define BUTTON3				(1U<<12)
#define BUTTON4				(1U<<13)

#define BUTTONS			       4

#define THRESHOLD			   3
#define DEBOUNCE_TIME  		  100

#define MAX_WAITING_PAIR	   2

void checkGreenLightTimeout(void);
void SysTick_CheckFirstPressTimeout(void);
void changeLight(uint32_t lightA, uint32_t lightB);

#endif /* CONTROLLER_H_ */