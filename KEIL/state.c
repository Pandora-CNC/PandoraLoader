#include "state.h"

#include <stdlib.h>
#include <string.h>

SYSTEM_INFO* sysInfo;

void stateInit(void)
{
	if(!sysInfo)
		sysInfo = (SYSTEM_INFO*)malloc(sizeof(SYSTEM_INFO));
	
	memset(&sysInfo, 0, sizeof(SYSTEM_INFO));
}

void stateDestroy(void)
{
	if(!sysInfo)
		return;
	
	free((void*)sysInfo);
	sysInfo = NULL;
}
