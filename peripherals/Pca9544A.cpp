/*
 * Pca9544A.cpp
 *
 * Copyright 2013 Germaneers GmbH
 * Copyright 2013 Stefan Rupp (stefan.rupp@germaneers.com)
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



#include "Pca9544A.h"
#include "Pca9544aI2cChannel.h"



Pca9544A::Pca9544A()
: _activeBus(Pca9544A_CHANNEL_NONE), _initialized(false)
{
	for (int i=0; i<4; i++) {
		_i2cChannels[i] = 0;
	}
}


void Pca9544A::setup(I2CController* i2c, uint8_t i2cAddress)
{
	_lock = i2c->getLock();
	RecursiveMutexGuard guard(_lock);

	_i2c = i2c;
	_i2cAddress = i2cAddress;
	_activeBus = Pca9544A_CHANNEL_NONE;

	_i2cChannels[0] = new Pca9544AI2cChannel(Pca9544A_CHANNEL0, this);
	_i2cChannels[1] = new Pca9544AI2cChannel(Pca9544A_CHANNEL1, this);
	_i2cChannels[2] = new Pca9544AI2cChannel(Pca9544A_CHANNEL2, this);
	_i2cChannels[3] = new Pca9544AI2cChannel(Pca9544A_CHANNEL3, this);

	_initialized = true;
}


bool Pca9544A::writeToBus(Pca9544aI2cChannel_t bus, uint8_t i2cAddress, const void* data, int len, bool sendStart, bool sendStop)
{
	RecursiveMutexGuard guard(_lock);

	bool ret=false;


	if ( ! _initialized ) {
		return false;
	}

	if (i2cAddress == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	if ( selectBus(bus) ) {
		ret = _i2c->write(i2cAddress, data, len, sendStart, sendStop);
	}

	return ret;
}


bool Pca9544A::readFromBus(Pca9544aI2cChannel_t bus, uint8_t i2cAddress, void* data, int len, bool sendStart, bool sendStop)
{
	RecursiveMutexGuard guard(_lock);

	bool ret=false;

	if ( ! _initialized ) {
		return (false);
	}

	if (i2cAddress == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	if ( selectBus(bus) ) {
		ret = _i2c->read(i2cAddress, data, len, sendStart, sendStop);
	}


	return ret;
}

/*
Pca9544AI2cChannel* Pca9544A::getChannel(Pca9544aI2cChannel_t channel)
{
}
*/

Pca9544A::~Pca9544A()
{
}


bool Pca9544A::selectBus(Pca9544aI2cChannel_t bus)
{
	RecursiveMutexGuard guard(_lock);

	bool ret = true;

	if ( _initialized ) {

		if (bus != _activeBus) {
			uint8_t data[2];
			data[0] = 0x4 + bus;
			if ( _i2c->write(_i2cAddress, data, 1, true, true) == 0) {
				_activeBus = bus;
				ret = true;
			}
			else {
				_activeBus = Pca9544A_CHANNEL_NONE;
				ret = false;
			}
		}
		else {
			ret = true;
		}
	}
	else {
		_activeBus = Pca9544A_CHANNEL_NONE;
		ret = true;
	}

	return ret;
}


#if 1
unsigned long Pca9544A::read(uint8_t addr, void* buf, int count,
		bool sendStartCondition, bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->read(addr, buf, count, sendStartCondition, sendStopCondition);
}

unsigned long Pca9544A::read8(uint8_t addr, uint8_t* data,
		bool sendStartCondition, bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->read8(addr, data, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::write(uint8_t addr, const void* buf, int count,
		bool sendStartCondition, bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write(addr, buf, count, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::write8(uint8_t addr, uint8_t data,
		bool sendStartCondition, bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write8(addr, data, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::writeRead(uint8_t addr, uint8_t* writeBuf,
		uint8_t writeCount, uint8_t* readBuf, uint8_t readCount,
		bool sendStartCondition, bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->writeRead(addr, writeBuf, writeCount, readBuf, readCount, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::write8read(uint8_t addr, uint8_t writeData,
		uint8_t* readBuf, int readCount, bool sendStartCondition,
		bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write8read(addr, writeData, readBuf, readCount, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::write8read8(uint8_t addr, uint8_t data_w,
		uint8_t* data_r, bool sendStartCondition, bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write8read8(addr, data_w, data_r, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::read16(uint8_t addr, uint16_t* data,
		AbstractI2cController::byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
	return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->read16(addr, data,	byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::read32(uint8_t addr, uint32_t* data,
		AbstractI2cController::byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->read32(addr, data, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::write16(uint8_t addr, uint16_t data,
		AbstractI2cController::byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write16(addr, data, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::write32(uint8_t addr, uint32_t data,
		AbstractI2cController::byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write32(addr, data, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::write8read16(uint8_t addr, uint8_t data_w,
		uint16_t* data_r, AbstractI2cController::byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write8read16(addr, data_w, data_r, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::write8read32(uint8_t addr, uint8_t data_w,
		uint32_t* data_r, AbstractI2cController::byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write8read32(addr, data_w,	data_r, byteorder, sendStartCondition, sendStopCondition);

}


unsigned long Pca9544A::write16read(uint8_t addr, uint16_t writeData,
		uint8_t* readBuf, int readCount, AbstractI2cController::byteorder_t byteorder,
		bool sendStartCondition, bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write16read(addr, writeData, readBuf, readCount, byteorder, sendStartCondition, sendStopCondition);

}


unsigned long Pca9544A::write16read8(uint16_t addr, uint16_t data_w,
		uint8_t* data_r, AbstractI2cController::byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write16read8(addr, data_w, data_r, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::write16read16(uint16_t addr, uint16_t data_w,
		uint16_t* data_r, AbstractI2cController::byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write16read16(addr, data_w, data_r, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544A::write16read32(uint16_t addr, uint16_t data_w,
		uint32_t* data_r, AbstractI2cController::byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	if ( !_initialized || _activeBus == Pca9544A_CHANNEL_NONE) {
		return -1;
	}

	if (addr == _i2cAddress) {
		while (1); // error! can't write to a device that has our own address!
	}

	return _i2c->write16read32(addr, data_w, data_r, byteorder, sendStartCondition, sendStopCondition);
}
#endif



