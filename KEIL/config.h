 /* ===================================================================================================================
	* Project:			PandoraLoader
	* File:					config.h
	* Description:	Main, global, manual and automatic compile-time configuration header
	*	Copyright:		(c) 2017, Pandora CNC Development Team; Consult the LICENSE file for further licensing information.
	* =================================================================================================================== */

#ifndef CONFIG_H
#define CONFIG_H

// ===========================
// Manual configuration below
// ===========================

// Main UART baud rate
#define UART_BAUD 115200

// Disable all FPGA specific functions including the buzzer and the board version (if defined)
//#define FPGA_DISABLE

// Disable the buzzer (if defined)
//#define BUZZER_DISABLE


// ==================================================================================
// Automatic configuration based on build depending defines below and version number
// Do not change.
// ==================================================================================

// Bootloader version for maintaining
#define VERSION "0.0.1"

// Check if a valid target PLL frequency is defined and set the numeric frequency value accordingly
#if defined(__UPLL_192__) && !defined(__UPLL_240__)
	#define PLL_TFREQ 192000
#elif defined(__UPLL_240__) && !defined(__UPLL_192__)
	#define PLL_TFREQ 240000
#else
	#error "Invalid target PLL frequency specified!"
#endif

// Check whether no specific DDR1 memory speed-grade is chosen and default to grade 6
#if !defined(__DDR_5__) && !defined(__DDR_6__) && !defined(__DDR_75__)
	#define __DDR_6__
#endif

// Check if a valid speed grade is defined and set the numeric speed grade accordingly
#if defined(__DDR_5__) && !defined(__DDR_6__) && !defined(__DDR_75__)
	#define DDR_SPGRADE 5
#elif defined(__DDR_6__) && !defined(__DDR_5__) && !defined(__DDR_75__)
	#define DDR_SPGRADE 6
#elif defined(__DDR_75__) && !defined(__DDR_5__) && !defined(__DDR_6__)
	#define DDR_SPGRADE 75
#else
	#error "Invalid DDR speed grade specified!"
#endif

// Validate the main UART baud rate
#if (UART_BAUD < 1200)
	#error "The UART Baudrate is invalid. It must be equal to or higher than 1200!"
#elif (UART_BAUD != 115200)
	#warning "The UART Baudrate should be set to 115200 for best results!"
#endif


#endif
