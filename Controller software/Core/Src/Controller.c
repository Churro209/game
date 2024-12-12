#include "stm32l4xx.h"
#include "uart.h"
#include "sysclock.h"
#include "GPIO_Button.h"
#include "DMA.h"
// Global variables for button state
volatile uint8_t button_state_bitmask = 0xFF; // Current button state


void controller_init()
{
    System_Clock_Init();                        // Initialize system clock
    UART2_Init();                               // Initialize UART
    GPIO_Init();                                // Initialize GPIO for buttons
    DMA_Init_Circular(&button_state_bitmask, 1); // Initialize DMA with button state bitmask
}

void Update_Button_States()
{
    uint8_t new_state = 0;
    new_state |= ((GPIOB->IDR & (1 << 0)) ? 1 : 0) << 0; // PB0
    new_state |= ((GPIOB->IDR & (1 << 1)) ? 1 : 0) << 1; // PB1
    new_state |= ((GPIOB->IDR & (1 << 2)) ? 1 : 0) << 2; // PB2
    new_state |= ((GPIOB->IDR & (1 << 3)) ? 1 : 0) << 3; // PB3

    button_state_bitmask = new_state;

    USART_StartTransmission(&button_state_bitmask, 1); // Send via UART interrupt-drive
}


void SysTick_Init(void)
{
    // Assuming the core clock is 80 MHz (SystemCoreClock)
    SysTick->LOAD = (uint32_t)((SystemCoreClock / 1000) - 1UL); // 1ms tick
    SysTick->VAL = 0UL;  // Clear the current value
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | // Use processor clock
                    SysTick_CTRL_TICKINT_Msk   | // Enable interrupt
                    SysTick_CTRL_ENABLE_Msk;     // Enable SysTick
}

