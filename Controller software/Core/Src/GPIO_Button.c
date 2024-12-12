/*
 * GPIO_Button.c
 *
 *  Created on: Nov 29, 2024
 *      Author: root
 *
 */

#include "GPIO_Button.h"
#include "stm32l476xx.h"

//Config GPIOB(0,1,2,3) to use buttons as inputs
void GPIO_Init()
{

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN ; //enable GPIOB clk
	GPIOB->MODER &= ~0xFF; //clear PB0-3 Note: also sets as input mode
	GPIOB->PUPDR &= ~(3 << (0 * 2) | 3 << (1 * 2) | 3 << (2 * 2) | 3<<(2*3)); // Clear PB0, PB1, PB2, PB3 in PUPDR
	GPIOB->PUPDR |= ((1<<(0*2)) | (1<<(1*2)) | (1<<(2*2)) | (1<<(3*2))); //GPIOB(0,1,2,3) to Pull up

}
