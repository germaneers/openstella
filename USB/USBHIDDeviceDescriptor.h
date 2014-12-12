/*
 * USBHIDDeviceDescriptor.h
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


#ifndef USBHIDDEVICEDESCRIPTOR_H_
#define USBHIDDEVICEDESCRIPTOR_H_

#include "USBDeviceDescriptor.h"

class USBHIDDeviceDescriptor : public USBDefaultDeviceDescriptor {
public:
	USBHIDDeviceDescriptor();
	virtual void setHIDInterfaceString(const char *s) { setDescriptorString(4, s); }
	virtual void setConfigString(const char *s) { setDescriptorString(5, s); }
};

#endif /* USBHIDDEVICEDESCRIPTOR_H_ */
