/*
 * Semaphore.h
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

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <stdint.h>
#include <freertos/include/FreeRTOS.h>
#include <freertos/include/semphr.h>

class Semaphore
{
	private:
		xSemaphoreHandle _hnd;
	public:
		Semaphore();
		virtual ~Semaphore();

		bool take(uint32_t timeout_ms = portMAX_DELAY);
		bool give();
		bool giveFromISR(int32_t *higherPriorityTaskWoken);
};

#endif /* SEMAPHORE_H_ */
