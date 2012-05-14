/*
 * main.cpp
 *
 * Copyright 2011, 2012 Germaneers GmbH
 * Copyright 2011, 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
 *
 * This file is part of libopenstella.
 *
 * libopenstella is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * libopenstella is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libopenstella.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>

#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_ints.h>
#include <StellarisWare/driverlib/rom.h>
#include <StellarisWare/driverlib/interrupt.h>
#include <StellarisWare/driverlib/sysctl.h>

#include <freertos/include/FreeRTOS.h>
#include <freertos/include/task.h>

#include "../../src/MainTask.h"

extern "C" {

	void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
	{
		/* This function will get called if a task overflows its stack.   If the
		parameters are corrupt then inspect pxCurrentTCB to find which was the
		offending task. */

		( void ) pxTask;
		( void ) pcTaskName;

		for( ;; );
	}

	void vApplicationTickHook( void )
	{

	}

	void vApplicationMallocFailedHook( void )
	{
		while(1);
	}

	extern void vPortSVCHandler(void);
	extern void xPortPendSVHandler(void);
	extern void xPortSysTickHandler( void );

}

int main(void)
{
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	IntRegister(FAULT_SVCALL, vPortSVCHandler);
	IntRegister(FAULT_PENDSV, xPortPendSVHandler);
	IntRegister(FAULT_SYSTICK, xPortSysTickHandler);

	ROM_IntMasterEnable();
	MainTask *mt = new MainTask();
	mt->run();
	vTaskStartScheduler();
}
