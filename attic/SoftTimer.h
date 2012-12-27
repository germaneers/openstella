/*
 * SoftTimer.h
 *
 *  Created on: 13.11.2011
 *      Author: hd
 */

#ifndef SOFTTIMER_H_
#define SOFTTIMER_H_

#include <stdint.h>
#include <map>
#include <functional>

#include "lib/freertos/include/FreeRTOS.h"
#include "lib/freertos/include/timers.h"

class SoftTimer
{
	public:
		typedef std::function<void (SoftTimer *)> callback_t;

	private:
		xTimerHandle _hnd;
		uint32_t _period;
		bool _doAutoReload;
		callback_t _callbackFunc;

	public:
		SoftTimer(uint32_t period, bool doAutoReload, callback_t callbackFunc, const char* name=0);
		virtual ~SoftTimer();
		void executeCallback();

		bool isActive();
		uint32_t getPeriod();

		bool start(uint32_t xBlockTime=portMAX_DELAY);
		bool stop(uint32_t xBlockTime=portMAX_DELAY);
		bool reset(uint32_t xBlockTime=portMAX_DELAY);
		bool setPeriod(uint32_t new_period, uint32_t xBlockTime=portMAX_DELAY);

		bool startFromISR( int32_t *pxHigherPriorityTaskWoken );
		bool stopFromISR( int32_t *pxHigherPriorityTaskWoken );
		bool resetFromISR( int32_t *pxHigherPriorityTaskWoken );
		bool setPeriodFromISR(uint32_t new_period, int32_t *pxHigherPriorityTaskWoken );

};

#endif /* SOFTTIMER_H_ */
