/*
 * CANControllerTask.cpp
 *
 *  Created on: 28.12.2012
 *      Author: hd
 */

#include "CANControllerTask.h"
#include "CANController.h"
#include "../OS/CriticalSection.h"

CANControllerTask *_instance = 0;
Queue<uint16_t> *_interruptNotificationQueue = 0;

CANControllerTask::CANControllerTask()
  : Task("can", 500)
{
    _interruptNotificationQueue = new Queue<uint16_t>(16);
}

CANControllerTask* CANControllerTask::getInstance()
{
	CriticalSection critical();
	if (_instance==0) {
		_instance = new CANControllerTask();
	}
	return _instance;
}

void CANControllerTask::notifyInterrupt(CAN::channel_t channel, bool rxOk, bool txOk, uint8_t mob_id)
{
	if (_interruptNotificationQueue!=0) {
		int32_t higherPriorityTaskWoken = 0;
		uint16_t info = 0;

		info |= ((uint8_t) channel<<12); // can channel 0..15
		if (rxOk) { info |= 0x100; }
		if (txOk) { info |= 0x200; }
		info |= mob_id; // mob_id 1..32

		_interruptNotificationQueue->sendToBackFromISR(info, &higherPriorityTaskWoken);
		if (higherPriorityTaskWoken) {
			Task::yieldFromISR();
		}
	}
}

void CANControllerTask::startIfNotRunning() {
	// TODO replace with counting semaphore? suspend task if not needed any more?

	bool doRun = false;
	static bool isRunning = false;
	{
		CriticalSection critical();
		if (!isRunning) {
			isRunning = true;
			doRun = true;
		}
	}

	if (doRun) { run(); }
}

void CANControllerTask::execute()
{
	uint16_t info;
	uint32_t timeToWait = 0;
	while (true) {
		if (_interruptNotificationQueue->receive(&info, timeToWait)) {
			CAN::channel_t channel = (CAN::channel_t)(info>>8);
			uint8_t mob_id = info & 0xFF;
			bool rxOk = (mob_id & 0x100) != 0;
			bool txOk = (mob_id & 0x200) != 0;
			CANController::get(channel)->notifyMOBInterrupt(rxOk, txOk, mob_id);
		}

		timeToWait = 1000;
		for (uint8_t i=0; i<CAN::channel_none; i++) {
			if (CANController::_controllers[i]!=0) {
				uint32_t t = CANController::get((CAN::channel_t)i)->sendCyclicCANMessages();
				if (t<timeToWait) { timeToWait = t; }
			}
		}

	}
}
