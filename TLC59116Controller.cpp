/*
 * TCL59116Controller.cpp
 *
 * Copyright 2012 Germaneers GmbH
 * Copyright 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
 * Copyright 2012 Stefan Rupp (stefan.rupp@germaneers.com)
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

#include "TLC59116Controller.h"

TLC59116Controller::TLC59116Controller(I2CController *i2c, uint8_t addr, GPIOPin resetPin)
: _addr(addr), _i2c(i2c), _resetPin(resetPin), _ledoutShadow({0,0,0,0})
{
}

void TLC59116Controller::setRegister(uint8_t reg, uint8_t value)
{
	_i2c->write16(_addr, (reg<<8) | value);
}

void TLC59116Controller::setup(uint8_t mode1, uint8_t mode2)
{
	_resetPin.enablePeripheral();
	_resetPin.configureAsOutput();
	_resetPin.setHigh();
	setRegister(0, mode1);
	setRegister(1, mode2);
}

void TLC59116Controller::setPWM(uint8_t ledNum, uint8_t value)
{
	if (ledNum>15) return;
	setRegister(0x02 + ledNum, value);
}

void TLC59116Controller::setLedState(uint8_t ledNum, state_t state)
{
	if (ledNum>15) return;
	uint8_t reg = ledNum / 4;
	uint8_t shift = (ledNum % 4) * 2;
	_ledoutShadow[reg] &= ~(0x03 << shift);
	_ledoutShadow[reg] |= (state << shift);
	setRegister(0x14 + reg, _ledoutShadow[reg]);
}

void TLC59116Controller::setLedFullOn(uint8_t ledNum, bool isOn)
{
	setLedState(ledNum, isOn ?  TLC59116Controller::state_full_on :  TLC59116Controller::state_off);
}

void TLC59116Controller::setAllOn()
{
	setRegister(0x14, 0x55);
	setRegister(0x15, 0x55);
	setRegister(0x16, 0x55);
	setRegister(0x17, 0x55);
}







