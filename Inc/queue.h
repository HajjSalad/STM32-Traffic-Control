/**
 * @file queue.h
 * @brief Public API for circular queue implementation
*/

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

// Function Prototypes
bool queue_is_empty(void);
bool queue_is_full(void);
void queue_enqueue(uint32_t light_pair);
int32_t queue_dequeue(void);

#endif /* QUEUE_H_ */