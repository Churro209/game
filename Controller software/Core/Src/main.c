#include "uart.h"
#include "sysclock.h"
#include <string.h>
#include "controller.h"
#include <stdio.h>

volatile int systick_flag;
int main(void) {
    // Initialize system
    controller_init();
    SysTick_Init();
    // Declare the 'message' buffer here

    while (1) {
    	if (systick_flag) {  // Use the SysTick flag for periodic updates
    	            systick_flag = 0; // Clear the SysTick flag
    	            Update_Button_States(); // Transmit button state at regular intervals
    	}
    }
}
