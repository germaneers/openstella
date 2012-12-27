/*
 * Mutex.h
 *
 * Copyright 2011, 2012 Germaneers GmbH
 * Copyright 2011, 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
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


#ifndef MUTEX_H_
#define MUTEX_H_

#include <stdint.h>
#include <freertos/include/FreeRTOS.h>
#include <freertos/include/semphr.h>

/**
 * @class Mutex
 * @brief A FreeRTOS Mutex
 *
 * Use a Mutex to synchronize multiple Tasks access to a common ressource
 *
 * \include MutexDemo.h
 *
 */
class Mutex
{
	private:
		xSemaphoreHandle _hnd;
	public:
		/// create a new FreeRTOS Mutex
		Mutex();
		~Mutex();

		/// try to get the Mutex
		/**
		 * @param timeout_ms maximum time (in milliseconds) to wait for the mutex
		 * @return true is the mutex was taken, false if a timeout occurred.
		 */
		bool take ( uint32_t timeout_ms = portMAX_DELAY );

		/// release the Mutex, if taken
		/**
		 * @return true if the mutex was successfully released, false otherwise
		 * \warning don't use this method from within an ISR. use giveFromISR() instead!
		 */
		bool give();

};

/**
 * @class MutexGuard
 * @brief Wrapper class for Mutex that takes the Mutex on construction and releases it on destruction
 *
 * Use the MutexGuard to protect a resource inside a block.\n
 * As soon as the MutexGuard goes out of scope, it is destructed and thus releases the mutex.
 *
 * \include MutexDemo.h
 *
 */
class MutexGuard
{
	private:
		Mutex *_mutex;
	public:
		MutexGuard(Mutex *mutex) : _mutex(mutex) { _mutex->take(); }
		~MutexGuard() { _mutex->give(); }

};

#endif /* MUTEX_H_ */
