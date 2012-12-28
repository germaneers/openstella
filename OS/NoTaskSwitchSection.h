/*
 * NoTaskSwitchSection.h
 *
 *  Created on: 28.12.2012
 *      Author: hd
 */

#ifndef NOTASKSWITCHSECTION_H_
#define NOTASKSWITCHSECTION_H_
#include <freertos/include/task.h>

class NoTaskSwitchSection {
public:
	NoTaskSwitchSection() {
		vTaskSuspendAll();
	}
	virtual ~NoTaskSwitchSection() {
		xTaskResumeAll();
	}
};

#endif /* NOTASKSWITCHSECTION_H_ */
