#ifndef HWINFO_H
#define HWINFO_H

#include <stdint.h>

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

uint8_t hwInfo(HWINFO_T* hw);

#endif
