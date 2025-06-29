
#include "uart.h"
#include "queue.h"
#include "controller.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

static uint32_t waitingQueue[MAX_WAITING_PAIR];
static int front = -1, rear = -1;

bool queue_is_empty() {									// Check if queue is empty
	return (front == -1);
}

bool queue_is_full() {									// Check if queue is full
	return ((rear + 1) % MAX_WAITING_PAIR == front);
}

void queue_enqueue(uint32_t lightPair) {						// Add request to the queue
	if (!queue_is_full()) {
		if (front == -1) front = 0;						// Initialize front
		rear = (rear + 1) % MAX_WAITING_PAIR;
		waitingQueue[rear] = lightPair;
	}
}

uint32_t queue_dequeue() {									// Remove request from the queue
    if (queue_is_empty()) return -1;  					// No waiting pairs
    uint32_t lightPair = waitingQueue[front];
    if (front == rear) {  								// Queue empty after removing
        front = rear = -1;
    } else {
        front = (front + 1) % MAX_WAITING_PAIR;
    }
    return lightPair;
}