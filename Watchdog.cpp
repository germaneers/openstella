/*
 * Watchdog.cpp
 *
 *  Created on: 15.03.2013
 *      Author: denkmahu
 */

#include "Watchdog.h"
#include <lib/StellarisWare/inc/hw_types.h>
#include <lib/StellarisWare/inc/hw_ints.h>
#include <lib/StellarisWare/inc/hw_memmap.h>
#include <lib/StellarisWare/driverlib/sysctl.h>
#include <lib/StellarisWare/driverlib/interrupt.h>
#include <lib/StellarisWare/driverlib/rom_map.h>
#include <lib/StellarisWare/driverlib/watchdog.h>

void Watchdog::setup(uint32_t interval_ms, bool lock)
{
	uint32_t interval_ticks = interval_ms * (MAP_SysCtlClockGet()/1000);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
	MAP_IntEnable(INT_WATCHDOG);
	MAP_WatchdogReloadSet(WATCHDOG0_BASE, interval_ticks);
	MAP_WatchdogResetEnable(WATCHDOG0_BASE);
	MAP_WatchdogEnable(WATCHDOG0_BASE);
	MAP_WatchdogStallEnable(WATCHDOG0_BASE);  //allow debugger to stop watchdog when in breakpoint!
	if (lock) { MAP_WatchdogLock(WATCHDOG0_BASE); }
}

void Watchdog::feed()
{
	MAP_WatchdogIntClear(WATCHDOG0_BASE);
}



