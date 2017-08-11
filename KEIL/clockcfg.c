// ===================================================================================================================
// Project:				PandoraLoader
// File:					clockcfg.h
// Description:		Clock configurator header
// Copyright:			(c) 2017, Pandora CNC Development Team; Consult the LICENSE file for further licensing information.
// ===================================================================================================================

// ===============
// Local includes
// ===============
#include "clockcfg.h"
#include "config.h"
#include "hwinfo.h"
#include "errors.h"

#include "../wblib.h"
#include "../w55fa93_reg.h"
#include <stdint.h>

#define CPU_DQSODS 0x1010
#define CPU_CLKSKEW 0x00888800


// ======================================
// Determine the compile-time parameters
// ======================================
// This is not inside the header file, since this is the only file using the values.

#if (PLL_TFREQ == 192000)
	#define DDR2_SDTIME 0x21667525
	#define DDR_SDMR 0x22
	
	#if (DDR_SPGRADE == 5)
		#define DDR_SDTIME 0x94e6425
	#elif (DDR_SPGRADE == 6)
		#define DDR_SDTIME 0x94e7425
	#elif (DDR_SPGRADE == 75)
		#define DDR_SDTIME 0x98e7549
	#else
		#error "Invalid DDR speed grade specified!"
	#endif
#elif (PLL_TFREQ == 240000)
	#define DDR2_SDTIME 0x29f09625
	#define DDR_SDMR 0x32
	
	#if (DDR_SPGRADE == 5)
		#define DDR_SDTIME 0x9928525
	#elif (DDR_SPGRADE == 6)
		#define DDR_SDTIME 0xa149529
	#elif (DDR_SPGRADE == 75)
		#define DDR_SDTIME 0xa129649
	#else
		#error "Invalid DDR speed grade specified!"
	#endif
#else
	#error "Invalid target PLL frequency specified!"
#endif

uint8_t clockInit(HWINFO_T* hw)
{	
	if(!hw)
		return ERR_ARGNULLP;
	
	// Configure DQSODS and CKDQSDS
	outp32(REG_DQSODS, CPU_DQSODS);
	outp32(REG_CKDQSDS, CPU_CLKSKEW);
	
	// Configure SDRAM reference clock SDREF depending on whether we get 12 MHz on the external clock
	outp32(REG_SDREF, ((hw->ExtClockFreq == 12000) ? 0x805A : 0x80C0));
	
	// Analyze memory configuration
	if(hw->MemoryType == MEMTYPE_DDR2) {
		// Configure latency
		outp32(REG_SDMR, 0x432);
		// Setup DQSODS (again; maybe it was reset by the SDMR register write?)
		outp32(REG_DQSODS, CPU_DQSODS);
		// Setup MISCPCR (supposedly electrically releated; driving strength)
		outp32(REG_MISCPCR, 0x1);
		// Setup DDR2 timing
		outp32(REG_SDTIME, DDR2_SDTIME);
	} else if(hw->MemoryType == MEMTYPE_DDR) {
		// Setup DDR timing
		outp32(REG_SDTIME, DDR_SDTIME);
		// Configure latency
		outp32(REG_SDMR, DDR_SDMR);
	} else // Invalid memory type
		return ERR_INTERR;
	
	// Finally configure the system clocks
	sysSetSystemClock(eSYS_UPLL, PLL_TFREQ, PLL_TFREQ, PLL_TFREQ, PLL_TFREQ / 2, PLL_TFREQ / 4);
	
	return ERR_NONE;
}
