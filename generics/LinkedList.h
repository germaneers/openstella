/*
 * LinkedList.h
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
#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_


template <class T> class LinkedList {
public:
	class Item {
		private:
			T _data;
			Item *_next;
		public:
			Item(T newData) : _data(newData), _next(0) {};
			T getData() { return _data; }
			Item *getNext() { return _next; }
			void setNext(Item *next) { _next = next; }
	};


private:
	Item *_firstItem;
public:
	LinkedList() : _firstItem(0) {}

	Item *getFirstItem() { return _firstItem; }

	unsigned int count() {
		unsigned int result = 0;
		for (Item *item = _firstItem; item!=0; item=item->getNext()) {
			result++;
		}
		return result;
	}

	Item *getItemAt(unsigned int index) {
		unsigned int i = 0;
		for (Item *item = _firstItem; item!=0; item=item->getNext()) {
			if (i==index) { return item; }
			i++;
		}
		return 0; // index not found
	}

	Item *operator[] (unsigned int index) { return getItemAt( index ); }

	unsigned int add(T data) {
		unsigned int result = 0;
		Item *newItem = new Item(data);
		if (_firstItem==0) {
			_firstItem = newItem;
		} else {
			Item *last = _firstItem;
			result = 1;
			while (last->getNext() != 0) {
				last = last->getNext();
				result++;
			}
			last->setNext(newItem);
		}
		return result;
	}

	bool removeAt(unsigned int index) {
		unsigned int i = 0;
		Item *previous = 0;

		for (Item *current = _firstItem; current!=0; current=current->getNext()) {
			if (i==index) {
				if (previous==0) {
					_firstItem = current->getNext();
				} else {
					previous->setNext(current->getNext());
				}
				delete current;
				return true;
			}

			previous = current;
			i++;
		}
		return false;
	}

	unsigned int remove(T data) {
		unsigned int result = 0;

		while ((_firstItem!=0) && (_firstItem->getData()==data)) {
			Item *tmp = _firstItem->getNext();
			delete _firstItem;
			_firstItem = tmp;
			result++;
		}

		if (_firstItem!=0) {
			Item *previous = _firstItem;
			Item *current = _firstItem->getNext();

			while (current!=0) {
				if (current->getData()==data) {
					previous->setNext(current->getNext());
					delete current;
					current = previous->getNext();
					result++;
				} else {
					current = current->getNext();
				}
			}
		}

		return result;
	}
};

#endif /* LINKEDLIST_H_ */
