/*
 * USBSerialConverter.h
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

#ifndef USBSERIALCONVERTER_H_
#define USBSERIALCONVERTER_H_

#include <openstella/USB/USBCDCDevice.h>
#include <openstella/UART.h>
#include <openstella/OS/Task.h>
#include <openstella/OS/Mutex.h>

class USBSerialConverter : public USBCDCDevice, public Task {
private:
	UARTController *_uart;
	Mutex _lock;

	void readFromUSBwriteToUARTTask();

protected:
	virtual void execute();

public:
	USBSerialConverter(UARTController *uart, USBController *controller=0);
	virtual unsigned long DeviceHandler(void *pvCBData, unsigned long  ulEvent, unsigned long  ulMsgData, void *pvMsgData);


};

#endif /* USBSERIALCONVERTER_H_ */
