/*
 * uart.c
 *
 *  Created on: Feb 20, 2025
 *      Author: Hajj
 */

#include "stm32f446xx.h"
#include "uart.h"

#define GPIOAEN				(1U<<0)
#define UART2EN				(1U<<17)

#define CR1_TE				(1U<<3)
#define CR1_RE				(1U<<2)
#define CR1_UE				(1U<<13)
#define SR_TXE				(1U<<7)

#define SYS_FREQ			16000000
#define APB1_CLK			SYS_FREQ
#define UART_BAUDRATE		115200

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

int __io_putchar(int ch) {
	uart2_write(ch);
	return ch;
}

// PA2 tx, PA3 rx -> UART2
void uart2_init(void) {

	RCC->AHB1ENR |= GPIOAEN;			// Enable clock GPIOA

	GPIOA->MODER &=~(1U<<4);			// PA2 mode to alternate function
	GPIOA->MODER |= (1U<<5);
	GPIOA->AFR[0] |= (7U<<8);			// Set PA2 AF to UART2_TX (AF07)

	GPIOA->MODER &=~(1U<<6);			// PA3 mode to alternate function
	GPIOA->MODER |= (1U<<7);
	GPIOA->AFR[0] |= (7U<<12);			// Set PA3 AF to UART2_RX (AF07)

	RCC->APB1ENR |= UART2EN;			// Enable clock to UART2

	// Configure baudrate USART2 and USART4
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	USART2->CR1 = (CR1_TE | CR1_RE);	// Configure the transfer direction

	USART2->CR1 |= CR1_UE;				// Enable USART Module
}

void uart2_write(int ch) {
	while(!(USART2->SR & SR_TXE)){};	// Make sure the transmit data register is empty.
	USART2->DR = (ch & 0xFF);			// Write to transmit data register
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate) {

	USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate) {

	return ((PeriphClk + (BaudRate / 2U)) / BaudRate);
}
