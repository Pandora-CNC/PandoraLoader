#include "types.h"
#include "config.h"
#include "hwinfo.h"
#include "clockcfg.h"
#include "errors.h"

#include "../wblib.h"
#include "../turbowriter.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(void)
{
	HWINFO_T hw;
	WB_UART_T uart1;
	
	// Evaluate the capabilities of the system
	if(hwInfo(&hw) != ERR_NONE)
		while(1); // If hwInfo fails, everything's lost; halt the CPU
	
	// Setup the system clocks and the memory hardware
	if(clockInit(&hw) != ERR_NONE)
		while(1); // If clockInit fails, everything's lost; halt the CPU
	
	// Now that the clock's running, configure the UART
	uart1.uiBaudrate = UART_BAUD;
	uart1.uiDataBits = WB_DATA_BITS_8;
	uart1.uiParity = WB_PARITY_NONE;
	uart1.uiStopBits = WB_STOP_BITS_1;
	uart1.uiRxTriggerLevel = LEVEL_1_BYTE;
	uart1.uiFreq = hw.ExtClockFreq * 1000; // Ext. clock frequency in Hertz
	
	// Then try to initialize the UART
	if(sysInitializeUART(&uart1) != Successful)
		while(1); // If sysInitializeUART fails, something's really bad; halt the CPU
	
	// Print the banner
	sysprintf(" (\n )\\ )            (\n(()/(   )        )\\ )    (      )\n" \
						" /(_)| /(  (    (()/( (  )(  ( /(\n(_)) )(_)) )\\ )  ((_)))\\(()\\ )(_))\n" \
						"| _ ((_)_ _(_/(  _| |((_)((_|(_)_\n|  _/ _` | ' \\)) _` / _ \\ '_/ _` |\n" \
						"|_| \\__,_|_||_|\\__,_\\___/_| \\__,_|\n\n");
	sysprintf("PandoraLoader v%s\n", VERSION);
	sysprintf("Copyright (c) 2017, Pandora CNC Development Team\n\n");
	
	// Print memory technology
	sysprintf("Memory technology: DDR");
	if(hw.MemoryType == MEMTYPE_DDR2)
		sysprintf("2\n");
	else
		sysprintf(" (SG%d)\n", DDR_SPGRADE);
	
	// Print ext. clock freq.
	sysprintf("Ext. clock frequency: %d MHz\n", hw.ExtClockFreq / 1000);
	// Print PLL target freq.
	sysprintf("PLL target frequency: %d MHz\n\n", hw.TargetPllFreq / 1000);
	
	// Print message that the NAND interface is being initialized
	sysprintf("Initializing the NAND interface...");
	// Actually initialize the NAND interface
	fmiInitDevice();
	if(sicSMInit() != 0) {
		sysprintf(" [FAIL]"); // Confirm failure
		while(1); // Halt CPU
	} else
		sysprintf(" [OK]\n"); // Confirm success
	
	sysprintf("Analyzing the Bootsector:");
	
	return 0;
}
