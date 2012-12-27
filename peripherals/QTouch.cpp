/*
 * QTouch.cpp
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

#include "QTouch.h"

#include "../OS/Task.h"

#include "assert.h"

#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/driverlib/rom_map.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/rom.h>

QTouch::QTouch(SPIController *spi, GPIOPin enablePin):
	_hasEnablePin(true), _spi(spi), _enablePin(enablePin)
{
//	_delay_150us = ROM_SysCtlClockGet() / 10000; // 300us
	_delay_150us = MAP_SysCtlClockGet() / 20000; // 150us
	enablePin.enablePeripheral();
	enablePin.configureAsOutput();
	enablePin.setHigh();
}

QTouch::QTouch(SPIController *spi):
	_hasEnablePin(false), _spi(spi), _enablePin(GPIO::A[0])
{
	_delay_150us = MAP_SysCtlClockGet() / 10000; // 300us
}

uint8_t QTouch::writeReadOneByte(uint8_t write_data)
{
	uint8_t result;
	if (_hasEnablePin) _enablePin.setLow();
	SysCtlDelay(_delay_150us);
	result = _spi->writeAndReadBlocking(write_data);
	if (_hasEnablePin) _enablePin.setHigh();
	return result;
}


void QTouch::configure(void)
{
	uint8_t defaultData[42] = {
		0xF2, 0x00, 0x38, 0x12, 0x06, 0x06, 0x12, 0x00, // byte 0 used to be: 0xB2
		0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
		0x32, 0xFF, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80,
		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x7A,
		0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A,
		0x7A, 0x7A
	};

/*	while (1) {
 		_enablePin.setLow();
		SysCtlDelay(_delay_150us);
		uint8_t answer = _spi->writeAndReadBlocking(0xD1);
		if (answer!=0x55) {
			uint8_t a = answer;
		}
		SysCtlDelay(_delay_150us);
		_spi->writeAndReadBlocking(0x00);
		SysCtlDelay(_delay_150us);
		_enablePin.setHigh();
		Task::delay_ms(1000);
	} */

	uint8_t x = sendCommandReadOneByte(0xD0); // read device mode
	if (x!=0xF2) { // not initialized / default config?
		if (_hasEnablePin) _enablePin.setLow();
		SysCtlDelay(_delay_150us);
		_spi->writeAndReadBlocking(0x01);
		for (uint8_t i=0; i<sizeof(defaultData); i++) {
			SysCtlDelay(_delay_150us);
			_spi->writeAndReadBlocking(defaultData[i]);
		}
		if (_hasEnablePin) _enablePin.setHigh();
		SysCtlDelay(_delay_150us);
		saveConfiguration();
	}

	uint8_t y = sendCommandReadOneByte(0xD8); // read AKS mask LSB
	if (y!=0x00) {
		while(1); // BAD ERROR?
	}

}

void QTouch::saveConfiguration()
{
	writeReadOneByte(0x0A);
	Task::delay_ms(250);
}

uint8_t QTouch::sendCommandReadOneByte(uint8_t cmd)
{
	uint8_t result;

	uint8_t answer;
	do {
		if (_hasEnablePin) {
			_enablePin.setHigh();
			Task::delay_ms(10);
			_enablePin.setLow();
		}
		SysCtlDelay(_delay_150us);
		answer = _spi->writeAndReadBlocking(cmd);
	} while (answer!=0x55);

	SysCtlDelay(_delay_150us);
	result = _spi->writeAndReadBlocking(0);

	if (_hasEnablePin) _enablePin.setHigh();
	return result;
}

uint16_t QTouch::sendCommandReadTwoBytes(uint8_t cmd)
{
	uint16_t result = 0xFFFF;
	if (_hasEnablePin) _enablePin.setLow();

	uint8_t answer;
	while(1) {
		SysCtlDelay(_delay_150us);
		answer = _spi->writeAndReadBlocking(cmd);
		if (answer==0x55) break;

		// command write failed. retry.
		if (_hasEnablePin) _enablePin.setHigh();
		Task::delay_ms(10);
		if (_hasEnablePin) _enablePin.setLow();
	}

	SysCtlDelay(_delay_150us);
	result = _spi->writeAndReadBlocking(0) << 8;
	SysCtlDelay(_delay_150us);
	result |= _spi->writeAndReadBlocking(0);

	if (_hasEnablePin) _enablePin.setHigh();
	return result;
}

uint16_t QTouch::getAllKeys(void)
{
	return sendCommandReadTwoBytes(0xC1);
/*
	uint16_t result = 0xFFFF;
	uint8_t b = writeReadOneByte(0xC1);
	if (b == 0x55) {
		// "Idle" - Fresh Command
		result = (writeReadOneByte(0) << 8) | writeReadOneByte(0);
	}
	return result; */
}

uint16_t QTouch::getKey(uint8_t key)
{
	assert(key<=10);
	return sendCommandReadTwoBytes(0x20+key);
}
