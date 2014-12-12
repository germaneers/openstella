/*
 * USBCompositeDeviceDescriptor.h
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


#ifndef USBCOMPOSITEDEVICEDESCRIPTOR_H_
#define USBCOMPOSITEDEVICEDESCRIPTOR_H_

#include "USBAbstractDeviceDescriptor.h"
#include <StellarisWare/usblib/device/usbdcomp.h>

class USBCompositeDeviceDescriptor : public USBAbstractDeviceDescriptor {
private:
	struct {
	    unsigned short usVID;
	    unsigned short usPID;
	    unsigned short usMaxPowermA;
	    unsigned char ucPwrAttributes;
	    tUSBCallback pfnCallback;
	    const char * const *ppStringDescriptors;
	    unsigned long ulNumStringDescriptors;
	    unsigned long ulNumDevices;
	    tCompositeEntry *psDevices;
	    unsigned long *pulDeviceWorkspace;
	    tCompositeInstance *psPrivateData;
	} data;

protected:
	virtual void setStringDescriptors(pchar *stringDescriptors) { data.ppStringDescriptors = stringDescriptors; }
	virtual void setNumStringDescriptors(unsigned long num)   { data.ulNumStringDescriptors = num; }

public:
	USBCompositeDeviceDescriptor();

	virtual void setCallbackData(void *cbdata) { }
	virtual void setDeviceId(uint16_t vid, uint16_t pid) { data.usVID = vid; data.usPID=pid; }
	virtual void setPowerConfig(uint16_t max_power_ma, power_attributes_t attr) { data.usMaxPowermA = max_power_ma; data.ucPwrAttributes=attr; }
	virtual void setManufacturerString(const char *s) { setDescriptorString(1, s); }
	virtual void setProductString(const char *s) { setDescriptorString(2, s); }
	virtual void setSerialNumberString(const char *s) { setDescriptorString(3, s); }
	virtual void setPrivateData(void *d) { data.psPrivateData = (tCompositeInstance*)d; }
	virtual void *getPrivateData() { return data.psPrivateData; }
	virtual void setCallbackFunction(tUSBCallback cbfunc) { data.pfnCallback = cbfunc; }

	void setNumDevices(unsigned long numDevices) { data.ulNumDevices = numDevices; }
	void setDevices(tCompositeEntry *devices) { data.psDevices = devices; }
	void setDeviceWorkspace(unsigned long *deviceWorkspace) { data.pulDeviceWorkspace = deviceWorkspace; }

	virtual void *getDataStruct() { return &data; }
};

#endif /* USBCOMPOSITEDEVICEDESCRIPTOR_H_ */
