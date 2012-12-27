/*
 * Event.h
 *
 *  Created on: 15.11.2011
 *      Author: hd
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <utility>
#include <vector>

#include "Mutex.h"
#include "Observer.h"

class Event
{
	private:
		typedef pair<Observer*,Observer::observer_callback_t> collection_entry_t;
		vector<collection_entry_t> *_observerCollection;
		Mutex _mutex;
		void lazyInstantiate();
	public:
		Event();
		virtual ~Event();
		void registerObserver(Observer* observer, Observer::observer_callback_t callback);
		void unregisterObserver(Observer* observer, Observer::observer_callback_t callback);
		void notifyObservers(void *param);
};

#endif /* EVENT_H_ */
