/*
 * Ringbuffer.h
 *
 * Copyright 2011 Germaneers GmbH
 * Copyright 2011 Hubert Denkmair (hubert.denkmair@germaneers.com)
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

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include "openstella/OS/Mutex.h"
#include <stdlib.h>

/**
 * @class Ringbuffer
 * @brief a threadsafe ringbuffer template
 *
 * \li Instantiate a ringbuffer for a certain type.
 * \li The full size for the buffer (size*sizeof(item)) will be allocated in the constructor.
 * \li sendToBack() allows you to send as many items to the ringbuffer as you like. \n
 *     Though, only the last (size) ones will be kept
 * \li get the first item from the buffer via shift()
 *
 * \warning be careful when constructing a ringbuffer for pointers.\n
 *          pointers get overwritten when the buffer runs full, and objects referred by the pointers
 *          are not automatically destructed.
 */
template <class T>
class Ringbuffer
{
	private:
		Mutex _mutex;
		T *_data;

		int _size;
		int _begin;
		int _end;
		int _count;

	public:
		/// Instatiate and allocate the ringbuffer
		/**
		 * @param size maximum numer of items the ringbuffer can hold
		 */
		Ringbuffer(int size=10) : _size(size), _begin(0), _end(0), _count(0) {
			_data = (T*) pvPortMalloc(sizeof(T)*_size);
		}
		virtual ~Ringbuffer() { vPortFree(_data); }

		/// get the size (in items) of the ringbuffer
		unsigned int size() { return _size; }

		/// get the number of items currently waiting in the ringbuffer
		/** @return number of items currently waiting in the ringbuffer */
		unsigned int count() {
			MutexGuard(&_mutex);
			return _count;
		}

		/// add an item to the ringbuffer
		/**  @param item the item to add. it will be copied into the buffer */
		void sendToBack(const T item) {
			MutexGuard(&_mutex);
			_data[_end] = item;
			if (++_end >= _size) _end=0;
			if (_count<_size) _count++;
		}

		/// get the first element from the buffer
		/** @param [out] buf the object in which the extracted data will be saved.
		 *  @return true if a object was returned, false if the buffer was empty.
		 */
		bool shift(T *buf) {
			MutexGuard(&_mutex);
			if (_count==0) { return false; }

			*buf = _data[_begin];
			_begin++;
			if (_begin >= _size) _begin=0;
			_count--;

			return true;
		}

};

#endif /* RINGBUFFER_H_ */
