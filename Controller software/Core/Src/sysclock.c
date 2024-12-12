#include "stm32l476xx.h"



#include "stm32l476xx.h"
#include <stdio.h>

void System_Clock_Init(void) {
    // Enable HSI (16 MHz internal oscillator)
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)); // Wait until HSI is ready

    // Configure PLL
    RCC->PLLCFGR = 0;                          // Reset PLL configuration
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;    // Use HSI as PLL source
    RCC->PLLCFGR |= (1 << RCC_PLLCFGR_PLLM_Pos); // PLLM = 2
    RCC->PLLCFGR |= (20 << RCC_PLLCFGR_PLLN_Pos); // PLLN = 20
    RCC->PLLCFGR |= (0 << RCC_PLLCFGR_PLLR_Pos); // PLLR = 2
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;          // Enable PLLR output

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)); // Wait for PLL to lock

    // Set flash latency for 80 MHz
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_4WS; // 4 wait states

    // Select PLL as the system clock
    RCC->CFGR &= ~RCC_CFGR_SW;              // Clear system clock switch bits
    RCC->CFGR |= RCC_CFGR_SW_PLL;           // Set PLL as system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Wait until PLL is used

    // Update SystemCoreClock
    SystemCoreClockUpdate();

}

