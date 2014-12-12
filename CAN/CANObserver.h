/*
 * CANObserver.h
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


#ifndef CANOBSERVER_H_
#define CANOBSERVER_H_

#include <openstella/OS/Queue.h>
#include <stdint.h>
#include "../CAN.h"
#include "CANMessage.h"


class CANMessageNotifyObject;

/**
 * @class CANObserver
 * @brief base class for can message receivers
 *
 * Derive from CANObserver and call CANObserver::listenCAN() to receive CAN messages.
 * The corresponding CAN Controller also has to be set up to receive messages.
 *
 * Call isCANMessageAvailable() or getCANMessage() regularly,
 * e.g. in a Task's main loop, to process incoming messages.
 *
 * \include "CANecho.h"
 *
 */

class CANObserver
{
	friend class CANController;

	private:
		Queue<CANMessage*> _queue;
		bool notifyCANMessage(CANMessage *obj);

	public:
		/** call this from your derived classes constructor.
		 * @param queueSize length of the queue (in messages)
		 *        that will be buffered by this observer.
		 *        if this queue runs full, messages will be thrown away.
		 */
		CANObserver(uint8_t queueSize=10);

		/// listen to all CAN messages on a channel
		/**
		 * @param channel the can channel to listen to.
		 *        the channel has to be setup() to be able to acutally receive messages.
		 *
		 * \warning
		 * Receiving all messages a busy CAN bus can result in high cpu load.
		 * Consider using filters via listenCANId() if you expect a high traffic CAN bus.
		 */
		void listenCAN(CAN::channel_t channel);
		void listenCAN(CANController *can);

		/// listen to certain CAN messages on a channel
		/**
		 * @param channel the can channel to listen to.
		 *        the channel has to be setup() to be able to acutally receive messages.
		 * @param id the can id to listen to.
		 * @param mask the mask that applies to the id for the filter function.
		 */
		void listenCANId(CAN::channel_t channel, int32_t id, uint32_t mask=0xFFFFFFFF);
		void listenCANId(CANController *can, int32_t id, uint32_t mask=0xFFFFFFFF);

		/// check whether a CAN message waits in the queue
		/**
		 * @return returns true if the CAN message queue is not empty
		 */
		bool isCANMessageAvailable();

		/// fetch a CAN message from the queue
		/**
		 * @param [out] msg address of a CANMessage object to store the data
		 * @param timeout time in milliseconds to wait for new messages if no message is waiting in the queue.\n
		 *        use timeout=0 for non-blocking operation
		 * @result returns true if a new message was received. returns false if a timeout occured.
		 */
		bool getCANMessage(CANMessage *msg, uint32_t timeout=0xFFFFFFFF);

		void removeListenCAN(CAN::channel_t channel);
		void removeListenCAN(CANController *can);
		void removeListenCANId(CAN::channel_t channel, int32_t id, uint32_t mask=0xFFFFFFFF);
		void removeListenCANId(CANController *can, int32_t id, uint32_t mask=0xFFFFFFFF);

};

#endif /* CANOBSERVER_H_ */
