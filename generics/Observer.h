/*
 * Observer.h
 *
 * Copyright 2012 Germaneers GmbH
 * Copyright 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
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
#ifndef OBSERVER_H_
#define OBSERVER_H_

#include "LinkedList.h"

template <class Tsubject, class Tdata> class Observer {
	public:
		Observer() {}
		virtual ~Observer() {}
		virtual void update(Tsubject *subject, Tdata data)= 0;
};

template <class Tsubject, class Tdata> class Subject {

	private:
		LinkedList<Observer<Tsubject,Tdata> *> _observers;

	public:
		Subject() {}
		virtual ~Subject() {}

		void attach (Observer<Tsubject,Tdata> &observer)
		{
			_observers.add(&observer);
		}

		void notify (Tdata data)
		{
			for (Observer<Tsubject,Tdata> *item=_observers.getFirstItem(); item!=0; item=item->getNext())
			{
				item->getData()->update(this, data);
			}
		}

};

#endif /* OBSERVER_H_ */
