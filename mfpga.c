// ===================================================================================================================
// Project:				PandoraLoader
// File:					mfpga.c
// Description:		DDCSV1.1 / RMHV2.1 motion FPGA driver
// Copyright:			(c) 2017, Pandora CNC Development Team; Consult the LICENSE file for further licensing information.
// ===================================================================================================================


// ======================
// Configuration include
// ======================

#include "config.h"


// Only compile if FPGA support is not actively disabled
#ifndef FPGA_DISABLE

#include <stdint.h>

#include "lib/sys/w55fa93_reg.h"

#include "mfpga.h"
#include "errors.h"


/************************************************************************
<Info>
Looks like a standard 16bit address/data multiplexed scheme mapped on GPIO
<Data lines>
16 bit bus Xn = GPIOXn
MSB                                           LSB
[B6|B5|B4|B3|B2|E7|E6|E5|E4|E3|E2|A7|A6|A5|A4|A3]
<Control lines>
GPIOB_14 = A0 (0 for address write, 1 for data access)
GPIOB_13 = RD (Active high)
GPIOA_0 = CS (Active high)
GPIOA_1 = WR (Active high)
************************************************************************/

/* Control pin location */
#define A0	(14u)
#define RD	(13u)
#define CS	(0u)
#define WR	(1u)

/* Data nibbles masks */
#define DATA_MASK_GPIOA	(0xF8u)
#define DATA_MASK_GPIOE	(0xFCu)
#define DATA_MASK_GPIOB	(0x7Cu)

/* As-register macro */
#define REG(addr) (*(volatile unsigned long *)(addr))

/* Macros for control signals */
#define A0_HIGH() 	REG(REG_GPIOB_DOUT) |= (1u << A0);
#define A0_LOW() 		REG(REG_GPIOB_DOUT) &= ~(1u << A0);
#define RD_HIGH() 	REG(REG_GPIOB_DOUT) |= (1u << RD);
#define RD_LOW() 		REG(REG_GPIOB_DOUT) &= ~(1u << RD);
#define CS_HIGH() 	REG(REG_GPIOA_DOUT) |= (1u << CS);
#define CS_LOW() 		REG(REG_GPIOA_DOUT) &= ~(1u << CS);
#define WR_HIGH() 	REG(REG_GPIOA_DOUT) |= (1u << WR);
#define WR_LOW() 		REG(REG_GPIOA_DOUT) &= ~(1u << WR);

/* Address / data macro */
#define ACC_ADDR	(0u)
#define ACC_DATA	(1u)

/* Data access macro */
#define OUT_DATA(dat) do{ REG(REG_GPIOA_DOUT) = (REG(REG_GPIOA_DOUT) & ~DATA_MASK_GPIOA) | ((dat & 0x1Fu) << 3u); \
						REG(REG_GPIOE_DOUT) = (REG(REG_GPIOE_DOUT) & ~DATA_MASK_GPIOE) | ((dat & 0x7E0u) >> 3u); \
						REG(REG_GPIOB_DOUT) = (REG(REG_GPIOB_DOUT) & ~DATA_MASK_GPIOB) | ((dat & 0xF800u) >> 9u); } while(0u)

#define IN_DATA() (((REG(REG_GPIOA_PIN) & DATA_MASK_GPIOA) >> 3u) \
					| ((REG(REG_GPIOE_PIN) & DATA_MASK_GPIOE) << 3u) \
					| ((REG(REG_GPIOB_PIN) & DATA_MASK_GPIOB) << 9u))					

/* Port direction macros */
#define DATA_DIR_OUT() do { REG(REG_GPIOA_OMD) |= DATA_MASK_GPIOA; \
						REG(REG_GPIOE_OMD) |= DATA_MASK_GPIOE; \
						REG(REG_GPIOB_OMD) |= DATA_MASK_GPIOB; } while(0u)

#define DATA_DIR_IN() do { REG(REG_GPIOA_OMD) &= ~DATA_MASK_GPIOA; \
						REG(REG_GPIOE_OMD) &= ~DATA_MASK_GPIOE; \
						REG(REG_GPIOB_OMD) &= ~DATA_MASK_GPIOB; } while(0u)

uint8_t mfpgaUartRawWrite(uint16_t f, uint16_t v);
uint8_t mfpgaUartRawRead(uint16_t f, uint16_t* v);

uint8_t mfpgaInitialized = 0, mfpgaUartInitialized = 0, mfpgaBuzzerState = MFPGA_BUZZER_OFF;

/* Init all the GPIO registers */
void mfpgaInit(void)
{
	if(mfpgaInitialized)
		return;
	
	/* Pin modes setup (GPIO mode) */
	REG(REG_GPAFUN) &= ~(0xFFCFu); /* Outputs only on CS, WR and DATA */
	REG(REG_GPBFUN) &= ~(0x3C003FF0); /* Outputs only on A0, RD and DATA */
	REG(REG_GPDFUN) &= ~(0xFF00003F); /* GPIOD usage unknown at the moment */
	REG(REG_GPEFUN) &= ~(0xFFF0u); /* Only data pins */
	
	/* Clear CS and WR */
	REG(REG_GPIOA_DOUT) &= ~((1u << CS) | (1u << WR));
	
	/* Clear RD */
	REG(REG_GPIOB_DOUT) &= ~(1u << RD);
	
	/* Pull - ups */
	REG(REG_GPIOA_PUEN) |= DATA_MASK_GPIOA;
	REG(REG_GPIOB_PUEN) |= DATA_MASK_GPIOB;
	REG(REG_GPIOE_PUEN) |= DATA_MASK_GPIOE;

	/* Output mode */
	REG(REG_GPIOA_OMD) |= (DATA_MASK_GPIOA | (1u << CS) | (1u << WR)); 
	REG(REG_GPIOB_OMD) |= (DATA_MASK_GPIOB | (1u << A0) |  (1u << RD));
	REG(REG_GPIOE_OMD) |= DATA_MASK_GPIOE;
	
	/* Don't know exactly what for TODO maybe SPI0??*/
	REG(REG_GPIOD_PUEN) |= (1u << 15u); /* Pull up */
	REG(REG_GPIOD_OMD) &= ~(1u << 15u); /* Input */
	REG(REG_GPIOD_OMD) |= (1u << 14u) | (1u << 13u) | (1u << 12u) | (1u << 2u) | (1u << 1u) | (1u << 0u);
	
	mfpgaInitialized = 1;
}

