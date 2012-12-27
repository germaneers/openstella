/*
 * CANMessage.h
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

#ifndef CANMESSAGE_H_
#define CANMESSAGE_H_

#include <stdint.h>
#include "CANtypes.h"
/**
 * @class CANMessage
 * @brief data structure for CAN Messages
 */
class CANController;

class CANMessage {
	friend class CANMessageObject;
	protected:
		uint8_t _flags;
		CANController *_receivingController;
	public:

		/// the CAN ID of the message
		uint32_t id;

		/// the CAN DLC of the message
		uint8_t  dlc;

		/// message data. only valid in range [0..dlc-1]
		uint8_t  data[8];

    	CANMessage(uint32_t id=0, uint8_t dlc=0);

		/// is the message an extended message?
		bool isExtendedId();
		/// set the extended protocol flag
		void setExtendedId(bool extended);

		/// is the message a remote frame?
		bool isRemoteFrame();
		/// set the remote frame flag
		void setRemoteFrame(bool remote);

		/// assign all data from another CAN message
		/** @param msg the source message */
		void assign(const CANMessage *msg);

		CAN::channel_t getReceivingChannel();
		CANController *getReceivingController() const;
		void setReceivingController(CANController *_receivingController);

		uint64_t extractSignal(uint8_t startBit, uint8_t bitCount);

		void returnMessageToPool();
};

#endif /* CANMESSAGE_H_ */
