/*
 * CriticalSection.h
 *
 *  Created on: 28.12.2012
 *      Author: hd
 */

#ifndef CRITICALSECTION_H_
#define CRITICALSECTION_H_

#include "Task.h"

class CriticalSection {
public:
	CriticalSection() { vPortEnterCritical(); }
	virtual ~CriticalSection() { vPortExitCritical(); }
};

#endif /* CRITICALSECTION_H_ */
