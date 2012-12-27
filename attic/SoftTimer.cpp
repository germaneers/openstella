/*
 * SoftTimer.cpp
 *
 *  Created on: 13.11.2011
 *      Author: hd
 */

#include "SoftTimer.h"

extern "C" {
	static void softTimerCallback( xTimerHandle pxTimer )
	{
		SoftTimer *t = (SoftTimer*) pvTimerGetTimerID(pxTimer);
		t->executeCallback();
	}
}

SoftTimer::SoftTimer(uint32_t period, bool doAutoReload, callback_t callbackFunc, const char* name) :
	_period(period),
	_doAutoReload(doAutoReload),
	_callbackFunc(callbackFunc)
{
	_hnd = xTimerCreate( (const signed char*) name, _period, _doAutoReload, (void*)this, softTimerCallback );
}

SoftTimer::~SoftTimer()
{
	xTimerDelete(_hnd, portMAX_DELAY);
}

void SoftTimer::executeCallback()
{
	_callbackFunc(this);
}

bool SoftTimer::isActive()
{
	return xTimerIsTimerActive(_hnd) != pdFALSE;
}

uint32_t SoftTimer::getPeriod()
{
	return _period;
}

bool SoftTimer::start(uint32_t xBlockTime)
{
	return xTimerStart(_hnd, xBlockTime) == pdPASS;
}

bool SoftTimer::stop(uint32_t xBlockTime)
{
	return xTimerStop(_hnd, xBlockTime) == pdPASS;
}

bool SoftTimer::reset(uint32_t xBlockTime)
{
	return xTimerReset(_hnd, xBlockTime) == pdPASS;
}

bool SoftTimer::setPeriod(uint32_t new_period, uint32_t xBlockTime)
{
	if (xTimerChangePeriod(_hnd, new_period, xBlockTime) == pdPASS)
	{
		_period = new_period;
		return true;
	} else {
		return false;
	}
}

bool SoftTimer::startFromISR( int32_t *pxHigherPriorityTaskWoken )
{
	return xTimerStartFromISR(_hnd, pxHigherPriorityTaskWoken) == pdPASS;
}

bool SoftTimer::stopFromISR( int32_t *pxHigherPriorityTaskWoken )
{
	return xTimerStopFromISR(_hnd, pxHigherPriorityTaskWoken) == pdPASS;
}

bool SoftTimer::resetFromISR( int32_t *pxHigherPriorityTaskWoken )
{
	return xTimerResetFromISR(_hnd, pxHigherPriorityTaskWoken) == pdPASS;
}

bool SoftTimer::setPeriodFromISR(uint32_t new_period, int32_t *pxHigherPriorityTaskWoken )
{
	if (xTimerChangePeriodFromISR(_hnd, new_period, pxHigherPriorityTaskWoken) == pdPASS)
	{
		_period = new_period;
		return true;
	} else {
		return false;
	}
}
