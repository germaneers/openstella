/*
 * RecursiveMutex.h
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

#ifndef RECURSIVE_MUTEX_H_
#define RECURSIVE_MUTEX_H_

#include <stdint.h>
#include <freertos/include/FreeRTOS.h>
#include <freertos/include/semphr.h>

/**
 * @class RecursiveMutex
 * @brief A FreeRTOS Recursive Mutex

 * A recursive mutex is basically a mutex with an reference counter.
 * The same task may take() the same mutex multiple times, and it is available for
 * other Tasks as soon as it is give()n the same number of times as it was take()n.
 *
 * \include RecursiveMutexDemo.h
 *
 */
class RecursiveMutex
{
	private:
		xSemaphoreHandle _hnd;
	public:
		/// create a new FreeRTOS Recursive Mutex
		RecursiveMutex();
		~RecursiveMutex();

		/// try to get the Mutex
		/**
		 * The same Task is able to take the same mutex multiple times simultaneously.
		 * @param timeout_ms maximum time (in milliseconds) to wait for the mutex
		 * @return true is the mutex was taken, false if a timeout occurred.
		 */
		bool take(uint32_t timeout_ms = portMAX_DELAY);

		/// release the Mutex, if taken
		/**
		 * @return true if the mutex was successfully released, false otherwise
		 */
		bool give();
};

/**
 * @class RecursiveMutexGuard
 * @brief Wrapper class for RecursiveMutex that takes the Mutex on construction and releases it on destruction
 *
 * Use the RecursiveMutexGuard to protect a resource inside a block.\n
 * As soon as the RecursiveMutexGuard goes out of scope, it is destructed and thus releases the mutex.
 *
 * \include RecursiveMutexDemo.h
 *
 */
class RecursiveMutexGuard
{
	private:
		RecursiveMutex *_mutex;
	public:
		RecursiveMutexGuard(RecursiveMutex *mutex) : _mutex(mutex) { _mutex->take(); }
		~RecursiveMutexGuard() { _mutex->give(); }

};
#endif /* RECURSIVE_MUTEX_H_ */
