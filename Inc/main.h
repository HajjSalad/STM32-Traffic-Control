/*
 * main.h
 *
 *  Created on: Mar 28, 2025
 *      Author: abdirahmanhajj
 */

#ifndef MAIN_H_
#define MAIN_H_



#define BUTTON1				(1U<<10)
#define BUTTON2				(1U<<11)
#define BUTTON3				(1U<<12)
#define BUTTON4				(1U<<13)
#define BUTTONS			       4

#define THRESHOLD			   3
#define DEBOUNCE_TIME  		  100

#define MAX_WAITING_PAIR	   2



void checkGreenLightTimeout(void);
void SysTick_CheckFirstPressTimeout(void);

#endif /* MAIN_H_ */
