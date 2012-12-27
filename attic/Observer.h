/*
 * Observer.h
 *
 *  Created on: 13.11.2011
 *      Author: hd
 */

#ifndef OBSERVER_H_
#define OBSERVER_H_

#include <utility>
#include <functional>
#include <vector>
#include <queue>

#include "openstella/OS/Ringbuffer.h"

using namespace std;

class Event;

class Observer
{
	public:
		typedef function<void (Event *, void *)> observer_callback_t;

	private:
		class ObserverCallback {
			public:
				Observer::observer_callback_t callback;
				Event *sender;
				void *param;
				ObserverCallback(Observer::observer_callback_t theCallback, Event *theSender, void *theParam) :
					callback(theCallback), sender(theSender), param(theParam) {}
		};

		Ringbuffer<ObserverCallback*> _observerQueue;

	public:
		Observer(uint8_t maxQueuedEvents=5);
		virtual ~Observer();
		void pollEvents();

		void observerNotify(observer_callback_t callback, Event *sender, void *param);
};

#endif /* OBSERVER_H_ */
