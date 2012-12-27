/*
 * MMA8453Q.cpp
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

#include "MMA8453Q.h"
#include "../OS/RecursiveMutex.h"

MMA8453Q::MMA8453Q(I2CController *i2c, bool SA0):
	_readAddr(SA0 ? 0x9D : 0x9C),
	_writeAddr(SA0 ? 0x1D : 0x1C),
	_i2c(i2c),
	_x(0), _y(0), _z(0)
{
}

void MMA8453Q::set8bitRegister(uint8_t reg, uint8_t value)
{
	_i2c->write16(_writeAddr, reg<<8 | value, I2CController::byteorder_big_endian, true, true);
}

uint8_t MMA8453Q::read8bitRegister(uint8_t reg)
{
	uint8_t result;
	RecursiveMutexGuard(_i2c->getLock());
	_i2c->write8(_writeAddr, reg, true, false);
	_i2c->read8(_readAddr, &result, true, true);
	return result;
}

uint8_t MMA8453Q::read16bitRegister(uint8_t reg)
{
	uint16_t result;
	RecursiveMutexGuard(_i2c->getLock());
	_i2c->write8(_writeAddr, reg, true, false);
	_i2c->read16(_readAddr, &result, I2CController::byteorder_big_endian, true, true);
	return result;
}

bool MMA8453Q::checkWhoAmI()
{
	return read8bitRegister(0x0D)==0x3A;
}

void MMA8453Q::setRange(MMA8453Q::RangeMode_t range)
{
	while (read8bitRegister(0x0E) != range)
	{
		set8bitRegister(0x0E, range);
	}
}

void MMA8453Q::setActive(bool isActive)
{
	RecursiveMutexGuard(_i2c->getLock());
	uint8_t reg = read8bitRegister(0x2A);
	if (isActive) {
		reg |= 0x01;
	} else {
		reg &= ~0x01;
	}
	set8bitRegister(0x2A, reg);
}

void MMA8453Q::setLowNoiseMode(bool isLowNoiseModeActive)
{
	RecursiveMutexGuard(_i2c->getLock());
	uint8_t reg = read8bitRegister(0x2A);
	if (isLowNoiseModeActive) {
		reg |= 0x04;
	} else {
		reg &= ~0x04;
	}
	set8bitRegister(0x2A, reg);
}

void MMA8453Q::updatePosition()
{
	RecursiveMutexGuard(_i2c->getLock());
	uint8_t buf[6];
	_i2c->write8(_writeAddr, 0x01, true, false);
	_i2c->read(_readAddr, buf, 6, true, true);
	_x = (buf[0]<<2) | (buf[1]>>6);
	_y = (buf[2]<<2) | (buf[3]>>6);
	_z = (buf[4]<<2) | (buf[5]>>6);
}

uint16_t MMA8453Q::getLastX()
{
	return _x;
}

uint16_t MMA8453Q::getLastY()
{
	return _y;
}

uint16_t MMA8453Q::getLastZ()
{
	return _z;
}

uint16_t MMA8453Q::getCurrentX()
{
	uint16_t value = read16bitRegister(0x01);
	_x = ((value >> 6) & 0x3FC) | (value & 0x03);
	return _x;
}

uint16_t MMA8453Q::getCurrentY()
{
	uint16_t value = read16bitRegister(0x01);
	_y = ((value >> 6) & 0x3FC) | (value & 0x03);
	return _y;
}

uint16_t MMA8453Q::getCurrentZ()
{
	uint16_t value = read16bitRegister(0x01);
	_z = ((value >> 6) & 0x3FC) | (value & 0x03);
	return _z;
}

int16_t MMA8453Q::getLastXsigned()
{
	int32_t v = getLastX();
	if (v<512) {
		return v;
	} else {
		return 0 - (1024 - v);
	}
}

int16_t MMA8453Q::getLastYsigned()
{
	int32_t v = getLastY();
	if (v<512) {
		return v;
	} else {
		return 0 - (1024 - v);
	}
}

int16_t MMA8453Q::getLastZsigned()
{
	int32_t v = getLastZ();
	if (v<512) {
		return v;
	} else {
		return 0 - (1024 - v);
	}
}

