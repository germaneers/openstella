/*
 * USBCDCDevice.cpp
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


#include "USBCDCDevice.h"
#include <string.h>
#include <stdint.h>
#include "../OS/Task.h"
#include "../OS/Mutex.h"

extern "C" {

	unsigned long USBCDCDeviceRxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgData, void *pvMsgData)
	{
		return ((USBCDCDevice*)pvCBData)->RxHandler(pvCBData, ulEvent, ulMsgData, pvMsgData);
	}

	unsigned long USBCDCDeviceTxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgData, void *pvMsgData)
	{
		return ((USBCDCDevice*)pvCBData)->TxHandler(pvCBData, ulEvent, ulMsgData, pvMsgData);
	}
}


USBCDCDevice::USBCDCDevice(USBController *controller)
  : USBDevice(controller, new USBCDCDeviceDescriptor())
{
	USBCDCDeviceDescriptor *desc = getUSBCDCDeviceDescriptor();
	desc->setRxCallbackFunction(USBCDCDeviceRxHandler);
	desc->setRxCallbackData(this);

	desc->setTxCallbackFunction(USBCDCDeviceTxHandler);
	desc->setTxCallbackData(this);

	desc->setProductString("Openstella CDC Device");
	desc->setPrivateData(new tCDCSerInstance);

	_rxSemaphore.take(0);
}

void USBCDCDevice::enable()
{
	USBDCDCInit(_controller->getNumber(), (const tUSBDCDCDevice*)getDeviceDescriptorData());
	USBDevice::enable();
}

void *USBCDCDevice::compositeInit()
{
	return USBDCDCCompositeInit(_controller->getNumber(), (const tUSBDCDCDevice*)getDeviceDescriptorData());
}


unsigned long USBCDCDevice::RxHandler(void *pvCBData, unsigned long  ulEvent, unsigned long  ulMsgData, void *pvMsgData)
{
    switch(ulEvent)
    {
        case USB_EVENT_RX_AVAILABLE:
        	_rxSemaphore.giveFromISR();
            break;

        case USB_EVENT_DATA_REMAINING:
        	return(0);

        case USB_EVENT_REQUEST_BUFFER:
            return(0);

        default:
            break;
    }

    return(0);
}

unsigned long USBCDCDevice::TxHandler(void *pvCBData, unsigned long  ulEvent, unsigned long  ulMsgData, void *pvMsgData)
{
    switch(ulEvent) {
        case USB_EVENT_TX_COMPLETE:
        	_txSemaphore.giveFromISR();
            break;
        default:
            break;
    }

    return(0);
}

unsigned long USBCDCDevice::DeviceHandler(void *pvCBData, unsigned long  ulEvent, unsigned long  ulMsgData, void *pvMsgData)
{
	switch (ulEvent) {
		case USB_EVENT_CONNECTED:
			_txSemaphore.giveFromISR();
			_rxSemaphore.take(0);
			break;
        case USB_EVENT_DISCONNECTED:
        	_txSemaphore.take(0);
			_rxSemaphore.take(0);
        	break;
		case USBD_CDC_EVENT_GET_LINE_CODING:
			tLineCoding *psLineCoding = (tLineCoding*) pvMsgData;
			psLineCoding->ulRate     = 115200;
			psLineCoding->ucDatabits = 8;
			psLineCoding->ucParity   = USB_CDC_PARITY_NONE;
			psLineCoding->ucStop     = USB_CDC_STOP_BITS_1;
			return 0;
	}
	return USBDevice::DeviceHandler(pvCBData, ulEvent, ulMsgData, pvMsgData);
}

int USBCDCDevice::readBuffer(uint8_t *buf, uint32_t bufferSize, uint32_t timeout)
{
	static Mutex lock;
	MutexGuard guard(&lock);

	if (!_rxSemaphore.take(timeout)) { return -1; }
    uint32_t bytesAvail = USBDCDCRxPacketAvailable(getDeviceDescriptorData());
    int bytesRead = USBDCDCPacketRead(getDeviceDescriptorData(), (uint8_t*)buf, bufferSize, false);
    if (bytesAvail>bufferSize) { _rxSemaphore.give(); }
    return bytesRead;
}

int USBCDCDevice::readString(char *buf, uint32_t bufferSize, uint32_t timeout)
{
	uint32_t bytesRead = readBuffer((uint8_t*)buf, bufferSize-1, timeout);
	if (bytesRead >= 0) {
		buf[bytesRead] = 0;
		return bytesRead;
	} else {
		return -1;
	}
}

int USBCDCDevice::getChar(uint32_t timeout)
{
	uint8_t ch;
	if (readBuffer(&ch, 1, timeout)==1) {
		return ch;
	} else {
		return -1;
	}
}

bool USBCDCDevice::writeBuffer(uint8_t *buf, uint32_t bufferSize, uint32_t timeout)
{
	while (bufferSize>0) {

		if (!_txSemaphore.take(timeout)) { return false; }

		uint32_t packetSize = bufferSize;
		if (bufferSize>64) {
			packetSize = 64;
		}

		while (1) {
			uint32_t result = USBDCDCPacketWrite(getDeviceDescriptorData(), buf, packetSize, 1);
			if (result>0) break;
			if (!isConnected()) return false;
		}

		bufferSize -= packetSize;
		buf += packetSize;

	}
	return true;
}

bool USBCDCDevice::writeString(char *s, uint32_t timeout)
{
	return writeBuffer((uint8_t*)s, strlen(s), timeout);
}

bool USBCDCDevice::putChar(char ch, uint32_t timeout)
{
	return writeBuffer((uint8_t*)&ch, 1, timeout);
}

void USBCDCDevice::setDeviceId(uint16_t vid, uint16_t pid)
{
	getUSBCDCDeviceDescriptor()->setDeviceId(vid, pid);
}

void USBCDCDevice::setManufacturerString(const char *s)
{
	getUSBCDCDeviceDescriptor()->setDescriptorString(1, s);
}

void USBCDCDevice::setProductString(const char *s)
{
	getUSBCDCDeviceDescriptor()->setDescriptorString(2, s);
}

void USBCDCDevice::setSerialNumberString(const char *s)
{
	getUSBCDCDeviceDescriptor()->setDescriptorString(3, s);
}

void USBCDCDevice::setControlInterfaceDescString(const char *s)
{
	getUSBCDCDeviceDescriptor()->setDescriptorString(4, s);
}

void USBCDCDevice::setConfigDescString(const char *s)
{
	getUSBCDCDeviceDescriptor()->setDescriptorString(5, s);
}
