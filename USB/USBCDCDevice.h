/*
 * USBCDCDevice.h
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


#ifndef USBCDCDEVICE_H_
#define USBCDCDEVICE_H_

#include "USBDevice.h"
#include <stdint.h>
#include "USBCDCDeviceDescriptor.h"
#include "../OS/Semaphore.h"

class USBCDCDevice: public USBDevice {
private:
	Semaphore _txSemaphore;
	Semaphore _rxSemaphore;
	void handleRxAvail();
protected:
	USBCDCDeviceDescriptor *getUSBCDCDeviceDescriptor() { return (USBCDCDeviceDescriptor*) _deviceDescriptor; }
	virtual uint32_t getCompositeSize() { return COMPOSITE_DCDC_SIZE; }
	virtual tDeviceInfo *getDeviceInfoStruct()  { return &g_sCDCSerDeviceInfo; }
public:
	USBCDCDevice(USBController *controller=0);
	virtual void *compositeInit();
	virtual void enable();
	virtual unsigned long DeviceHandler(void *pvCBData, unsigned long  ulEvent, unsigned long  ulMsgData, void *pvMsgData);
	virtual unsigned long RxHandler(void *pvCBData, unsigned long  ulEvent, unsigned long  ulMsgData, void *pvMsgData);
	virtual unsigned long TxHandler(void *pvCBData, unsigned long  ulEvent, unsigned long  ulMsgData, void *pvMsgData);

	int  readBuffer(uint8_t *buf, uint32_t bufferSize, uint32_t timeout=0xFFFFFFFF);
	int  readString(char *buf, uint32_t bufferSize, uint32_t timeout=0xFFFFFFFF);
	int  getChar(uint32_t timeout=0xFFFFFFFF);

	bool writeBuffer(uint8_t *buf, uint32_t bufferSize, uint32_t timeout=0xFFFFFFFF);
	bool writeString(char *s, uint32_t timeout=0xFFFFFFFF);
	bool putChar(char ch, uint32_t timeout=0xFFFFFFFF);
};

#endif /* USBCDCDEVICE_H_ */
