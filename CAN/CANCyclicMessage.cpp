/*
 * CANCyclicMessage.h
 *
 * Copyright 2012 Germaneers GmbH
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

#include "CANCyclicMessage.h"
#include "CANController.h"
#include <string.h>

CANCyclicMessage::CANCyclicMessage(CANMessage *msg, uint16_t interval, uint16_t offset) :
	_msg(msg),
	_interval(interval),
	_offset(offset),
	_enabled(true)
{
}

CANCyclicMessage::CANCyclicMessage(uint32_t id, uint8_t dlc, uint16_t interval, uint16_t offset) :
	_interval(interval),
	_offset(offset),
	_enabled(true)
{
	_msg = new CANMessage(id, dlc);
}

CANCyclicMessage::CANCyclicMessage(CANController *can, uint32_t id, uint8_t dlc, uint16_t interval, uint16_t offset) :
	_interval(interval),
	_offset(offset),
	_enabled(true)
{
	_msg = new CANMessage(id, dlc);
	can->registerCyclicMessage(this);
}




bool CANCyclicMessage::shouldBeSentAt(uint32_t timestamp)
{
	return _enabled && ((timestamp % _interval) == _offset);
}



uint32_t CANCyclicMessage::timeTillNextSend(uint32_t timestamp)
{
	if (!_enabled) return 0xFFFFFFFF;
	if ( _offset > timestamp ) {
		return _offset - timestamp;
	} else {
		return _interval - ((timestamp - _offset) % _interval);
	}
}



void CANCyclicMessage::send(CANController *can)
{
	if (_enabled) can->sendMessage(_msg);
}



void CANCyclicMessage::setInterval(uint16_t interval)
{
	RecursiveMutexGuard guard(&_lock);
	_interval = interval;
}



uint16_t CANCyclicMessage::getInterval()
{
	RecursiveMutexGuard guard(&_lock);
	return _interval;
}



void CANCyclicMessage::setOffset(uint16_t offset)
{
	RecursiveMutexGuard guard(&_lock);
	_offset = offset;
}



uint16_t CANCyclicMessage::getOffset()
{
	RecursiveMutexGuard guard(&_lock);
	return _offset;
}



void CANCyclicMessage::setId(uint32_t id)
{
	RecursiveMutexGuard guard(&_lock);
	_msg->id = id;
}



uint32_t CANCyclicMessage::getId()
{
	RecursiveMutexGuard guard(&_lock);
	return _msg->id;
}



void CANCyclicMessage::setDlc(uint8_t dlc)
{
	RecursiveMutexGuard guard(&_lock);
	_msg->dlc = dlc;
}



uint32_t CANCyclicMessage::getDlc()
{
	RecursiveMutexGuard guard(&_lock);
	return _msg->dlc;
}



void CANCyclicMessage::setData(uint8_t data[])
{
	RecursiveMutexGuard guard(&_lock);
	memcpy(_msg->data, data, 8);
}



void CANCyclicMessage::setData(CANMessage *msg)
{
	RecursiveMutexGuard guard(&_lock);
	memcpy(_msg->data, msg->data, 8);
}



void CANCyclicMessage::setData(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
	RecursiveMutexGuard guard(&_lock);
	_msg->data[0] = d0;
	_msg->data[1] = d1;
	_msg->data[2] = d2;
	_msg->data[3] = d3;
	_msg->data[4] = d4;
	_msg->data[5] = d5;
	_msg->data[6] = d6;
	_msg->data[7] = d7;
}



void CANCyclicMessage::setDataByte(uint8_t index, uint8_t value)
{
	RecursiveMutexGuard guard(&_lock);
	if (index<8) {
		_msg->data[index] = value;
	}
}


uint8_t CANCyclicMessage::getDataByte(uint8_t index)
{
	RecursiveMutexGuard guard(&_lock);
	if (index<8) {
		return _msg->data[index];
	} else {
		return 0;
	}
}



bool CANCyclicMessage::isExtendedId()
{
	RecursiveMutexGuard guard(&_lock);
	return _msg->isExtendedId();
}



void CANCyclicMessage::setExtendedId(bool extended)
{
	RecursiveMutexGuard guard(&_lock);
	_msg->setExtendedId(extended);
}



bool CANCyclicMessage::isRemoteFrame()
{
	RecursiveMutexGuard guard(&_lock);
	return _msg->isRemoteFrame();
}



void CANCyclicMessage::setRemoteFrame(bool remote)
{
	RecursiveMutexGuard guard(&_lock);
	_msg->setRemoteFrame(remote);
}
