/*
 * Watchdog.h
 *
 *  Created on: 15.03.2013
 *      Author: denkmahu
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#include <stdint.h>

class Watchdog {
private:
	Watchdog();
public:
	static void setup(uint32_t interval_ms, bool lock=true);
	static void feed();
};

#endif /* WATCHDOG_H_ */
