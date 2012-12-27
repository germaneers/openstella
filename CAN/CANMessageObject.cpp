/*
 * CANMessageObject.cpp
 *
 * Copyright 2011, 2012 Germaneers GmbH
 * Copyright 2011, 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
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


#include "CANController.h"

#include "CANMessageObject.h"

#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/driverlib/can.h>
#include <StellarisWare/driverlib/rom.h>
#include <StellarisWare/driverlib/rom_map.h>

CANMessageObject::CANMessageObject() :
	CANMessage::CANMessage(),
	_channel(CAN::channel_0),
	_mob_num(0),
	_mob_flags(0)
{
}

CANMessageObject::CANMessageObject(CAN::channel_t channel, uint8_t mob_num) :
	CANMessage::CANMessage(),
	_channel(channel),
	_mob_num(mob_num),
	_mob_flags(0)
{
}

uint32_t CANMessageObject::getBase()
{
	return CANController::get(_channel)->_base;
}

bool CANMessageObject::isTxIntEnabled()
{
	return (_mob_flags & MSG_OBJ_TX_INT_ENABLE) != 0;
}

void CANMessageObject::setTxIntEnabled(bool txIntEnabled)
{
	if (txIntEnabled) {
		_mob_flags |= MSG_OBJ_TX_INT_ENABLE;
	} else {
		_mob_flags &= ~MSG_OBJ_TX_INT_ENABLE;
	}
}

bool CANMessageObject::isRxIntEnabled()
{
	return (_mob_flags & MSG_OBJ_RX_INT_ENABLE) != 0;
}

void CANMessageObject::setRxIntEnabled(bool rxIntEnabled)
{
	if (rxIntEnabled) {
		_mob_flags |= MSG_OBJ_RX_INT_ENABLE;
	} else {
		_mob_flags &= ~MSG_OBJ_RX_INT_ENABLE;
	}
}

bool CANMessageObject::isUsingIdFilter()
{
	return (_mob_flags & MSG_OBJ_USE_ID_FILTER) != 0;
}

void CANMessageObject::setUseIdFilter(bool useFilter)
{
	if (useFilter) {
		_mob_flags |= MSG_OBJ_USE_ID_FILTER;
	} else {
		_mob_flags &= ~MSG_OBJ_USE_ID_FILTER;
	}
}

bool CANMessageObject::isUsingExtendedIdFilter()
{
	return (_mob_flags & MSG_OBJ_USE_EXT_FILTER) != 0;
}

void CANMessageObject::setUseExtendedIdFilter(bool useFilter)
{
	if (useFilter) {
		_mob_flags |= MSG_OBJ_USE_EXT_FILTER;
	} else {
		_mob_flags &= ~MSG_OBJ_USE_EXT_FILTER;
	}
}

bool CANMessageObject::isUsingDirFilter()
{
	return (_mob_flags & MSG_OBJ_USE_DIR_FILTER) != 0;
}

void CANMessageObject::setUseDirFilter(bool useFilter)
{
	if (useFilter) {
		_mob_flags |= MSG_OBJ_USE_DIR_FILTER;
	} else {
		_mob_flags &= ~MSG_OBJ_USE_DIR_FILTER;
	}
}

bool CANMessageObject::isNewData()
{
	return (_mob_flags & MSG_OBJ_NEW_DATA) != 0;
}

bool CANMessageObject::isDataLost()
{
	return (_mob_flags & MSG_OBJ_DATA_LOST) != 0;
}

bool CANMessageObject::isPartOfFIFO()
{
	return (_mob_flags & MSG_OBJ_FIFO) != 0;
}

void CANMessageObject::setPartOfFIFO(bool partOfFIFO)
{
	if (partOfFIFO) {
		_mob_flags |= MSG_OBJ_FIFO;
	} else {
		_mob_flags &= ~MSG_OBJ_FIFO;
	}
}

void CANMessageObject::clear()
{
	CANMessageClear(getBase(), _mob_num);
	get(false);
}

void CANMessageObject::setMobNum(uint8_t mobNum)
{
	_mob_num = mobNum;
}

void CANMessageObject::setChannel(CAN::channel_t channel)
{
	_channel = channel;
}


void CANMessageObject::get(bool clearPendingInterrupt)
{
	tCANMsgObject msgobj;
	msgobj.pucMsgData = data;
	MAP_CANMessageGet(getBase(), _mob_num, &msgobj, clearPendingInterrupt);
	id   = msgobj.ulMsgID;
	mask = msgobj.ulMsgIDMask;
	dlc  = msgobj.ulMsgLen;
	_flags = msgobj.ulFlags & (MSG_OBJ_EXTENDED_ID | MSG_OBJ_REMOTE_FRAME);
	_mob_flags = msgobj.ulFlags & (
		MSG_OBJ_TX_INT_ENABLE |
		MSG_OBJ_RX_INT_ENABLE |
		MSG_OBJ_USE_ID_FILTER |
		MSG_OBJ_USE_EXT_FILTER |
		MSG_OBJ_USE_DIR_FILTER  |
		MSG_OBJ_NEW_DATA  |
		MSG_OBJ_DATA_LOST |
		MSG_OBJ_FIFO
	);
}

void CANMessageObject::set(CAN::message_type_t msgtype)
{
	type = msgtype;
	tCANMsgObject msgobj;
	msgobj.ulMsgID = id;
	msgobj.ulMsgIDMask = mask;
	msgobj.ulMsgLen = dlc;
	msgobj.pucMsgData = data;
	msgobj.ulFlags = _flags | _mob_flags;
	MAP_CANMessageSet(getBase(), _mob_num, &msgobj, (tMsgObjType)msgtype);
}
