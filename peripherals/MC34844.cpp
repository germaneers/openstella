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


void MC34844::setup(I2CController *i2c, GPIOPin chipSelect, GPIOPin pwmPin, uint8_t numChannels, uint8_t address)
{
	_i2c = i2c;
	_chipSelect = chipSelect;
	_pwmPin = pwmPin;
	_address = address;
	_numChannels = numChannels;

	this->init();
}


void MC34844::init()
{
	uint8_t data[2];

	data[0] = 0x01;
	data[1] = 0x01;

	_i2c->write(0x76, data, 2, true, true);

	_pwmPin.enablePeripheral();
	_chipSelect.enablePeripheral();

	_pwmPin.configureAsOutput();
	_pwmPin.setLow();

	_chipSelect.configureAsOutput();
	_chipSelect.setHigh();

	_chipSelect.setLow();

	this->initRegisters();
}


void MC34844::initRegisters()
{
	uint8_t data[2];

	_pwmPin.setLow();

	data[0] = 0x00;  // OVP
	data[1] = 0xF0;
	_i2c->write(0x76, data, 2, true, true);

	data[0] = 0x14;  // BST
	data[1] = 0x02;
	_i2c->write(0x76, data, 2, true, true);

	data[0] = 0x08;  // CHEN
	data[1] = 0x1F;
	_i2c->write(0x76, data, 2, true, true);

	data[0] = 0x09;  // CHEN
	if ( _numChannels == 9 ) {
		data[1] = 0x0f;
	}
	else if ( _numChannels == 6 ){
		data[1] = 0x01;
	}
	else { // TODO: implement more than 6 or 9 channels
		while(1) {;}
	}
	_i2c->write(0x76, data, 2, true, true);

	data[0] = 0x00;  // EnBit
	data[1] = 0xF1;
	_i2c->write(0x76, data, 2, true, true);


	_pwmPin.setHigh();
	_chipSelect.setLow();

	setDimmung(0x00);
}


int MC34844::setDimmung(uint8_t value)
{
	uint8_t data[2];
	int ret;

	if (value == _lastValue) { return 0; }
	else { _lastValue = value; }

	//if ((_chipSelect == 0) || ( _pwmPin == 0)) {
	if ((!_chipSelect.isValid()) || ( !_pwmPin.isValid())) {
		return -1;
	}

	_chipSelect.setHigh();

	data[0] = 0xFA; // Global current program register
	data[1] = value;
	ret = _i2c->write(_address, data, 2, true, true);

	if ((value != 0) && (!_forcedOff)) _pwmPin.setHigh();
	else _pwmPin.setLow();

	_chipSelect.setLow();

	return ret;

}


void MC34844::forceOff(bool off)
{
	if (off) {
		_pwmPin.setLow();
	}

	_forcedOff = off;
}


