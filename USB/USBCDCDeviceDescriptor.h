/*
 * USBCDCDeviceDescriptor.h
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


#ifndef USBCDCDEVICEDESCRIPTOR_H_
#define USBCDCDEVICEDESCRIPTOR_H_

#include "USBAbstractDeviceDescriptor.h"
#include <StellarisWare/usblib/usbcdc.h>
#include <StellarisWare/usblib/device/usbdcdc.h>

class USBCDCDeviceDescriptor : public USBAbstractDeviceDescriptor {
private:
	struct {
	    unsigned short usVID;
	    unsigned short usPID;
	    unsigned short usMaxPowermA;
	    unsigned char ucPwrAttributes;

	    tUSBCallback pfnControlCallback;
	    void *pvControlCBData;

	    tUSBCallback pfnRxCallback;
	    void *pvRxCBData;

	    tUSBCallback pfnTxCallback;
	    void *pvTxCBData;

	    //! must contain the following string descriptor pointers in this order.
	    //! Language descriptor, Manufacturer name string (language 1), Product
	    //! name string (language 1), Serial number string (language 1),
	    //! Control interface description string (language 1), Configuration
	    //! description string (language 1).
	    const char * const *ppStringDescriptors;
	    unsigned long ulNumStringDescriptors;

	    tCDCSerInstance *psPrivateCDCSerData;
	} data;

protected:
	virtual void setStringDescriptors(pchar *stringDescriptors) { data.ppStringDescriptors = stringDescriptors; }
	virtual void setNumStringDescriptors(unsigned long num)   { data.ulNumStringDescriptors = num; }

public:
	USBCDCDeviceDescriptor();

	virtual void setDeviceId(uint16_t vid, uint16_t pid) { data.usVID = vid; data.usPID=pid; }
	virtual void setPowerConfig(uint16_t max_power_ma, power_attributes_t attr) { data.usMaxPowermA = max_power_ma; data.ucPwrAttributes=attr; }
	virtual void setManufacturerString(const char *s) { setDescriptorString(1, makeDescriptorString(s)); }
	virtual void setProductString(const char *s) { setDescriptorString(2, makeDescriptorString(s)); }
	virtual void setSerialNumberString(const char *s) { setDescriptorString(3, makeDescriptorString(s)); }
	virtual void setControlInterfaceDescString(const char *s) { setDescriptorString(4, makeDescriptorString(s)); }
	virtual void setConfigDescString(const char *s) { setDescriptorString(5, makeDescriptorString(s)); }

	virtual void setPrivateData(void *d) { data.psPrivateCDCSerData = (tCDCSerInstance*)d; }
	virtual void *getPrivateData() { return data.psPrivateCDCSerData; }

	virtual void setCallbackFunction(tUSBCallback cbfunc) { data.pfnControlCallback = cbfunc; }
	virtual void setCallbackData(void *cbdata) { data.pvControlCBData = cbdata; }

	virtual void setRxCallbackFunction(tUSBCallback cbfunc) { data.pfnRxCallback = cbfunc; }
	virtual void setRxCallbackData(void *cbdata) { data.pvRxCBData = cbdata; }
	virtual void setTxCallbackFunction(tUSBCallback cbfunc) { data.pfnTxCallback = cbfunc; }
	virtual void setTxCallbackData(void *cbdata) { data.pvTxCBData = cbdata; }

	virtual void *getDataStruct() { return &data; }
};

#endif /* USBCDCDEVICEDESCRIPTOR_H_ */
