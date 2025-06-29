
#include "uart.h"
#include "systick.h"
#include "controller.h"

#include <stdio.h>
#include <stdint.h>
#include "stm32f446xx.h"

#define SYSTICK_LOAD_VAL		16000
#define CTRL_ENABLE				(1U<<0)
#define CTRL_CLKSRC				(1U<<2)
#define CTRL_COUNTFLAG			(1U<<16)

volatile uint32_t systickMillis = 0;		// Global variable to store milliseconds

void SysTick_Handler(void) {
	systickMillis++;						// Increment milliseconds counter
	checkGreenLightTimeout();				// 
	SysTick_CheckFirstPressTimeout();		// 
	
}

// Configure SysTick
void systick_init(void) {

	SysTick->LOAD = SYSTICK_LOAD_VAL;		// Reload with number of clocks per ms
	SysTick->VAL = 0;						// Clear Systick current value

	// Enable, set clock source, and enable interrupt
	SysTick->CTRL = CTRL_ENABLE | CTRL_CLKSRC | (1U << 1);
}

uint32_t systickGetMillis(void) {
	return systickMillis;					// Return the current milliseconds count
}

void systickDelayMs(int delay) {
	uint32_t start = systickGetMillis();
	while (systickGetMillis() - start < delay) {}	// Busy-wait for the specified delay
}


//
//void systickDelayMs(int delay) {
//
//	/*****  Configure SysTick  *****/
//	// Reload with number of clocks per ms
//	SysTick->LOAD = SYSTICK_LOAD_VAL;
//
//	// Clear Systick current value register
//	SysTick->VAL = 0;
//
//	// Enable Systick and Set the chosen clock source
//	SysTick->CTRL = CTRL_ENABLE | CTRL_CLKSRC;			// Set ENABLE=1 and CLKSRC=1
//
//	for (int i=0; i<delay; i++) {
//
//		// Wait until COUNTFLAG is set
//		while((SysTick->CTRL & CTRL_COUNTFLAG) == 0) {}
//	}
//
//	// Disable the Systick control
//	SysTick->CTRL = 0;
//
//}
