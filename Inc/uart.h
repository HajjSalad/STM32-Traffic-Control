/*
 * uart.h
 *
 *  Created on: Feb 22, 2025
 *      Author: Hajj
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "stm32f446xx.h"

#define LOG(fmt, ...)  printf( fmt "\n\r", ##__VA_ARGS__)

void uart2_init(void);
void uart2_write(int ch);

#endif /* UART_H_ */
