#include "stm32l4xx.h"

// Initialize DMA for circular transmission
void DMA_Init_Circular(uint8_t *buffer, uint16_t size) {
    // Enable DMA1 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    DMA1_Channel7->CCR&= ~DMA_CCR_EN;
    DMA1_Channel7->CCR &= ~DMA_CCR_MEM2MEM;

    // Configure DMA1_Channel7 for USART2_TX
    DMA1_Channel7->CPAR = (uint32_t)&USART2->TDR; // Peripheral address (USART2 TDR)
    DMA1_Channel7->CMAR = (uint32_t)buffer;      // Memory address
    DMA1_Channel7->CNDTR = size;                // Number of data items to transfer

    // Configure DMA channel settings
    DMA1_Channel7->CCR |= DMA_CCR_MINC;           // Enable memory increment mode
    DMA1_Channel7->CCR |= DMA_CCR_DIR  ;         // Memory-to-peripheral direction
	DMA1_Channel7->CCR  = DMA_CCR_CIRC ;         // Enable circular mode
	DMA1_Channel7->CCR  |= DMA_CCR_TCIE ;         // Enable transfer complete interrupt
	DMA1_Channel7->CCR    |= DMA_CCR_PL_1;         // Set priority level to high

    // Enable DMA channel
    DMA1_Channel7->CCR |= DMA_CCR_EN;

    // Set priority and enable DMA interrupt in NVIC
    NVIC_SetPriority(DMA1_Channel7_IRQn, 2); // Set priority (lower is higher priority)
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);      // Enable DMA1_Channel7 interrupt
}

// Start circular DMA transmission
void Start_DMA_Transmission_Circular() {
    // Enable DMA channel
    DMA1_Channel7->CCR |= DMA_CCR_EN;

    // Enable DMA requests in USART
    USART2->CR3 |= USART_CR3_DMAT;
}

// Stop DMA transmission
void Stop_DMA_Transmission() {
    // Disable DMA channel
    DMA1_Channel7->CCR &= ~DMA_CCR_EN;

    // Disable DMA requests in USART
    USART2->CR3 &= ~USART_CR3_DMAT;
}

// DMA1 Channel7 Interrupt Handler
void DMA1_Channel7_IRQHandler() {
    if (DMA1->ISR & DMA_ISR_TCIF7) {  // Transfer complete flag
        DMA1->IFCR = DMA_IFCR_CTCIF7; // Clear transfer complete flag
        printf("DMA transfer complete.\n");
    }

    if (DMA1->ISR & DMA_ISR_TEIF7) {  // Transfer error flag
        DMA1->IFCR = DMA_IFCR_CTEIF7; // Clear transfer error flag
        printf("DMA transfer error.\n");
    }
}
