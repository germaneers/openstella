/*
 * USBAbstractDeviceDescriptor.h
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


#ifndef USBABSTRACTDEVICEDESCRIPTOR_H_
#define USBABSTRACTDEVICEDESCRIPTOR_H_

#include <stdint.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/usblib/usblib.h>

class USBAbstractDeviceDescriptor {
public:
	typedef char* pchar;

	typedef enum {
		self_powered = 0xC0,
		bus_powered  = 0x80
	} power_attributes_t;

protected:
	char _langDescriptor[4];
	pchar *_stringDescriptors;

	char *makeDescriptorString(const char *s);

	USBAbstractDeviceDescriptor(uint8_t numStringDescriptors);

public:
	void setDescriptorString(uint8_t desc_num, const char *s);

	virtual void setStringDescriptors(pchar *_stringDescriptors) = 0;
	virtual void setNumStringDescriptors(unsigned long num) = 0;
	virtual void setCallbackData(void *cbdata) = 0;
	virtual void setDeviceId(uint16_t vid, uint16_t pid) = 0;
	virtual void setPowerConfig(uint16_t max_power_ma, power_attributes_t attr) = 0;
	virtual void setManufacturerString(const char *s) = 0;
	virtual void setProductString(const char *s) = 0;
	virtual void setSerialNumberString(const char *s) = 0;
	virtual void setPrivateData(void *d) = 0;
	virtual void *getPrivateData() = 0;
	virtual void setCallbackFunction(tUSBCallback cbfunc) = 0;
	virtual void *getDataStruct() = 0;

};

#endif /* USBABSTRACTDEVICEDESCRIPTOR_H_ */
