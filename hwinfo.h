// ===================================================================================================================
// Project:				PandoraLoader
// File:					hwinfo.h
// Description:		Hardware analyzer header
// Copyright:			(c) 2017, Pandora CNC Development Team; Consult the LICENSE file for further licensing information.
// ===================================================================================================================

#ifndef HWINFO_H
#define HWINFO_H

// =========
// Includes
// =========

#include <stdint.h>


// ==================
// Enums and structs
// ==================

enum {
	MEMTYPE_UNKNOWN,
	MEMTYPE_DDR,
	MEMTYPE_DDR2
};

typedef struct {
	uint32_t	TargetPllFreq;
	uint32_t 	ExtClockFreq;
	uint8_t 	MemoryType;
	uint8_t		MemorySpeed;
} HWINFO_T;


// ====================
// Function signatures
// ====================

uint8_t hwInfo(HWINFO_T* hw);


#endif
