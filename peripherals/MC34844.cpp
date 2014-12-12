/*
 * MC34844.cpp
 *
 * Copyright 2012 Germaneers GmbH
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

#include "MC34844.h"
#include <inc/hw_types.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <StellarisWare/inc/hw_i2c.h>
#include <StellarisWare/inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <stdlib.h>


MC34844::MC34844()
//: _chipSelect(0), _pwmPin(0), _lastValue(0xff)
: _forcedOff(false), _lastValue(0xff)
{

}


void MC34844::setup(I2CController *i2c, GPIOPin chipSelect, GPIOPin pwmPin, uint16_t channelMask, ovp_t ovp, uint8_t address)
{
	_i2c = i2c;
	_chipSelect = chipSelect;
	_pwmPin = pwmPin;
	_address = address;

	_reg8 = (channelMask & 0x001f);
	_reg9 = (channelMask & 0x03e0) >> 5;

	_ovp = ovp;

	for (int i=0; i<10; i++) {
		bool state = channelMask&(1<<i);
		_channelStates[i] = state;
	}

	this->init();

}


void MC34844::init()
{
	_chipSelect.configureAsOutput();
	_chipSelect.setHigh();
	_pwmPin.configureAsOutput();
	_pwmPin.setLow();

	setRegisterValue(0x01, 0x01); // Enable I2C

	this->initRegisters();

	_chipSelect.setLow();
}


void MC34844::initRegisters()
{
	uint8_t ovp;

	ovp = (_ovp + 2)<<4;

	_pwmPin.setLow();

	setRegisterValue(0x00, ovp); // OVP

	setRegisterValue(0x14, 0x02); // BST

	// set channel usage MASK
	setRegisterValue(0x08, _reg8);
	setRegisterValue(0x09, _reg9);

	setRegisterValue(0x00, ovp | 0x01);

	_chipSelect.setLow();
	_pwmPin.setHigh();

	setDimmungGlobal(0x00);
}


int MC34844::setDimmungGlobal(uint8_t value)
{

	if (value == _lastValue) { return 0; }
	else { _lastValue = value; }

	_pwmPin.setLow();

	//if ((_chipSelect == 0) || ( _pwmPin == 0)) {
	if ((!_chipSelect.isValid()) || ( !_pwmPin.isValid())) {
		return -1;
	}

	_chipSelect.setHigh();

	setRegisterValue(0xFA, value);

	_chipSelect.setLow();

	if ((value != 0) && (!_forcedOff)) _pwmPin.setHigh();
	else _pwmPin.setLow();


	return 0;

}


int MC34844::setDimmungChannel(uint8_t channel, uint8_t value)
{

	if (channel >= 10) {
		return -1;
	}


	if ( (_chipSelect.isValid()) &&  (channel <= 9)) {

		if (value == 0) {
			if ( isChannelEnabled(channel)) {
				_chipSelect.setHigh();
				disableChannel(channel);
				_chipSelect.setLow();
			}
		}
		else {
			_chipSelect.setHigh();
			bool pwmPinOldState = _pwmPin.isHigh();
			_pwmPin.setLow();
			if (isChannelDisabled(channel)) {
				enableChannel(channel);
			}
			setRegisterValue(0xF0 + channel, value);
			_chipSelect.setLow();
			_pwmPin.set(pwmPinOldState);
		}

		return 0;
	}

	return -1;

}


void MC34844::forceOff(bool off)
{
	if (off) {
		_pwmPin.setLow();
	}

	_forcedOff = off;
}


void MC34844::disableChannel(uint8_t channel)
{
	if (channel >= 10) {
		return;
	}


	if ( _channelStates[channel] ) {
		_channelStates[channel] = false;

		bool pwmPinOldState = _pwmPin.isHigh();
		_pwmPin.setLow();

		// set MSB of reg8 -> POWER_OFF
		setRegisterValue(0x08, 0x80|_reg8);

		// write CHENx in reg8 and reg9
		if (channel <= 4) {
			_reg8 &= ~(1<<channel);
			setRegisterValue(0x08, 0x80|_reg8);
		}
		else {
			channel -= 5;
			_reg9 &= ~(1<<channel);
			setRegisterValue(0x09, _reg9);
		}

		// clear MSB of reg8 -> POWER_OFF
		setRegisterValue(0x80, _reg8);

		_pwmPin.set(pwmPinOldState);
	}
}


void MC34844::enableChannel(uint8_t channel)
{
	if (channel >= 10) {
		return;
	}

	if ( ! _channelStates[channel] ) {
		_channelStates[channel] = true;

		bool pwmPinOldState = _pwmPin.isHigh();
		_pwmPin.setLow();

		// set MSB of reg9 -> POWER_ON
		setRegisterValue(0x09, 0x80|_reg9);

		// write CHENx in reg8 and reg9
		if (channel <= 4) {
			_reg8 |= (1<<channel);
			setRegisterValue(0x08, _reg8);
		}
		else {
			channel -= 5;
			_reg9 |= (1<<channel);
			setRegisterValue(0x09, 0x80|_reg9);
		}

		// clear MSB of reg9 -> POWER_ON
		setRegisterValue(0x09, _reg9);

		_pwmPin.set(pwmPinOldState);

	}
}


void MC34844::setRegisterValue(uint8_t reg, uint8_t value)
{
	uint8_t data[2];

	data[0] = reg;
	data[1] = value;
	_i2c->write(_address, data, 2, true, true);

}
