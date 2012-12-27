/*
 * USBMouseDevice.cpp
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

#include "USBMouseDevice.h"
#include "USBHIDDeviceDescriptor.h"

USBMouseDevice::USBMouseDevice(USBController *controller)
: USBHIDDevice(controller)
{
	USBHIDDeviceDescriptor *desc = getUSBHIDDeviceDescriptor();
	desc->setProductString("Openstella Mouse");
	desc->setHIDInterfaceString("HID Mouse Interface");
	desc->setConfigString("HID Mouse Configuration");
	desc->setPrivateData(new tHIDMouseInstance);
}

void USBMouseDevice::enable()
{
	USBDHIDMouseInit(_controller->getNumber(), (const tUSBDHIDMouseDevice*)getDeviceDescriptorData());
	USBHIDDevice::enable();
}

void *USBMouseDevice::compositeInit()
{
	return USBDHIDMouseCompositeInit(_controller->getNumber(), (const tUSBDHIDMouseDevice*)getDeviceDescriptorData());
}

bool USBMouseDevice::sendMouseEvent(int8_t deltaX, int8_t deltaY, uint8_t buttonState)
{
	if (!isConnected()) { return false; }

	_state = state_sending;
    uint32_t result = USBDHIDMouseStateChange(getDeviceDescriptorData(), deltaX, deltaY, buttonState);

    if(result == MOUSE_SUCCESS) {
        if(!waitForSendIdle(500)) {
            _connected = false;
            return false;
        }
    } else {
    	return false;
    }

    return true;

}


