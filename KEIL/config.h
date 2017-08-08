#ifndef CONFIG_H
#define CONFIG_H

// Manual configuration below

#define VERSION "0.0.1"

#define UART_BAUD 115200

// Automatic configuration based on build depending defines below
// Do not change.

#if defined(__UPLL_192__) && !defined(__UPLL_240__)
	#define PLL_TFREQ 192000
#elif defined(__UPLL_240__) && !defined(__UPLL_192__)
	#define PLL_TFREQ 240000
#else
	#error "Invalid target PLL frequency specified!"
#endif

#if !defined(__DDR_5__) && !defined(__DDR_6__) && !defined(__DDR_75__)
	#define __DDR_6__
#endif

#if defined(__DDR_5__) && !defined(__DDR_6__) && !defined(__DDR_75__)
	#define DDR_SPGRADE 5
#elif defined(__DDR_6__) && !defined(__DDR_5__) && !defined(__DDR_75__)
	#define DDR_SPGRADE 6
#elif defined(__DDR_75__) && !defined(__DDR_5__) && !defined(__DDR_6__)
	#define DDR_SPGRADE 75
#else
	#error "Invalid DDR speed grade specified!"
#endif

#if (UART_BAUD < 1200)
	#error "The UART Baudrate is invalid. It must be equal to or higher than 1200!"
#elif (UART_BAUD != 115200)
	#warning "The UART Baudrate should be set to 115200 for best results!"
#endif

#endif
