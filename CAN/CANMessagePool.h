/*
 * CANMessagePool.h
 *
 *  Created on: 09.11.2012
 *      Author: denkmahu
 */

#ifndef CANMESSAGEPOOL_H_
#define CANMESSAGEPOOL_H_
#include "CANMessage.h"
#include "../OS/Mutex.h"
#include "../generics/LinkedList.h"

class CANMessagePool {
private:
	class PoolEntry {
		public:
			CANMessage *msg;
			bool isInUse;
			PoolEntry() : msg(new CANMessage()), isInUse(false) {}
	};

	typedef LinkedList<PoolEntry*> PoolList;

	uint16_t _maxSize;
	PoolList _pool;
	Mutex _lock;

public:
	CANMessagePool(uint16_t maxSize=20)
	  : _maxSize(maxSize),
	    _pool()
	{
	}

	CANMessage *getMessage() {
		MutexGuard guard(&_lock);
		uint16_t count = 0;

		for (PoolList::Item *item=_pool.getFirstItem(); item!=0; item=item->getNext()) {
			++count;
			PoolEntry *entry = item->getData();
			if (!entry->isInUse) {
				entry->isInUse = true;
				return entry->msg;
			}
		}

		if (count<_maxSize) {
			PoolEntry *entry = new PoolEntry();
			_pool.add(entry);
			entry->isInUse = true;
			return entry->msg;
		}

		return 0;
	}

	void returnMessage(CANMessage *msg) {
		MutexGuard guard(&_lock);

		for (PoolList::Item *item=_pool.getFirstItem(); item!=0; item=item->getNext()) {
			PoolEntry *entry = item->getData();
			if (entry->isInUse && (entry->msg==msg)) {
				entry->isInUse = false;
				return;
			}
		}
	}

};

#endif /* CANMESSAGEPOOL_H_ */
