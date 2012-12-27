/*
 * CANController.h
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


#ifndef CANCONTROLLER_H_
#define CANCONTROLLER_H_

#include <stdint.h>

#include "openstella/OS/Task.h"
#include "openstella/OS/Queue.h"
#include "openstella/OS/Mutex.h"
#include "CANtypes.h"
#include "CANMessageObject.h"
#include "CANObserver.h"
#include "CANCyclicMessage.h"
#include "../generics/FragmentedList.h"
#include "../GPIO.h"
#include "CANMessagePool.h"

#ifdef PART_LM3S9B81
  #define HAS_CAN_CHANNEL_0
  #define HAS_CAN_CHANNEL_1
  #define HAS_CAN_CHANNEL_2
#endif
#ifdef PART_LM3S9B96
  #define HAS_CAN_CHANNEL_0
  #define HAS_CAN_CHANNEL_1
#endif
#ifdef PART_LM3S9D96
  #define HAS_CAN_CHANNEL_0
  #define HAS_CAN_CHANNEL_1
#endif
#ifdef PART_LM3S5G51
  #define HAS_CAN_CHANNEL_0
  #define HAS_CAN_CHANNEL_1
#endif

class CANMessageObject;
class CANObserver;

/**
 * @class CANController
 * @brief can controller hardware abstraction
 *
 * Use CANController to
 *  \li configure on-chip can controllers
 *	\li send messages
 *	\li send cyclic messages
 *	\li receive messages
 *
 * \include "CANecho.h"
 *
 */

class CANController : public Task
{
	friend void CAN0IntHandler();
	friend void CAN1IntHandler();
	friend void CAN2IntHandler();
	friend class CANMessageObject;

	private:

		static const uint8_t observer_list_length = 10;
		static const uint8_t cyclic_list_fragment_length = 10;

		typedef struct {
			int32_t can_id;
			uint32_t mask;
			CANObserver* observer;
		} observer_list_entry_t;

		struct observer_list_t {
			observer_list_entry_t entries[observer_list_length];
			observer_list_t* next;
		};

		FragmentedList<CANCyclicMessage,10> _cyclicMessages;

		static CANController *_controllers[3];

		CAN::channel_t _channel;
		uint32_t _periph;
		uint32_t _base;

		CANMessageObject _mobs[32];

		observer_list_t *_observers;
		Queue<uint8_t> _isrToThreadQueue;
		Mutex _observerMutex;
		uint32_t _lastMessageReceivedTimestamp;

		bool _silent;
		CANMessagePool _pool;

		CANController(CAN::channel_t channel, uint32_t periph, uint32_t base);
		void enableInterrupts(uint32_t interruptFlags);
		void disableInterrupts(uint32_t interruptFlags);
		void handleInterrupt();
		void notifyObservers(CANMessageObject *obj);

		observer_list_t *createObserverListFragment();

		uint32_t getControlRegister();
		uint32_t getTxRequestRegister();
		uint32_t getNewDataRegister();
		uint32_t getMobEnabledRegister();

		uint8_t findFreeSendingMOB();

		CANMessageObject* getMessageObject(uint8_t mob_id);

		uint32_t sendCyclicCANMessages();

	protected:
		/// the CAN Controller task routine
		/**
		 * \li receives messages from the CAN ISR
		 * \li dispatches messages to the CANObserver s
		 * \li garbage collects delivered CANMessages
		 * \li sends CANCyclicMessages
		 */
		virtual void execute();

	public:
		/// get CANController instance
		/**
		 * @param channel Number of the returned CANController (e.g. CAN::channel_0)
		 * @return the requested CANController object. newly constructed, if not requested before.
		 */
		static CANController* get(CAN::channel_t channel);

		/// setup can hardware
		/**
		 * after setup, the can controller has to be enabled to be fully functional. @see enable()
		 * @param bitrate the CAN bitrate for this channel, e.g. CAN::bitrate_500kBit.
		 * @param rxpin the GPIOPin for CAN-RX. the pin's GPIO Hardware will be enabled, initialized and mapped.
		 * @param txpin the GPIOPin for CAN-TX. the pin's GPIO Hardware will be enabled, initialized and mapped.
		 */
		void setup(CAN::bitrate_t bitrate, GPIOPin rxpin, GPIOPin txpin);
		void setBitrate(CAN::bitrate_t bitrate);

