/*
 * exti.c
 *
 *  Created on: Mar 3, 2025
 *      Author: Hajj
 */

#include "exti.h"
#include "stm32f446xx.h"

#define GPIOCEN		(1U<<2)
#define SYSCFGEN	(1U<<14)

// PC10, PC11, PC12, PC13 for external input interrupt
void exti_init(void) {

	__disable_irq();			    // Disable global interrupts

	RCC->AHB1ENR |= GPIOCEN;	    // Enable clock for GPIOC

	GPIOC->MODER &= ~(1U<<20);		// PC10 input mode - BUTTON1
	GPIOC->MODER &= ~(1U<<21);
	GPIOC->PUPDR &= ~(1U<<21);  	// Clear bit 21
	GPIOC->PUPDR |= (1U<<20);   	// Enable pull-up resistor (01)

	GPIOC->MODER &= ~(1U<<22);		// PC11 input mode - BUTTON2
	GPIOC->MODER &= ~(1U<<23);
	GPIOC->PUPDR &= ~(1U<<23);  	// Clear bit 23
	GPIOC->PUPDR |= (1U<<22);   	// Enable pull-up resistor (01)

	GPIOC->MODER &= ~(1U<<24);		// PC12 input mode - BUTTON3
	GPIOC->MODER &= ~(1U<<25);
	GPIOC->PUPDR &= ~(1U<<25);  	// Clear bit 25
	GPIOC->PUPDR |= (1U<<24);   	// Enable pull-up resistor (01)

	GPIOC->MODER &= ~(1U<<26);		// PC13 input mode - BUTTON4
	GPIOC->MODER &= ~(1U<<27);
	GPIOC->PUPDR &= ~(1U<<27);  	// Clear bit 27
	GPIOC->PUPDR |= (1U<<26);   	// Enable pull-up resistor (01)

	RCC->APB2ENR |= SYSCFGEN;		// Enable clock access to SYSCFG

	SYSCFG->EXTICR[2] |= (1U<<9);	// Select PORTC for EXTI10
	SYSCFG->EXTICR[2] |= (1U<<13);	// Select PORTC for EXTI11
	SYSCFG->EXTICR[3] |= (1U<<1);	// Select PORTC for EXTI12
	SYSCFG->EXTICR[3] |= (1U<<5);	// Select PORTC for EXTI13

	EXTI->IMR |= (1u<<10);			// Unmask EXTI10
	EXTI->IMR |= (1u<<11);			// Unmask EXTI11
	EXTI->IMR |= (1u<<12);			// Unmask EXTI12
	EXTI->IMR |= (1u<<13);			// Unmask EXTI13

	EXTI->FTSR |= (1U<<10);			// Select falling edge trigger
	EXTI->FTSR |= (1U<<11);
	EXTI->FTSR |= (1U<<12);
	EXTI->FTSR |= (1U<<13);

	NVIC_EnableIRQ(EXTI15_10_IRQn);	// Enable EXTI 10-15 lines in NVIC

	__enable_irq();			        // Enable the global interrupts
}
