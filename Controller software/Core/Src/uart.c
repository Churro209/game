#include "uart.h"
#include <stdint.h>
#include <string.h>

#define BUFFER_SIZE 64

// Global variables for transmission
volatile uint8_t tx_buffer[BUFFER_SIZE];
volatile uint32_t tx_length = 0;
volatile uint32_t tx_index = 0;

// Function prototypes
void UART2_GPIO_Init();
void USART_Init(USART_TypeDef *USARTx);

void UART2_Init() {
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN; // Enable USART2 clock

    // Set to system clock
    RCC->CCIPR &= ~(RCC_CCIPR_USART2SEL);
    RCC->CCIPR |= (1 << RCC_CCIPR_USART2SEL_Pos);

    UART2_GPIO_Init(); // Initialize GPIO for UART
    USART_Init(USART2);

    // Enable USART2 interrupt in NVIC
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 1); // Set interrupt priority
}

void UART2_GPIO_Init() {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable clock for GPIOA

    // Configure PA2 (TX) and PA3 (RX) for alternate function mode
    GPIOA->MODER &= ~((3 << (2 * 2)) | (3 << (2 * 3)));
    GPIOA->MODER |= (2 << (2 * 2)) | (2 << (2 * 3));

    GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos) | (7 << GPIO_AFRL_AFSEL3_Pos); // Set AF7 for USART2

    GPIOA->OSPEEDR |= (2 << (2 * 2)) | (2 << (2 * 3)); // High speed for PA2, PA3
    GPIOA->PUPDR |= (1 << (2 * 2)) | (1 << (2 * 3));   // Pull-up for PA2, PA3
}

void USART_Init(USART_TypeDef *USARTx) {
    USARTx->CR1 &= ~USART_CR1_UE; // Disable UART

    USARTx->CR1 &= ~USART_CR1_M;    // Set to 8-bit word length
    USARTx->CR2 &= ~USART_CR2_STOP; // 1 stop bit
    USARTx->CR1 &= ~USART_CR1_PCE;  // No parity

    USARTx->BRR = 0x208D; // Baud rate: 9600 (assuming 80 MHz clock)

    USARTx->CR1 |= USART_CR1_TE;    // Enable transmitter
    USARTx->CR1 |= USART_CR1_RE;    // Enable receiver
    USARTx->CR1 |= USART_CR1_TXEIE; // Enable TXE interrupt

    USARTx->CR1 |= USART_CR1_UE; // Enable UART

    while (!(USARTx->ISR & USART_ISR_TEACK)); // Wait for transmitter ready
    while (!(USARTx->ISR & USART_ISR_REACK)); // Wait for receiver ready
}

void USART_StartTransmission(uint8_t *data, uint32_t length) {
    if (length == 0 || length > BUFFER_SIZE) return;

    // Copy data to transmission buffer
    memcpy((void *)tx_buffer, (void *)data, length);
    tx_length = length;
    tx_index = 0;

    // Enable TXE interrupt to start transmission
    USART2->CR1 |= USART_CR1_TXEIE;
}

void USART2_IRQHandler(void) {
    // Handle TXE interrupt (Transmit Data Register Empty)
    if ((USART2->ISR & USART_ISR_TXE) && (USART2->CR1 & USART_CR1_TXEIE)) {
        if (tx_index < tx_length) {
            USART2->TDR = tx_buffer[tx_index++]; // Load next byte
        } else {
            USART2->CR1 &= ~USART_CR1_TXEIE; // Disable TXE interrupt
            USART2->CR1 |= USART_CR1_TCIE;  // Enable TC interrupt
        }
    }

    // Handle TC interrupt (Transmission Complete)
    if ((USART2->ISR & USART_ISR_TC) && (USART2->CR1 & USART_CR1_TCIE)) {
        USART2->CR1 &= ~USART_CR1_TCIE; // Disable TC interrupt
        tx_length = 0;                 // Reset transmission length
        tx_index = 0;                  // Reset index
    }
}
