/*
 * Ringbuffer.h
 *
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

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <openstella/OS/Mutex.h>

template <class valueType, int bufferSize> class RingBuffer {
private:
	valueType _buf[bufferSize];
	int _pos;
	Mutex _lock;
public:
	RingBuffer(valueType initialValue=0) :_pos(0) {
		for (int i=0; i<bufferSize; i++) {
			_buf[i] = initialValue;
		}
	}

	void push(valueType value) {
		MutexGuard guard(&_lock);
		_pos = (_pos + 1) % bufferSize;
		_buf[_pos] = value;
	}

	valueType get(int position) {
		MutexGuard guard(&_lock);
		int p = _pos-position;
		while (p<0) { p+=bufferSize; }
		return _buf[p];
	}
};


#endif /* RINGBUFFER_H_ */
