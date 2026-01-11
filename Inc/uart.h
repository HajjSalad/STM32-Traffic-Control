/**
 * @file uart.h
 * @brief Public API for UART2 peripheral.
*/

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "stm32f446xx.h"

/** @brief Format for printf */
#define LOG(fmt, ...)  printf( fmt "\n\r", ##__VA_ARGS__)

// Function Prototypes
void uart2_init(void);
void uart2_write(int ch);

#endif /* UART_H_ */
