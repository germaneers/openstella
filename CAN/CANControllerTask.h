/*
 * CANControllerTask.h
 *
 *  Created on: 28.12.2012
 *      Author: hd
 */

#ifndef CANCONTROLLERTASK_H_
#define CANCONTROLLERTASK_H_
#include "../OS/Task.h"
#include "../OS/Queue.h"
#include "CANtypes.h"

class CANControllerTask : public Task {
private:
	static Queue<uint16_t> *_interruptNotificationQueue;
	static CANControllerTask *_instance;
public:
	CANControllerTask();
	static CANControllerTask *getInstance();
	static void notifyInterrupt(CAN::channel_t channel, bool rxOk, bool txOk, uint8_t mob_id);
	void startIfNotRunning();
protected:
	virtual void execute();
};

#endif /* CANCONTROLLERTASK_H_ */
