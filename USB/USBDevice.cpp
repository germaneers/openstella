/*
 * USBDevice.cpp
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


#include "USBDevice.h"

#include <string.h>
#include "../OS/Task.h"

extern "C" {
	unsigned long USBDeviceHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgData, void *pvMsgData)
	{
		return ((USBDevice*)pvCBData)->DeviceHandler(pvCBData, ulEvent, ulMsgData, pvMsgData);
	}
}

USBDevice::USBDevice(USBController *controller, USBAbstractDeviceDescriptor *deviceDescriptor)
  : _controller(controller),
    _deviceDescriptor(deviceDescriptor),
	_connected(false),
	_state(state_unconfigured)
{
	if (_controller==0) { controller = USBController::get(USBController::controller_0); }
	_deviceDescriptor->setCallbackData(this);
	_deviceDescriptor->setCallbackFunction(USBDeviceHandler);
	_stateChangeSemaphore.take(0);
}

void *USBDevice::getDeviceDescriptorData()
{
	return _deviceDescriptor->getDataStruct();
}

bool USBDevice::waitForSendIdle(uint32_t timeout_ms)
{
	uint32_t now = Task::getTime();
	uint32_t timeout = now + timeout_ms;
	while (1) {
		if (!_stateChangeSemaphore.take(timeout - now)) { return false; }
		if (_state==state_idle) { return true; }
		now = Task::getTime();
		if (now > timeout) { return false; }
	}
}

bool USBDevice::isConnected()
{
	return _connected;
}

unsigned long USBDevice::DeviceHandler(void *pvCBData, unsigned long  ulEvent, unsigned long  ulMsgData, void *pvMsgData)
{
    switch(ulEvent)
    {
        case USB_EVENT_CONNECTED:
        	_state = state_idle;
        	_stateChangeSemaphore.giveFromISR();
        	_connected = true;
            break;

        case USB_EVENT_DISCONNECTED:
        	_connected = false;
        	_state = state_unconfigured;
        	_stateChangeSemaphore.giveFromISR();
            break;

        case USB_EVENT_TX_COMPLETE:
        	_state = state_idle;
        	_stateChangeSemaphore.giveFromISR();
            break;

    }
    return(0);
}

void USBDevice::enable()
{
	_controller->setMode(USBController::mode_device);
}


