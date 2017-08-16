#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef struct {
	uint32_t StartBlock;
	uint32_t EndBlock;
	uint32_t Size;
	uint32_t ExecAddr;
} IMAGE_INFO;

#endif
