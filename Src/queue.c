/**
 * @file queue.c
 * @brief Circular queue Implementation for managing traffic light requests.
 * 
 * This module provides a simple circular queue to hold traffic light 
 * pair requests. It supports enqueueing new requests and dequeueing 
 * the next request for processing.
 * 
 * @note This queue implementation is not safe for concurrent access
 *       from interrupts or multiple threads. -> Maybe thread or mutex could solve this
*/

#include "uart.h"
#include "queue.h"
#include "controller.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

static uint32_t waitingQueue[MAX_WAITING_PAIR];
static int front = -1, rear = -1;

/** @brief Check if the traffic light request queue is empty. */
bool queue_is_empty() {									
	return (front == -1);
}

/** @brief Check if the traffic light request queue is full. */
bool queue_is_full() {									
	return ((rear + 1) % MAX_WAITING_PAIR == front);
}

/**
 * @brief Add a traffic light pair request to the queue.
 *
 * If the queue is not full, the request is added at the rear of
 * the queue. The queue is circular and wraps around automatically.
 *
 * @param lightPair ID of the traffic light pair to enqueue.
*/
void queue_enqueue(uint32_t lightPair) {					
	if (!queue_is_full()) {
		if (front == -1) front = 0;						// Initialize front
		rear = (rear + 1) % MAX_WAITING_PAIR;
		waitingQueue[rear] = lightPair;
	}
}

/**
 * @brief Remove and return the next traffic light pair request.
 *
 * Retrieves the front element of the queue and updates the queue
 * pointers. Returns -1 if the queue is empty.
 *
 * @return ID of the next traffic light pair, or -1 if the queue is empty.
*/
int32_t queue_dequeue() {								
    if (queue_is_empty()) return -1;  					// No waiting pairs
    uint32_t lightPair = waitingQueue[front];
    if (front == rear) {  								// Queue empty after removing
        front = rear = -1;
    } else {
        front = (front + 1) % MAX_WAITING_PAIR;
    }
    return lightPair;
}