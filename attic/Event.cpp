/*
 * Event.cpp
 *
 *  Created on: 15.11.2011
 *      Author: hd
 */

#include "Event.h"

Event::Event() :
	_observerCollection(0),
	_mutex()
{
}

Event::~Event()
{
	if (_observerCollection)
	{
		delete(_observerCollection);
	}
}

void Event::lazyInstantiate()
{
	if (!_observerCollection) {
		_observerCollection = new vector<collection_entry_t>();
	}

}

void Event::registerObserver(Observer* observer, Observer::observer_callback_t callback)
{
	MutexGuard(_mutex);
	lazyInstantiate();
	_observerCollection->push_back(collection_entry_t(observer, callback));
}

void Event::unregisterObserver(Observer* observer, Observer::observer_callback_t callback)
{
	// erase-remove idiom
	/*
	 	MutexGuard(_mutex);
	 	_observerCollection.erase(
		std::remove(
			_observerCollection.begin(), _observerCollection.end(), collection_entry_t(observer, callback)
		),
		_observerCollection.end()
	);*/
}

void Event::notifyObservers(void *param)
{
	MutexGuard(_mutex);
	if (_observerCollection) {
		for (unsigned int i=0; i<_observerCollection->size(); i++)
		{
			collection_entry_t cb = (*_observerCollection)[i];
			cb.first->observerNotify(cb.second, this, param);
		}
	}
}
