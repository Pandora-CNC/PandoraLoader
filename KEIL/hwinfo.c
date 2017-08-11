// ===================================================================================================================
// Project:				PandoraLoader
// File:					hwinfo.c
// Description:		Hardware analyzer
// Copyright:			(c) 2017, Pandora CNC Development Team; Consult the LICENSE file for further licensing information.
// ===================================================================================================================

// ===============
// Local includes
// ===============

#include "hwinfo.h"
#include "config.h"
#include "errors.h"

#include <stdint.h>
#include "../wblib.h"
#include "../w55fa93_reg.h"


// ====================================
// The main hardware analysis function
// ====================================
// Requires a non-null pointer to a struct to be filled with the results.
// Will return an error-code.

uint8_t hwInfo(HWINFO_T* hw)
{
	if(!hw)
		return ERR_ARGNULLP;
	
	// Get the target PLL frequency in kHz
	hw->TargetPllFreq = PLL_TFREQ;
	
	// Get the external clock frequency in kHz
	hw->ExtClockFreq = sysGetExternalClock();
	
	// Determine the memory technology used
	switch(inp32(REG_CHIPCFG) & SDRAMSEL)
	{
		// DDR
		case 0x30:
			hw->MemoryType = MEMTYPE_DDR;
		break;
		
		// DDR2
		case 0x20:
			hw->MemoryType = MEMTYPE_DDR2;
		break;
		
		// Unknown
		default:
			hw->MemoryType = MEMTYPE_UNKNOWN;
	}
	
	// Get the memory speed grade
	hw->MemorySpeed = DDR_SPGRADE;
	
	return ERR_NONE;
}
