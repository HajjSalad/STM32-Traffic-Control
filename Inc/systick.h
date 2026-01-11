/**
 * @file systick.h
  *@brief Public API for SysTick timer configuration and millisecond utilities.
*/

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "stm32f446xx.h"

// Function Prototypes
void SysTick_Handler(void);
void systick_init(void);
uint32_t systickGetMillis(void);
void systickDelayMs(int delay);

#endif /* SYSTICK_H_ */
