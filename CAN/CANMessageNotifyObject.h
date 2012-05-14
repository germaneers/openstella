/*
 * CANMessageNotifyObject.h
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


#ifndef CANMESSAGENOTIFYOBJECT_H_
#define CANMESSAGENOTIFYOBJECT_H_

#include <stdint.h>
#include "CANMessage.h"
#include "CANMessageNotifyObject.h"
#include "openstella/CAN.h"

class CANMessageNotifyObject : public CANMessage {
	friend class CANController;

	private:
		CAN::channel_t _channel;
		int8_t _referenceCounter;

	public:
		CANMessageNotifyObject();
		CANMessageNotifyObject(CAN::channel_t channel, CANMessage *msg, uint8_t referenceCounter);
		CAN::channel_t getChannel();
		void setReadyForGC();

};

#endif /* CANMESSAGENOTIFYOBJECT_H_ */
