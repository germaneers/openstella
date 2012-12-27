/*
 * Observer.cpp
 *
 *  Created on: 13.11.2011
 *      Author: hd
 */

#include "Observer.h"
#include <algorithm>

Observer::Observer(uint8_t maxQueuedEvents) :
	_observerQueue(maxQueuedEvents)
{
}

Observer::~Observer()
{
}

void Observer::observerNotify(observer_callback_t callback, Event *sender, void *param)
{
	ObserverCallback *x = new ObserverCallback(callback, sender, param);
	if (!_observerQueue.sendToBack(x))
	{
		delete(x);
	}
}

void Observer::pollEvents()
{
	ObserverCallback *x;
	while (_observerQueue.count()>0) {
		if (_observerQueue.shift(&x))
		{
			x->callback(x->sender, x->param);
			delete(x);
		}
	}

}
