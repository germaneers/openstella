/*
 * CANMessage.cpp
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

#include "CANMessage.h"

#include <string.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_can.h>
#include <StellarisWare/driverlib/can.h>

CANMessage::CANMessage(uint32_t id, uint8_t dlc)
  : _flags(0), id(id), dlc(dlc)
{
	memset(data, 0, 8);
	setExtendedId(id>=0x800);
}


bool CANMessage::isExtendedId()
{
	return (_flags & MSG_OBJ_EXTENDED_ID) != 0;
}

void CANMessage::setExtendedId(bool extended)
{
	if (extended) {
		_flags |= MSG_OBJ_EXTENDED_ID;
	} else {
		_flags &= ~MSG_OBJ_EXTENDED_ID;
	}
}

bool CANMessage::isRemoteFrame()
{
	return (_flags & MSG_OBJ_REMOTE_FRAME) != 0;
}

void CANMessage::setRemoteFrame(bool remote)
{
	if (remote) {
		_flags |= MSG_OBJ_REMOTE_FRAME;
	} else {
		_flags &= ~MSG_OBJ_REMOTE_FRAME;
	}
}

void CANMessage::assign(const CANMessage *msg)
{
	_flags = msg->_flags;
	id = msg->id;
	dlc = msg->dlc;
	memcpy(data, msg->data, sizeof(data));
}

