/*
 * USBCompositeDevice.cpp
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


#include "USBCompositeDevice.h"

USBCompositeDevice::USBCompositeDevice(USBController *controller)
  : USBDevice(controller, new USBCompositeDeviceDescriptor()), _devices()
{
	_deviceDescriptor->setProductString("Openstella Composite Device");
}

void USBCompositeDevice::addDevice(USBDevice *device)
{
	_devices.add(device);
}

void USBCompositeDevice::enable()
{
	uint8_t num = _devices.count();
	tCompositeEntry *a = new tCompositeEntry[num];

	uint8_t c=0; int sumCompositeSize=0;
	for (LinkedList<USBDevice*>::Item *i=_devices.getFirstItem(); i!=0; i=i->getNext()) {
		a[c].psDevice = i->getData()->getDeviceInfoStruct();
		a[c].pvInstance = i->getData()->compositeInit();
		sumCompositeSize += i->getData()->getCompositeSize();
		c++;
	}

	USBCompositeDeviceDescriptor *desc = getUSBCompositeDeviceDescriptor();
	desc->setCallbackFunction(0);
	desc->setNumDevices(num);
	desc->setDevices(a);
	long unsigned *workspace = new long unsigned[num];
	desc->setDeviceWorkspace(workspace);
	desc->setPrivateData(new tCompositeInstance);

	USBStackModeSet(_controller->getNumber(), USB_MODE_FORCE_DEVICE, 0);
	USBDCompositeInit(_controller->getNumber(), (tUSBDCompositeDevice*)desc->getDataStruct(), sumCompositeSize, new uint8_t[sumCompositeSize]);
	USBDevice::enable();
}



