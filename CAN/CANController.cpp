/*
 * CANController.cpp
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

#include <string.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_can.h>
#include <StellarisWare/inc/hw_ints.h>
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/interrupt.h>
#include <StellarisWare/driverlib/can.h>
#include <StellarisWare/driverlib/rom.h>
#include <StellarisWare/driverlib/rom_map.h>

#include "../OS/Mutex.h"

void CAN0IntHandler(void) {
	CANController::_controllers[0]->handleInterrupt();
}

void CAN1IntHandler(void) {
	CANController::_controllers[1]->handleInterrupt();
}

void CAN2IntHandler(void) {
	CANController::_controllers[2]->handleInterrupt();
}





CANController::CANController(CAN::channel_t channel, uint32_t periph, uint32_t base) :
	Task(0, 1000),
	_channel(channel),
	_periph(periph),
	_base(base),
	_isrToThreadQueue(32),
	_observerMutex(),
	_lastMessageReceivedTimestamp(0),
	_silent(false)
{
	static const char* tasknames[3] = { "can0", "can1", "can2" };
	setTaskName(tasknames[channel]);

	for (uint8_t i=0; i<32; i++)
	{
		_mobs[i].setReceivingController(this);
		_mobs[i].setChannel(channel);
		_mobs[i].setMobNum(i+1);
	}

	_observers = createObserverListFragment();
}

CANController::observer_list_t *CANController::createObserverListFragment()
{
	CANController::observer_list_t *result = new CANController::observer_list_t;
	memset(result, 0, sizeof(CANController::observer_list_t));
	return result;
}

void CANController::disableInterrupts(uint32_t interruptFlags)
{
	MAP_CANIntDisable(_base, interruptFlags);
}

void CANController::setBitrate(CAN::bitrate_t bitrate)
{
	MAP_CANBitRateSet(_base, MAP_SysCtlClockGet(), bitrate);
}

void CANController::setup(CAN::bitrate_t bitrate, GPIOPin rxpin, GPIOPin txpin)
{
	rxpin.getPort()->enablePeripheral();
	txpin.getPort()->enablePeripheral();

	rxpin.configure(GPIOPin::CAN);
	txpin.configure(GPIOPin::CAN);

	switch (_channel) {
#ifdef HAS_CAN_CHANNEL_0
		case CAN::channel_0:
			rxpin.mapAsCAN0RX();
			txpin.mapAsCAN0TX();
			break;
#endif
#ifdef HAS_CAN_CHANNEL_1
		case CAN::channel_1:
			rxpin.mapAsCAN1RX();
			txpin.mapAsCAN1TX();
			break;
#endif
#ifdef HAS_CAN_CHANNEL_2
		case CAN::channel_2:
			rxpin.mapAsCAN2RX();
			txpin.mapAsCAN2TX();
			break;
#endif
	}

	MAP_SysCtlPeripheralEnable(_periph);

	MAP_CANInit(_base);
	setBitrate(bitrate);

	enableInterrupts(CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

	for (int i=0; i<32; i++)
	{
		CANMessageObject *o = getMessageObject(i);
		o->id = 0;
		o->mask = 0;
		o->dlc = 8;
		o->setRxIntEnabled(i<10);
		o->setPartOfFIFO(i<9);
		o->setUseIdFilter(i<10);
		o->set(CAN::message_type_rx);
	}

}

void CANController::execute()
{
	CANMessageObject *obj;
	while(1) {

		uint8_t num;
		while (_isrToThreadQueue.receive(&num, 0))
		{
			obj = getMessageObject(num);
			obj->get(true);

			if (obj->type==CAN::message_type_rx)
			{
				_lastMessageReceivedTimestamp = getTime();
				notifyObservers(obj);
			}
		}

		uint32_t timeToWait = sendCyclicCANMessages();
		if (timeToWait>100) timeToWait = 100;

		_isrToThreadQueue.peek(&num, timeToWait);

	}
}


void CANController::handleInterrupt()
{
	while (uint32_t cause = MAP_CANIntStatus(_base, CAN_INT_STS_CAUSE))
	{
		if (cause == CAN_INT_INTID_STATUS) // status interrupt. error occurred?
		{
			uint32_t status = getControlRegister(); // also clears the interrupt
			// TODO: error handling

		} else if ( (cause >= 1) && (cause <= 32) ) // mailbox event
		{
			CANIntClear(_base, cause);
			_isrToThreadQueue.sendToBackFromISR(cause-1);
		}

	}
}

CANMessageObject* CANController::getMessageObject(uint8_t mob_id)
{
	if (mob_id<32)
	{
		return &_mobs[mob_id];
	} else {
		return 0;
	}
}

void CANController::enableInterrupts(uint32_t interruptFlags)
{
	unsigned long INT;
	switch (_channel) {
		case CAN::channel_0:
			INT = INT_CAN0;
			break;
		case CAN::channel_1:
			INT = INT_CAN1;
			break;
		case CAN::channel_2:
			INT = INT_CAN2;
			break;
		default:
			while(1);
	}
	MAP_CANIntEnable(_base, interruptFlags);
	MAP_IntPrioritySet(INT, configMAX_SYSCALL_INTERRUPT_PRIORITY);
	IntEnable(INT);
}

uint32_t CANController::getControlRegister()
{
	return MAP_CANStatusGet(_base, CAN_STS_CONTROL);
}

uint32_t CANController::getTxRequestRegister()
{
	return MAP_CANStatusGet(_base, CAN_STS_TXREQUEST);
}

uint32_t CANController::getNewDataRegister()
{
	return MAP_CANStatusGet(_base, CAN_STS_NEWDAT);
}

uint32_t CANController::getMobEnabledRegister()
{
	return MAP_CANStatusGet(_base, CAN_STS_MSGVAL);
}

void CANController::enable()
{
	MAP_CANEnable(_base);
	run();
}

void CANController::disable()
{
	stop();
	MAP_CANDisable(_base);
}

bool CANController::isAutomaticRetransmission()
{
	return MAP_CANRetryGet(_base);
}

void CANController::setAutomaticRetransmission(bool retransmit)
{
	MAP_CANRetrySet(_base, retransmit);
}

CAN::error_counters_t CANController::getErrorCounters()
{
	CAN::error_counters_t result;
	result.passiveLimitReached = MAP_CANErrCntrGet(_base, &result.rx_errors, &result.tx_errors);
	return result;
}

uint8_t CANController::findFreeSendingMOB()
{
	for (uint8_t i=0; i<100; i++) {
		uint32_t txPendingStatus = getTxRequestRegister();
		for (uint8_t i=31; i>15; i--) {
			if ( (txPendingStatus & (1<<i)) == 0 ) { // mailbox i is free?
				return i;
			}
		}
		delay_ms(1);
	}
	return 0;
}

bool CANController::sendMessage(CANMessage *msg)
{
	static Mutex lock;
	MutexGuard guard(&lock);

	if (_silent) return true;
	uint8_t mob = findFreeSendingMOB();
	if (mob>0) {
		CANMessageObject *o = getMessageObject(mob);
		if (msg->id>=0x800) {
			msg->setExtendedId(true);
		}
		o->assign(msg);
		o->mask = 0;
		o->set(CAN::message_type_tx);
		//wait for MB?

		return true;
	} else {
		return false;
	}

}

CANController *CANController::_controllers[3] = {0, 0, 0};
CANController *CANController::get(CAN::channel_t channel)
{
	static Mutex _mutex;
	MutexGuard guard(&_mutex);

	if (!_controllers[channel])
	{
		uint32_t base;
		uint32_t periph;
		void (*handler)(void);
		switch (channel) {

#ifdef HAS_CAN_CHANNEL_0
			case CAN::channel_0:
				base = CAN0_BASE;
				periph = SYSCTL_PERIPH_CAN0;
				handler = CAN0IntHandler;
				break;
#endif

#ifdef HAS_CAN_CHANNEL_1
			case CAN::channel_1:
				base = CAN1_BASE;
				periph = SYSCTL_PERIPH_CAN1;
				handler = CAN1IntHandler;
				break;
#endif

#ifdef HAS_CAN_CHANNEL_2
			case CAN::channel_2:
				base = CAN2_BASE;
				periph = SYSCTL_PERIPH_CAN2;
				handler = CAN2IntHandler;
				break;
#endif

			default:
				while(1);
		}
		_controllers[channel] = new CANController(channel, periph, base);
		CANIntRegister(base, handler);
	}

	return _controllers[channel];
}

void CANController::notifyObservers(CANMessageObject *obj)
{
	MutexGuard guard(&_observerMutex);

	observer_list_t *list = _observers;
	while (list != 0) {
		for (int i=0; i<observer_list_length; i++) {
			observer_list_entry_t *entry = &list->entries[i];
			if (!entry->observer) continue;

			if ( (obj->id & entry->mask) == (entry->can_id & entry->mask) )
			{
				// match. notify the observer.
				CANMessage *msg = _pool.getMessage();
				if (msg) {
					msg->assign(obj);
					if (!entry->observer->notifyCANMessage(msg)) {
						_pool.returnMessage(msg);
					}
				}

			}
		}
		list = list->next;
	}

}


void CANController::returnMessageToPool(CANMessage *obj)
{
	_pool.returnMessage(obj);
}

bool CANController::registerObserver(CANObserver *observer)
{
	return registerObserver(observer, 0, 0);
}

bool CANController::registerObserver(CANObserver *observer, int32_t can_id, uint32_t mask)
{
	MutexGuard guard(&_observerMutex);

	// don't register if same observer on same object is already installed
	observer_list_t *list = _observers;
	while (list != 0) {
		for (int i=0; i<observer_list_length; i++) {
			if ( (list->entries[i].observer == observer)
			  && (list->entries[i].can_id == can_id)
			  && (list->entries[i].mask == mask)
			) {
				return true; // already registered
			}
		}
		list = list->next;
	}

	list = _observers;
	while (list != 0) {
		for (int i=0; i<observer_list_length; i++) {
			if (list->entries[i].observer==0) { // found an empty slot
				list->entries[i].observer = observer;
				list->entries[i].can_id = can_id;
				list->entries[i].mask = mask;
				return true;
			}
		}
		if (list->next==0) {
			list->next = createObserverListFragment();
		}
		list = list->next;
	}
	return false;
}

int CANController::unregisterObserver(CANObserver *observer)
{
	MutexGuard guard(&_observerMutex);

	int result = 0;

	for (int i=0; i<0; i++)

	for (observer_list_t *list=_observers; list!=0; list=list->next) {
		for (int i=0; i<observer_list_length; i++) {
			if (list->entries[i].observer==observer) {
				list->entries[i].observer = 0;
				result++;
			}
		}
	}

	return result;
}


void CANController::registerCyclicMessage(CANCyclicMessage *cmsg)
{
	_cyclicMessages.addItem(cmsg);
}

void CANController::unregisterCyclicMessage(CANCyclicMessage *cmsg)
{
	_cyclicMessages.removeItem(cmsg);
}

void CANController::registerCyclicMessage(CANMessage *msg, uint32_t interval)
{
	CANCyclicMessage *cmsg = new CANCyclicMessage(msg, interval);
	_cyclicMessages.addItem(cmsg);
}

void CANController::unregisterCyclicMessage(CANMessage *msg)
{
	_cyclicMessages.lock();
	for (auto *list = _cyclicMessages._firstFragment; list!=0; list = list->next)
	{
		for (int i=0; i<_cyclicMessages._fragmentSize; i++) {
			CANCyclicMessage *item = list->items[i];
			if ((item!=0) && (item->_msg==msg)) {
				list->items[i] = 0;
			}
		}
	}
	_cyclicMessages.unlock();
}

uint32_t CANController::sendCyclicCANMessages()
{
	_cyclicMessages.lock();
	static uint32_t lastTickCount = 0;
	uint32_t now = getTime();
	uint32_t minTimeToWait = 1000;

	for (; lastTickCount <= now; lastTickCount++)
	{
		for (auto *list = _cyclicMessages._firstFragment; list!=0; list = list->next)
		{
			for (int i=0; i<_cyclicMessages._fragmentSize; i++) {
				CANCyclicMessage *item = list->items[i];
				if (item!=0) {

					if (item->shouldBeSentAt(lastTickCount)) {
						item->send(this);
					}

					if (lastTickCount==now) { // last iteration
						uint32_t t = item->timeTillNextSend(now);
						if (t < minTimeToWait) {
							minTimeToWait = t;
						}
					}

				}
			}

		}
	}

	_cyclicMessages.unlock();
	return minTimeToWait;
}

bool CANController::sendMessage(uint32_t id)
{
	CANMessage msg(id,0);
	return sendMessage(&msg);
}


bool CANController::sendMessage(uint32_t id, uint8_t b1)
{
	CANMessage msg(id, 1);
	msg.data[0] = b1;
	return sendMessage(&msg);
}



bool CANController::sendMessage(uint32_t id, uint8_t b1, uint8_t b2)
{
	CANMessage msg(id,2);
	msg.data[0] = b1;
	msg.data[1] = b2;
	return sendMessage(&msg);
}



bool CANController::sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3)
{
	CANMessage msg(id,3);
	msg.data[0] = b1;
	msg.data[1] = b2;
	msg.data[2] = b3;
	return sendMessage(&msg);
}



bool CANController::sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
	CANMessage msg(id,4);
	msg.data[0] = b1;
	msg.data[1] = b2;
	msg.data[2] = b3;
	msg.data[3] = b4;
	return sendMessage(&msg);
}



bool CANController::sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5)
{
	CANMessage msg(id,5);
	msg.data[0] = b1;
	msg.data[1] = b2;
	msg.data[2] = b3;
	msg.data[3] = b4;
	msg.data[4] = b5;
	return sendMessage(&msg);
}



bool CANController::sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6)
{
	CANMessage msg(id,6);
	msg.data[0] = b1;
	msg.data[1] = b2;
	msg.data[2] = b3;
	msg.data[3] = b4;
	msg.data[4] = b5;
	msg.data[5] = b6;
	return sendMessage(&msg);
}



bool CANController::sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7)
{
	CANMessage msg(id,7);
	msg.data[0] = b1;
	msg.data[1] = b2;
	msg.data[2] = b3;
	msg.data[3] = b4;
	msg.data[4] = b5;
	msg.data[5] = b6;
	msg.data[6] = b7;
	return sendMessage(&msg);
}

bool CANController::sendMessage(uint32_t id, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7, uint8_t b8)
{
	CANMessage msg(id,8);
	msg.data[0] = b1;
	msg.data[1] = b2;
	msg.data[2] = b3;
	msg.data[3] = b4;
	msg.data[4] = b5;
	msg.data[5] = b6;
	msg.data[6] = b7;
	msg.data[7] = b8;
	return sendMessage(&msg);
}


uint32_t CANController::getTimeSinceLastReceivedMessage()
{
	return getTime() - _lastMessageReceivedTimestamp;
}

bool CANController::sendMessage(uint32_t id, uint8_t dlc, void const *ptr)
{
	uint8_t *data = (uint8_t *) ptr;

	if (dlc > 8) dlc = 8;
	CANMessage msg(id, dlc);
	if (dlc > 0) {
		memcpy(msg.data, data, dlc);
	}
	return sendMessage(&msg);
}

void CANController::setSilent(bool beSilent)
{
	_silent = beSilent;
}


