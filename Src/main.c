
#include "uart.h"
#include "exti.h"
#include "queue.h"
#include "lights.h"
#include "systick.h"

#include <stdio.h>
#include <stdint.h>
#include "stm32f446xx.h"

static void system_init(void) {
	lights_init();					// Initialize light GPIO registers
	exti_init();					// Initialize the input interrupts
	uart2_init();					// Initialize UART
	systick_init();					// Initialize SysTick
	map_lights();					// Map the lights
}

int main() {
	system_init();
	LOG("\n\r** Program Start **");

	LOG("Set initial light states");
	lights_set_initial_state();
	
	while(1) {
		__WFI();  // Wait for interrupt (low power)
	}
}
