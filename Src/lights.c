/*
 * lights.c
 *
 *  Created on: Feb 28, 2025
 *      Author: Hajj
 */

#include "stm32f446xx.h"
#include "uart.h"
#include "lights.h"
#include "systick.h"

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

