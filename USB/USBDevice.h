/*
 * USBDevice.h
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


#ifndef USBDEVICE_H_
#define USBDEVICE_H_

#include <stdint.h>

#include "USBController.h"
#include "USBDeviceDescriptor.h"

#include "../OS/Semaphore.h"
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/usblib/usblib.h>
#include <StellarisWare/usblib/usbhid.h>
#include <StellarisWare/usblib/device/usbdhid.h>

class USBDevice {
protected:

	USBController *_controller;
	USBAbstractDeviceDescriptor *_deviceDescriptor;

	bool _connected;

	Semaphore _stateChangeSemaphore;

	volatile enum {
		state_unconfigured,
		state_idle,
		state_sending
	} _state;

	void *getDeviceDescriptorData();

	bool waitForSendIdle(uint32_t timeout_ms);
	USBDevice(USBController *controller, USBAbstractDeviceDescriptor *deviceDescriptor);

public:
	bool isConnected();
	virtual void enable();
	virtual tDeviceInfo *getDeviceInfoStruct() = 0;
	virtual void *compositeInit() = 0;
	virtual uint32_t getCompositeSize() = 0;
	USBAbstractDeviceDescriptor *getDeviceDescriptor() { return _deviceDescriptor; }

public:
	virtual unsigned long DeviceHandler(void *pvCBData, unsigned long  ulEvent, unsigned long  ulMsgData, void *pvMsgData);
};

#endif /* USBDEVICE_H_ */
