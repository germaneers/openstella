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
#include "../OS/CriticalSection.h"
#include "../OS/NoTaskSwitchSection.h"

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
	_isEnabled(false),
	_availableSendingMOBs(16),
	_channel(channel),
	_periph(periph),
	_base(base),
	_observers(),
	_observerMutex(),
	_lastMessageReceivedTimestamp(0),
	_silent(false)
{
	_controllerTask = CANControllerTask::getInstance();
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

	tCANMsgObject msgobj;
	msgobj.ulMsgID = 0;
	msgobj.ulMsgIDMask = 0;
	msgobj.ulMsgLen = 8;
	msgobj.ulFlags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;

	for (int i=1; i<=32; i++)
	{
		if (i<=_availableSendingMOBs.getLength()) {
			_availableSendingMOBs.sendToBack(i);
		} else {
			if (i<32) { msgobj.ulFlags |= MSG_OBJ_FIFO; }
			MAP_CANMessageSet(_base, i, &msgobj, MSG_OBJ_TYPE_RX);
		}
	}

}

void CANController::notifyMOBInterrupt(bool rxOk, bool txOk, uint8_t mob_id)
{
	if (txOk) {
		_availableSendingMOBs.sendToBack(mob_id);
	} else if (rxOk) {
		_lastMessageReceivedTimestamp = Task::getTime();

		tCANMsgObject msgobj;
		MAP_CANMessageGet(_base, mob_id, &msgobj, true);

		CANMessage *msg = new CANMessage();
		msg->loadFromMOB(this, &msgobj);
		notifyObservers(msg);

		if (msg->getReferenceCounter()==0) {
			delete(msg);
		}
	}
}

void CANController::handleInterrupt()
{
	while (uint32_t mob_id = MAP_CANIntStatus(_base, CAN_INT_STS_CAUSE))
	{
		uint32_t status = getControlRegister(); // also clears the interrupt

		if (mob_id == CAN_INT_INTID_STATUS) // status interrupt. error occurred?
		{
			// TODO: error handling
		} else if ( (mob_id >= 1) && (mob_id <= 32) ) { // mailbox event
			bool rxOk = (status & CAN_STATUS_RXOK) > 0;
			bool txOk = (status & CAN_STATUS_TXOK) > 0;
			_controllerTask->notifyInterrupt(_channel, rxOk, txOk, mob_id);
		}

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
	_controllerTask->startIfNotRunning();
	_isEnabled = true;
}

void CANController::disable()
{
	_isEnabled = false;
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

bool CANController::sendMessage(CANMessage *msg)
{
	if (_silent) return true;

	uint8_t mob_num;
	if (_availableSendingMOBs.receive(&mob_num)) {

		tCANMsgObject msgobj;
		msgobj.ulMsgID     = msg->id;
		msgobj.ulMsgIDMask = 0;
		msgobj.ulMsgLen    = msg->dlc;
		msgobj.pucMsgData  = msg->data;

		msgobj.ulFlags = MSG_OBJ_TX_INT_ENABLE;
		if (msg->isExtendedId() || (msg->id>0x7FF)) {
			msgobj.ulFlags |= MSG_OBJ_EXTENDED_ID;
		}
		if (msg->isRemoteFrame()) {
			msgobj.ulFlags |= MSG_OBJ_REMOTE_FRAME;
		}

		MAP_CANMessageSet(_base, mob_num, &msgobj, MSG_OBJ_TYPE_TX);

		return true;
	} else {
		return false;
	}

}

CANController *CANController::_controllers[3] = {0, 0, 0};
CANController *CANController::get(CAN::channel_t channel)
{
	CriticalSection critical();

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

void CANController::notifyObservers(CANMessage *canmsg)
{
	MutexGuard guard(&_observerMutex);
	{
		NoTaskSwitchSection noTaskSwitchSection();
		for (ObserverList::Item *i=_observers.getFirstItem(); i!=0; i=i->getNext()) {
			if (i->getData().matches(canmsg)) {
				canmsg->incrementReferenceCounter();
				i->getData().observer->notifyCANMessage(canmsg);
			}
		}
	}

}

bool CANController::registerObserver(CANObserver *observer)
{
	return registerObserver(observer, 0, 0);
}

bool CANController::registerObserver(CANObserver *observer, int32_t can_id, uint32_t mask)
{
	MutexGuard guard(&_observerMutex);

	// don't register if same observer on same object is already installed
	for (ObserverList::Item *i=_observers.getFirstItem(); i!=0; i=i->getNext()) {
		if (i->getData().matches(can_id, mask, observer)) {
			return true; // already registered
		}
	}

	ObserverListEntry e;
	e.can_id = can_id;
	e.mask = mask;
	e.observer = observer;
	_observers.add(e);

	return false;
}

int CANController::unregisterObserver(CANObserver *observer)
{
	MutexGuard guard(&_observerMutex);

	int result = 0;
	int i;
	bool finished = false;

	while (!finished) {
		i = 0;
		finished = true;
		for (ObserverList::Item *item=_observers.getFirstItem(); item!=0; item=item->getNext()) {
			if (item->getData().observer==observer) {
				_observers.removeAt(i);
				result++;
				finished = false;
				break;
			}
			i++;
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
	uint32_t minTimeToWait = 0xFFFFFFFF;
	if (!_isEnabled) { return minTimeToWait; }

	_cyclicMessages.lock();
	static uint32_t lastTickCount = 0;
	uint32_t now = Task::getTime();

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

uint32_t CANController::getTimeSinceLastReceivedMessage()
{
	return Task::getTime() - _lastMessageReceivedTimestamp;
}

