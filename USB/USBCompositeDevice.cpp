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
  : USBDevice(controller, new USBCompositeDeviceDescriptor()), _devices(0)
{
	_deviceDescriptor->setProductString("Openstella Composite Device");
}

void USBCompositeDevice::addDevice(USBDevice *device)
{
	USBDeviceListItem *lli = new USBDeviceListItem(device);
	if (_devices==0) {
		_devices = lli;
	} else {
		USBDeviceListItem *last = _devices;
		while (last->next!=0) { last = last->next; }
		last->next = lli;
	}
}

uint32_t USBCompositeDevice::calcCompositeSize()
{
	uint32_t result = 0;
	for (USBDeviceListItem *dli = _devices; dli!=0; dli=dli->next) {
		result += dli->data->getCompositeSize();
	}
	return result;
}

uint8_t USBCompositeDevice::countDevices()
{
	uint8_t result = 0;
	for (USBDeviceListItem *dli = _devices; dli!=0; dli=dli->next) {
		result++;
	}
	return result;
}

void USBCompositeDevice::enable()
{

	uint8_t num = countDevices();

	USBCompositeDeviceDescriptor *desc = getUSBCompositeDeviceDescriptor();
	desc->setCallbackFunction(0);

	tCompositeEntry *a = new tCompositeEntry[num];
	int i=0; int compositeSize = 0;
	for (USBDeviceListItem *dli = _devices; dli!=0; dli=dli->next) {
		a[i].psDevice = dli->data->getDeviceInfoStruct();
		a[i].pvInstance = dli->data->compositeInit();
		compositeSize += dli->data->getCompositeSize();
		i++;
	}

	desc->setNumDevices(num);
	desc->setDevices(a);
	long unsigned *workspace = new long unsigned[num];
	desc->setDeviceWorkspace(workspace);
	desc->setPrivateData(new tCompositeInstance);


	USBDCompositeInit(_controller->getNumber(), (tUSBDCompositeDevice*)desc->getDataStruct(), compositeSize, new uint8_t[compositeSize]);


	USBDevice::enable();
}



