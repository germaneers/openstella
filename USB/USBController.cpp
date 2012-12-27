/*
 * USBController.cpp
 *
 * Copyright 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
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


#include "USBController.h"
#include "../OS/Mutex.h"
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/inc/hw_ints.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/interrupt.h>
#include <StellarisWare/driverlib/rom.h>
#include <StellarisWare/driverlib/rom_map.h>
#include <StellarisWare/driverlib/usb.h>

#include <StellarisWare/usblib/device/usbdevice.h>
#include <StellarisWare/usblib/device/usbdcomp.h>

USBController* USBController::_instances[] = { 0 };

USBController *USBController::get(controller_num_t num)
{
	static Mutex lock;
	MutexGuard guard(&lock);

	if (_instances[num]==0) {
		_instances[num] = new USBController(num);
	}
	return _instances[num];
}

USBController::USBController(controller_num_t num) :
	_num(num), _mode(mode_none)
{
	switch (num) {
		case controller_0:
			_base = USB0_BASE;
			_int  = INT_USB0;
			_periph = SYSCTL_PERIPH_USB0;
			_intHandler = USB0DeviceIntHandler;
			_otgIntHandler = USB0OTGModeIntHandler;
			break;
		default:
			while(1);
	}
	MAP_SysCtlPeripheralEnable(_periph);
	MAP_IntPrioritySet(INT_USB0, configMAX_SYSCALL_INTERRUPT_PRIORITY);
}

void USBController::setMode(mode_t mode, tUSBModeCallback modeHandlerCallback) {
	_mode = mode;
	switch (_mode) {
		case mode_device:
		case mode_force_device:
			USBIntRegister(_base, _intHandler);
			 break;
		case mode_host:
		case mode_force_host:
		case mode_otg:
			USBIntRegister(_base, _otgIntHandler);
			break;
		case mode_none:
			break;
	}
	USBStackModeSet(_num, (tUSBMode)mode, modeHandlerCallback);
}

USBController::mode_t USBController::getMode() {
	return _mode;
}
