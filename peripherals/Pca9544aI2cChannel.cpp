/*
 * Pca9544aI2cChannel.cpp
 *
 *  Created on: 20.11.2013
 *      Author: ruppst
 */

#include "Pca9544aI2cChannel.h"



Pca9544AI2cChannel::Pca9544AI2cChannel(Pca9544A::Pca9544aI2cChannel_t channel, Pca9544A *pca9544a)
: _channel(channel), _pca9544a(pca9544a)
{
}


unsigned long Pca9544AI2cChannel::write8read(uint8_t addr, uint8_t writeData,
		uint8_t* readBuf, int readCount, bool sendStartCondition,
		bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write8read(addr, writeData, readBuf, readCount, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write8read8(uint8_t addr, uint8_t data_w,
		uint8_t* data_r, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write8read8(addr, data_w, data_r, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::read16(uint8_t addr, uint16_t* data,
		byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->read16(addr, data, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::read32(uint8_t addr, uint32_t* data,
		byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->read32(addr, data, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write16(uint8_t addr, uint16_t data,
		byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write16(addr, data, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write32(uint8_t addr, uint32_t data,
		byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write32(addr, data, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write8read16(uint8_t addr, uint8_t data_w,
		uint16_t* data_r, byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write8read16(addr, data_w, data_r, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write8read32(uint8_t addr, uint8_t data_w,
		uint32_t* data_r, byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write8read32(addr, data_w, data_r, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write16read(uint8_t addr, uint16_t writeData,
		uint8_t* readBuf, int readCount, byteorder_t byteorder,
		bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write16read(addr, writeData, readBuf, readCount, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write16read8(uint16_t addr, uint16_t data_w,
		uint8_t* data_r, byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write16read8(addr, data_w, data_r, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write16read16(uint16_t addr, uint16_t data_w,
		uint16_t* data_r, byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write16read16(addr, data_w, data_r, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write16read32(uint16_t addr, uint16_t data_w,
		uint32_t* data_r, byteorder_t byteorder, bool sendStartCondition,
		bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write16read32(addr, data_w, data_r, byteorder, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::read(uint8_t addr, void* buf, int count,
		bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->read(addr, buf, count, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::read8(uint8_t addr, uint8_t* data,
		bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->read8(addr, data, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write(uint8_t addr, const void* buf,
		int count, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write(addr, buf, count, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::write8(uint8_t addr, uint8_t data,
		bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->write8(addr, data, sendStartCondition, sendStopCondition);
}


unsigned long Pca9544AI2cChannel::writeRead(uint8_t addr, uint8_t* writeBuf,
		uint8_t writeCount, uint8_t* readBuf, uint8_t readCount,
		bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(_pca9544a->_lock);
	_pca9544a->selectBus(_channel);

	return _pca9544a->writeRead(addr, writeBuf, writeCount, readBuf, readCount, sendStartCondition, sendStopCondition);
}


Pca9544AI2cChannel::~Pca9544AI2cChannel()
{
}

