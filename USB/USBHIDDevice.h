/*
 * USBHIDDevice.h
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


#ifndef USBHIDDEVICE_H_
#define USBHIDDEVICE_H_

#include "USBDevice.h"
#include "USBHIDDeviceDescriptor.h"

class USBHIDDevice : public USBDevice {
protected:
	USBHIDDevice(USBController *controller=0);

	USBHIDDeviceDescriptor *getUSBHIDDeviceDescriptor() { return (USBHIDDeviceDescriptor*) _deviceDescriptor; }
	virtual uint32_t getCompositeSize() { return COMPOSITE_DHID_SIZE; }
	virtual tDeviceInfo *getDeviceInfoStruct()  { return &g_sHIDDeviceInfo; }

};

#endif /* USBHIDDEVICE_H_ */
