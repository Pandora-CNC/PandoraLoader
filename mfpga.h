// ===================================================================================================================
// Project:				PandoraLoader
// File:					mfpga.h
// Description:		DDCSV1.1 / RMHV2.1 motion FPGA driver header
// Copyright:			(c) 2017, Pandora CNC Development Team; Consult the LICENSE file for further licensing information.
// ===================================================================================================================

// ======================
// Configuration include
// ======================

#include "config.h"

// Include-guard and check for FPGA support
#if (!defined(MFPGA_H) && !defined(FPGA_DISABLE))
#define MFPGA_H


// =========
// Includes
// =========
#include <stdint.h>

#include "lib/sys/w55fa93_gpio.h"


// ========
// Defines
// ========
#define MFPGA_BUZZER_REG 99

#define MFPGA_BUZZER_ON 1
#define MFPGA_BUZZER_OFF 0

//define 112
//113
//114
#define MFPGA_REG_UART_READ 32


// ====================
// Function signatures
// ====================
void mfpgaInit(void);
void mfpgaCleanup(void);
uint8_t mfpgaWrite(uint8_t addr, uint16_t value);
uint8_t mfpgaRead(uint8_t addr, uint16_t* value);

uint8_t mfpgaBuzzerSet(uint8_t state);
uint8_t mfpgaBuzzerGet(void);
uint8_t mfpgaBuzzerToggle(void);

#ifdef FPGA_UART_MIRROR
uint8_t mfpgaUartInit(void);
uint8_t mfpgaUartCleanup(void);
uint8_t mfpgaUartRead(uint8_t* data);
#endif

#endif