		/// enable the can controller and start the CANController Task
		void enable();

		/// disable the can controller and stop the CANController Task
		void disable();

		bool isAutomaticRetransmission();
		void setAutomaticRetransmission(bool retransmit);
		CAN::error_counters_t getErrorCounters();

		/// send a CANMessage object
		bool sendMessage(CANMessage *msg);

		/// send a empty message (DLC 0)
		/** @param id the CAN message id */
		bool sendMessage(uint32_t id);

		/// send a message with DLC 1
		/**
		 * @param id the CAN message id
		 * @param b1 the first (and only) data byte
		 */
		bool sendMessage(uint32_t id, uint8_t b1);

		/// send a message with DLC 2
		/**
		 * @param id the CAN message id
		 * @param b1 the first data byte
		 * @param b2 the second data byte
		 */
		bool sendMessage(uint32_t id, uint8_t b1, uint8_t b2);

		/// send a message with DLC 3
		/**
		 * @param id the CAN message id
		 * @param b1 the first data byte
		 * @param b2 the second data byte
		 * @param b3 the third data byte
		 */
		bool sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3);

		/// send a message with DLC 4
		/**
		 * @param id the CAN message id
		 * @param b1 the first data byte
		 * @param b2 the second data byte
		 * @param b3 the third data byte
		 * @param b4 the fourth data byte
		 */
		bool sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);

		/// send a message with DLC 5
		/**
		 * @param id the CAN message id
		 * @param b1 the first data byte
		 * @param b2 the second data byte
		 * @param b3 the third data byte
		 * @param b4 the fourth data byte
		 * @param b5 the fifth data byte
		 */
		bool sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5);

		/// send a message with DLC 6
		/**
		 * @param id the CAN message id
		 * @param b1 the first data byte
		 * @param b2 the second data byte
		 * @param b3 the third data byte
		 * @param b4 the fourth data byte
		 * @param b5 the fifth data byte
		 * @param b6 the sixth data byte
		 */
		bool sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6);

		/// send a message with DLC 7
		/**
		 * @param id the CAN message id
		 * @param b1 the first data byte
		 * @param b2 the second data byte
		 * @param b3 the third data byte
		 * @param b4 the fourth data byte
		 * @param b5 the fifth data byte
		 * @param b6 the sixth data byte
		 * @param b7 the seventh data byte
		 */
		bool sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7);

		/// send a message with DLC 8
		/**
		 * @param id the CAN message id
		 * @param b1 the first data byte
		 * @param b2 the second data byte
		 * @param b3 the third data byte
		 * @param b4 the fourth data byte
		 * @param b5 the fifth data byte
		 * @param b6 the sixth data byte
		 * @param b7 the seventh data byte
		 * @param b8 the eighth data byte
		 */
		bool sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7, uint8_t b8);

		/// send a message with specified DLC and data array
		/**
		 * @param id the CAN message id
		 * @param dlc length of message's payload
		 * @param data payload to send
		 */
		bool sendMessage(uint32_t id, uint8_t dlc, void const *data);

		bool registerObserver(CANObserver *observer);
		bool registerObserver(CANObserver *observer, int32_t can_id, uint32_t mask=0xFFFFFFFF);
		int unregisterObserver(CANObserver *observer);
		void returnMessageToPool(CANMessage *obj);

		/// create and register a CANCyclicMessage. deprecated.
		void registerCyclicMessage(CANMessage *msg, uint32_t interval);

		/// unregister a CANCyclicMessage by its CANMessage object. deprecated.
		void unregisterCyclicMessage(CANMessage *msg);

		/// register a CANCyclicMessage object
		void registerCyclicMessage(CANCyclicMessage *cmsg);

		/// unregister a CANCyclicMessage object
		void unregisterCyclicMessage(CANCyclicMessage *cmsg);

		/// get number of milliseconds since the last CAN message was received.
		uint32_t getTimeSinceLastReceivedMessage();

		/// don't actually send any messages (e.g. to respect network management)
		void setSilent(bool beSilent);

		/// get this controller's channel
		CAN::channel_t getChannel() { return _channel; }

};



#endif /* CANCONTROLLER_H_ */
