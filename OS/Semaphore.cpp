/*
 * Semaphore.cpp
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

#include "Semaphore.h"
#include <openstella/OS/Task.h>

Semaphore::Semaphore()
{
	vSemaphoreCreateBinary(_hnd);
	if (_hnd==0) while(1);
}

bool Semaphore::giveFromISR()
{
	static int32_t woken;
	woken = 0;
	int32_t result = xSemaphoreGiveFromISR(_hnd, &woken);
	if (woken!=0) {
		Task::yieldFromISR();
	}
	return result;
}

bool Semaphore::give()
{
	return xSemaphoreGive(_hnd) == pdTRUE;
}

bool Semaphore::take(uint32_t timeout_ms)
{
	return xSemaphoreTake(_hnd, (timeout_ms>0x3FFFFFFF) ? 0xFFFFFFFF : 4*timeout_ms) == pdTRUE;
}

bool Semaphore::giveFromISR(int32_t *higherPriorityTaskWoken)
{
	return xSemaphoreGiveFromISR(_hnd, higherPriorityTaskWoken);
}

Semaphore::~Semaphore()
{
	vSemaphoreDelete(_hnd);
}


