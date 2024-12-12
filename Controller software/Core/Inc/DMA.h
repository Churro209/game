/*
 * DMA.h
 *
 *  Created on: Nov 29, 2024
 *      Author: root
 */
#include <stdint.h>

#ifndef INC_DMA_H_
#define INC_DMA_H_

void DMA_Init_Circular(uint8_t *buffer, uint16_t size);
void Start_DMA_Transmission_Circular();
void Stop_DMA_Transmission();
#endif /* INC_DMA_H_ */
