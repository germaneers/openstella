/*
 * USBKeyboardDevice.cpp
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

#include "USBKeyboardDevice.h"
#include "USBHIDDeviceDescriptor.h"
static const char g_ppcKeyUsageCodes[][2] =
{
    { 0, HID_KEYB_USAGE_SPACE },                       //   0x20
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_1 },         // ! 0x21
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_FQUOTE },    // " 0x22
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_3 },         // # 0x23
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_4 },         // $ 0x24
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_5 },         // % 0x25
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_7 },         // & 0x26
    { 0, HID_KEYB_USAGE_FQUOTE },                      // ' 0x27
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_9 },         // ( 0x28
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_0 },         // ) 0x29
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_8 },         // * 0x2a
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_EQUAL },     // + 0x2b
    { 0, HID_KEYB_USAGE_COMMA },                       // , 0x2c
    { 0, HID_KEYB_USAGE_MINUS },                       // - 0x2d
    { 0, HID_KEYB_USAGE_PERIOD },                      // . 0x2e
    { 0, HID_KEYB_USAGE_FSLASH },                      // / 0x2f
    { 0, HID_KEYB_USAGE_0 },                           // 0 0x30
    { 0, HID_KEYB_USAGE_1 },                           // 1 0x31
    { 0, HID_KEYB_USAGE_2 },                           // 2 0x32
    { 0, HID_KEYB_USAGE_3 },                           // 3 0x33
    { 0, HID_KEYB_USAGE_4 },                           // 4 0x34
    { 0, HID_KEYB_USAGE_5 },                           // 5 0x35
    { 0, HID_KEYB_USAGE_6 },                           // 6 0x36
    { 0, HID_KEYB_USAGE_7 },                           // 7 0x37
    { 0, HID_KEYB_USAGE_8 },                           // 8 0x38
    { 0, HID_KEYB_USAGE_9 },                           // 9 0x39
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_SEMICOLON }, // : 0x3a
    { 0, HID_KEYB_USAGE_SEMICOLON },                   // ; 0x3b
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_COMMA },     // < 0x3c
    { 0, HID_KEYB_USAGE_EQUAL },                       // = 0x3d
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_PERIOD },    // > 0x3e
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_FSLASH },    // ? 0x3f
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_2 },         // @ 0x40
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_A },         // A 0x41
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_B },         // B 0x42
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_C },         // C 0x43
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_D },         // D 0x44
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_E },         // E 0x45
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_F },         // F 0x46
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_G },         // G 0x47
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_H },         // H 0x48
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_I },         // I 0x49
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_J },         // J 0x4a
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_K },         // K 0x4b
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_L },         // L 0x4c
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_M },         // M 0x4d
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_N },         // N 0x4e
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_O },         // O 0x4f
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_P },         // P 0x50
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_Q },         // Q 0x51
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_R },         // R 0x52
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_S },         // S 0x53
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_T },         // T 0x54
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_U },         // U 0x55
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_V },         // V 0x56
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_W },         // W 0x57
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_X },         // X 0x58
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_Y },         // Y 0x59
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_Z },         // Z 0x5a
    { 0, HID_KEYB_USAGE_LBRACKET },                    // [ 0x5b
    { 0, HID_KEYB_USAGE_BSLASH },                      // \ 0x5c
    { 0, HID_KEYB_USAGE_RBRACKET },                    // ] 0x5d
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_6 },         // ^ 0x5e
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_MINUS },     // _ 0x5f
    { 0, HID_KEYB_USAGE_BQUOTE },                      // ` 0x60
    { 0, HID_KEYB_USAGE_A },                           // a 0x61
    { 0, HID_KEYB_USAGE_B },                           // b 0x62
    { 0, HID_KEYB_USAGE_C },                           // c 0x63
    { 0, HID_KEYB_USAGE_D },                           // d 0x64
    { 0, HID_KEYB_USAGE_E },                           // e 0x65
    { 0, HID_KEYB_USAGE_F },                           // f 0x66
    { 0, HID_KEYB_USAGE_G },                           // g 0x67
    { 0, HID_KEYB_USAGE_H },                           // h 0x68
    { 0, HID_KEYB_USAGE_I },                           // i 0x69
    { 0, HID_KEYB_USAGE_J },                           // j 0x6a
    { 0, HID_KEYB_USAGE_K },                           // k 0x6b
    { 0, HID_KEYB_USAGE_L },                           // l 0x6c
    { 0, HID_KEYB_USAGE_M },                           // m 0x6d
    { 0, HID_KEYB_USAGE_N },                           // n 0x6e
    { 0, HID_KEYB_USAGE_O },                           // o 0x6f
    { 0, HID_KEYB_USAGE_P },                           // p 0x70
    { 0, HID_KEYB_USAGE_Q },                           // q 0x71
    { 0, HID_KEYB_USAGE_R },                           // r 0x72
    { 0, HID_KEYB_USAGE_S },                           // s 0x73
    { 0, HID_KEYB_USAGE_T },                           // t 0x74
    { 0, HID_KEYB_USAGE_U },                           // u 0x75
    { 0, HID_KEYB_USAGE_V },                           // v 0x76
    { 0, HID_KEYB_USAGE_W },                           // w 0x77
    { 0, HID_KEYB_USAGE_X },                           // x 0x78
    { 0, HID_KEYB_USAGE_Y },                           // y 0x79
    { 0, HID_KEYB_USAGE_Z },                           // z 0x7a
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_LBRACKET },  // { 0x7b
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_BSLASH },    // | 0x7c
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_RBRACKET },  // } 0x7d
    { HID_KEYB_LEFT_SHIFT, HID_KEYB_USAGE_BQUOTE },    // ~ 0x7e
};

USBKeyboardDevice::USBKeyboardDevice(USBController *controller)
: USBHIDDevice(controller)
{
	USBHIDDeviceDescriptor *desc = getUSBHIDDeviceDescriptor();
	desc->setProductString("Openstella Keyboard");
	desc->setHIDInterfaceString("HID Keyboard Interface");
	desc->setConfigString("HID Keyboard Configuration");
	desc->setPrivateData(new tHIDKeyboardInstance);
}

void USBKeyboardDevice::enable()
{
	USBDHIDKeyboardInit(_controller->getNumber(), (const tUSBDHIDKeyboardDevice*)getDeviceDescriptorData());
	USBHIDDevice::enable();
}

void *USBKeyboardDevice::compositeInit()
{
	return USBDHIDKeyboardCompositeInit(_controller->getNumber(), (const tUSBDHIDKeyboardDevice*)getDeviceDescriptorData());
}

bool USBKeyboardDevice::sendKeyEvent(uint8_t keycode, uint8_t modifiers, bool isPressed)
{
	_state = state_sending;
	return 	USBDHIDKeyboardKeyStateChange(getDeviceDescriptorData(), modifiers, keycode, isPressed) == KEYB_SUCCESS;
}

bool USBKeyboardDevice::sendKeyPress(uint8_t keycode, uint8_t modifiers)
{
	return sendKeyEvent(keycode, modifiers, true);
}

bool USBKeyboardDevice::sendKeyRelease(uint8_t keycode, uint8_t modifiers)
{
	return sendKeyEvent(keycode, modifiers, false);
}

bool USBKeyboardDevice::sendString(char *s)
{
	if (!isConnected()) { return false; }
	uint8_t ch;
	while(*s)
	{
		ch = *s++;
		if((ch < ' ') || (ch > '~')) { continue; }
		ch -= ' ';

		if (!sendKeyPress(g_ppcKeyUsageCodes[ch][1], g_ppcKeyUsageCodes[ch][0])) { return false; }
		if(!waitForSendIdle(500)) { _connected = false; return false; }
		if (!sendKeyRelease(g_ppcKeyUsageCodes[ch][1])) { return false; }
		if(!waitForSendIdle(500)) { _connected = false; return false; }

	}
	return true;
}
