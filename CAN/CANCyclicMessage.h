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


#ifndef CANCYCLICMESSAGE_H_
#define CANCYCLICMESSAGE_H_

#include "../OS/RecursiveMutex.h"
#include "CANMessage.h"
class CANController;

/**
 * @class CANCyclicMessage
 * @brief class to comfortably send cyclic CAN messages
 *
 * Use CANCyclicMessages to send CAN messages at a fixed interval, e.g. every 100ms.\n
 * After registering the object via CANController::registerCyclicMessage(),
 * the CANController task takes care of sending the messages.
 *
 * CANCyclicMessage is thread-aware so you can safely access/change message data
 * from application threads.
 *
 * \include "CANcyclic.h"
 *
 */

class CANCyclicMessage {
	friend class CANController;
private:
	RecursiveMutex _lock;
	CANMessage *_msg;
	uint16_t _interval;
	uint16_t _offset;
	bool _enabled;
	uint32_t _timestamp_next_send;

public:
	void send(CANController *can);
	CANCyclicMessage(CANMessage *msg, uint16_t interval, uint16_t offset=0);

	/**
	 * @param id the can id of the transmitted message
	 * @param dlc DLC of the transmitted message
	 * @param interval interval the cyclic message is sent in (in ms)
	 * @param offset timing offset of the cyclic message (in ms)
	 */
	CANCyclicMessage(uint32_t id, uint8_t dlc, uint16_t interval, uint16_t offset=0);

	/**
	 * @param canCtrl CANController to which this message should be registered
	 * @param id the can id of the transmitted message
	 * @param dlc DLC of the transmitted message
	 * @param interval interval the cyclic message is sent in (in ms)
	 * @param offset timing offset of the cyclic message (in ms)
	 */
	CANCyclicMessage(CANController *can, uint32_t id, uint8_t dlc, uint16_t interval, uint16_t offset=0, bool isEnabled = true);

	/// (temporarily) disable transmission of the message.
	void disable() { _enabled = false; };

	/// (re-)enable transmission of the message. sending is enabled by default.
	void enable()  { _enabled = true; };

	bool isEnabled() { return _enabled; }

	/// set the interval
	void setInterval(uint16_t interval);

	/// get the currently set interval
	uint16_t getInterval();

	void setOffset(uint16_t offset);
	uint16_t getOffset();

	/// set the CAN ID of the transmitted message
	void setId(uint32_t id);
	/// get the CAN ID of the transmitted message
	uint32_t getId();

	/// set the CAN DLC of the transmitted message
	void setDlc(uint8_t dlc);
	/// get the CAN DLC of the transmitted message
	uint32_t getDlc();


	/// set the data bytes of the transmitted message
	void setData(uint8_t data[]);

	/// assign data bytes of the transmitted message from another CAN message
	void setData(CANMessage *msg);

	/// assign new data to the transmitted message
	void setData(uint8_t d0=0, uint8_t d1=0, uint8_t d2=0, uint8_t d3=0, uint8_t d4=0, uint8_t d5=0, uint8_t d6=0, uint8_t d7=0);

	/// assign data[index] of the transmitted message to value
	void setDataByte(uint8_t index, uint8_t value);

	/// set bit in data[index]
	void setDataBit(uint8_t byteIndex, uint8_t bitIndex, bool state);

	/// return data[index] of the transmitted message
	uint8_t getDataByte(uint8_t index);

	bool isExtendedId();
	void setExtendedId(bool extended);
	bool isRemoteFrame();
	void setRemoteFrame(bool remote);
};

#endif /* CANCYCLICMESSAGE_H_ */
