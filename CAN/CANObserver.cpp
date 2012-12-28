/*
 * CANObserver.cpp
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

#include "CANObserver.h"


CANObserver::CANObserver(uint8_t queueSize) :
	_queue(queueSize)
{
}

void CANObserver::listenCAN(CAN::channel_t channel)
{
	CANController::get(channel)->registerObserver(this);
}

void CANObserver::listenCANId(CAN::channel_t channel, int32_t id, uint32_t mask)
{
	CANController::get(channel)->registerObserver(this, id, mask);
}

bool CANObserver::isCANMessageAvailable()
{
	return (_queue.messagesWaiting() > 0);
}

bool CANObserver::getCANMessage(CANMessage *msg, uint32_t timeout)
{
	CANMessage *obj;
	if (_queue.receive(&obj, timeout)) {
		msg->assign(obj);
		if (obj->decrementReferenceCounter()) {
			delete(obj);
		}
		return true;
	} else {
		return false;
	}
}

bool CANObserver::notifyCANMessage(CANMessage *obj)
{
	return _queue.sendToBack(obj, 0);
}
