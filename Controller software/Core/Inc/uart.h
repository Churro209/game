/*
 * uart.h
 *
 *  Created on: Nov 20, 2024
 *      Author: root
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "stm32l476xx.h"
void UART2_Init();
void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer,uint32_t nBytes);
void demo_of_UART_print(int a);
void USART_StartTransmission(uint8_t *data, uint32_t length);
#endif /* INC_UART_H_ */
