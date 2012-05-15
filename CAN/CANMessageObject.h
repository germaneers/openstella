/*
 * CANMessageObject.h
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

#ifndef CANMESSAGEOBJECT_H_
#define CANMESSAGEOBJECT_H_

#include <stdint.h>
#include "CANMessage.h"
#include "CANtypes.h"

class CANController;

class CANMessageObject : public CANMessage {
	friend class CANController;

	private:
		CAN::channel_t _channel;
		uint8_t _mob_num;
		uint16_t _mob_flags;

		CANMessageObject();
		CANMessageObject(CAN::channel_t channel, uint8_t mob_num);
		uint32_t getBase();

	public:
		CAN::message_type_t type;
		uint32_t mask;

		void setMobNum(uint8_t mobNum);
		void setChannel(CAN::channel_t channel);
		bool isTxIntEnabled();
		void setTxIntEnabled(bool txIntEnabled);
		bool isRxIntEnabled();
		void setRxIntEnabled(bool rxIntEnabled);

		bool isUsingIdFilter();
		void setUseIdFilter(bool useFilter);
		bool isUsingExtendedIdFilter();
		void setUseExtendedIdFilter(bool useFilter);
		bool isUsingDirFilter();
		void setUseDirFilter(bool useFilter);

		bool isNewData();
		bool isDataLost();

		bool isPartOfFIFO();
		void setPartOfFIFO(bool partOfFIFO);

		void clear();
		void get(bool clearPendingInterrupt=true);
		void set(CAN::message_type_t msgtype);

};

#endif /* CANMESSAGEOBJECT_H_ */
