/*
 * USBKeyboardDevice.h
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
#ifndef USBKEYBOARDDEVICE_H_
#define USBKEYBOARDDEVICE_H_

#include "USBHIDDevice.h"

#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/usblib/usblib.h>
#include <StellarisWare/usblib/usbhid.h>
#include <StellarisWare/usblib/device/usbdhid.h>
#include <StellarisWare/usblib/device/usbdhidkeyb.h>

class USBKeyboardDevice : public USBHIDDevice {
public:
	typedef enum {
		modifier_left_crtl   = 0x01,
		modifier_left_shift  = 0x02,
		modifier_left_alt    = 0x04,
		modifier_left_gui    = 0x08,
		modifier_right_ctrl  = 0x10,
		modifier_right_shift = 0x20,
		modifier_right_alt   = 0x40,
		modifier_right_gui   = 0x80
	} modifier_t;

public:
	USBKeyboardDevice(USBController *controller=0);
	void enable();
	void *compositeInit();

	bool sendKeyEvent(uint8_t keycode, uint8_t modifiers, bool isPressed);
	bool sendKeyPress(uint8_t keycode, uint8_t modifiers=0);
	bool sendKeyRelease(uint8_t keycode, uint8_t modifiers=0);
	bool sendString(char *s);
};

#endif /* USBKEYBOARDDEVICE_H_ */