void mfpgaCleanup(void)
{
	if(!mfpgaInitialized)
		return;
	
	// TODO: Add deinit code that puts the GPIO into the default state again
	
	mfpgaInitialized = 0;
}

/* Write data */
uint8_t mfpgaWrite(uint8_t addr, uint16_t data)
{
	if(!mfpgaInitialized)
		return ERR_NOINIT;
	
	A0_LOW(); /* Address access */
	OUT_DATA(addr); /* Put address */
	CS_HIGH(); /* Activate chip select */
	WR_HIGH(); /* Perform a write cycle */
	WR_LOW();
	A0_HIGH(); /* Data access */
	OUT_DATA(data);	/* Put data */ 
	WR_HIGH(); /* Perform a write cycle */
	WR_LOW();
	CS_LOW(); /* Deactivate the chip */
	
	return ERR_NONE;
}

/* Read data */
uint8_t mfpgaRead(uint8_t addr, uint16_t* data)
{
	if(!mfpgaInitialized)
		return ERR_NOINIT;
	
	A0_LOW(); /* Address access */
	OUT_DATA(addr); /* Put address */
	CS_HIGH(); /* Activate chip select */
	WR_HIGH(); /* Perform a write cycle */
	WR_LOW();
	DATA_DIR_IN(); /* Configure Pins as inputs, Pull-ups should be enabled at init */	
	A0_HIGH(); /* Data access */
	RD_HIGH(); /* Read access */	
	*data = IN_DATA(); /* Read data */	
	RD_LOW(); /* Disable read access */	
	CS_LOW(); /* Deactivate chip select */
	DATA_DIR_OUT(); /* Configure Pins as outputs */
	
	return ERR_NONE;
}

#ifndef FPGA_UART_DISABLE
uint8_t mfpgaBuzzerSet(uint8_t state)
{
	if(!mfpgaInitialized)
		return ERR_NOINIT;
	
	mfpgaBuzzerState = (state ? MFPGA_BUZZER_ON : MFPGA_BUZZER_OFF);
	mfpgaWrite(MFPGA_BUZZER_REG, (mfpgaBuzzerState ? 0 : 1));
	return ERR_NONE;
}

uint8_t mfpgaBuzzerGet(void)
{
	 return mfpgaBuzzerState;
}

uint8_t mfpgaBuzzerToggle(void)
{
	return mfpgaBuzzerSet(mfpgaBuzzerState ? MFPGA_BUZZER_OFF : MFPGA_BUZZER_ON);
}
#endif

#ifdef FPGA_UART_MIRROR
uint8_t mfpgaUartRawWrite(uint16_t f, uint16_t v)
{
	if(!mfpgaInitialized)
		return ERR_NOINIT;
	
	mfpgaWrite(112, v);
	mfpgaWrite(113, f);
	mfpgaWrite(114, 3);
	mfpgaWrite(114, 0);
	
	return ERR_NONE;
}

uint8_t mfpgaUartRawRead(uint16_t f, uint16_t* v)
{
	if(!mfpgaInitialized)
		return ERR_NOINIT;
	
	mfpgaWrite(113, f);
	mfpgaWrite(114, 5);
	mfpgaRead(32, v);
	mfpgaWrite(114, 0);
	
	return ERR_NONE;
}

uint8_t mfpgaUartInit(void)
{
	if(!mfpgaInitialized)
		return ERR_NOINIT;
	if(mfpgaUartInitialized)
		return ERR_NONE;
	
	mfpgaUartRawWrite(2, 0x0b);
  mfpgaUartRawWrite(3, 0x83);
  mfpgaUartRawWrite(5, 0x130);
	
	mfpgaUartInitialized = 1;
	
	return ERR_NONE;
}

uint8_t mfpgaUartRead(uint8_t* data)
{
	uint16_t avail = 0, buffer = 0;
	
	if(!mfpgaInitialized)
		return ERR_NOINIT;
	if(!mfpgaUartInitialized)
		return ERR_NOINIT;
	
	mfpgaUartRawRead(5, &avail);
	if(!(avail & 1))
		return ERR_NODATA;
	
	mfpgaUartRawRead(0, &buffer);
	*data = buffer;
	return ERR_NONE;
}

uint8_t mfpgaUartWrite(uint8_t data)
{
	if(!mfpgaInitialized)
		return ERR_NOINIT;
	if(!mfpgaUartInitialized)
		return ERR_NOINIT;
	
	mfpgaUartRawWrite(0, data);
	
	return ERR_NONE;
}
#endif

#endif
