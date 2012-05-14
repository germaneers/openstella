/*
 * Task.cpp
 *
 * Copyright 2011 Germaneers GmbH
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

#include "Task.h"
#include <stdlib.h>
#include <string.h>


void taskfunwrapper(void* parm) {
    (static_cast<Task*>(parm))->execute();
    while(1); // TODO throw error: execute() should never return
}

void taskfunctorwrapper(void* parm)
{
    (static_cast<TaskFunctorBase*>(parm))->call();
    while(1); // TODO throw error: call() should never return
}


Task::Task(char const *name, uint16_t stackSize, uint8_t priority) :
	_name(0),
	_stackSize(stackSize),
	_priority(priority),
	_hnd(0)
{
	if (name==0) {
		setTaskName("noname");
	} else {
		setTaskName(name);
	}

}

Task::~Task()
{
	free(_name);
}

void Task::run() {
	long result = xTaskCreate(&taskfunwrapper, ( signed char * ) _name, _stackSize, this, _priority, &_hnd);
	if (result != pdPASS) {  while(1); }; // could not create task!
}

void Task::runFunctor(TaskFunctorBase *functor, const char *name, uint16_t stackSize, uint8_t priority)
{
	xTaskCreate(&taskfunctorwrapper, ( signed char * ) name, stackSize, functor, priority, 0);
}

void Task::stop()
{
	vTaskDelete(_hnd);
}

void Task::setTaskName(char const *newName)
{
	if (_name) { free(_name); }
	_name = (char*) malloc( strlen(newName) + 1 );
	strcpy(_name, newName);
}

void Task::suspend()
{
	vTaskSuspend(_hnd);
}

void Task::resume()
{
	vTaskResume(_hnd);
}

void Task::resumeFromISR()
{
	xTaskResumeFromISR(_hnd);
}

void Task::yield()
{
	taskYIELD();
}

void Task::delay_ms(uint32_t delay) {
	vTaskDelay((delay>0x3FFFFFFF) ? 0xFFFFFFFF : 4*delay);
}

void Task::delay_ticks(uint32_t delay) {
	vTaskDelay(delay);
}

uint32_t Task::getTime()
{
	return xTaskGetTickCount()/4;
}

void Task::setPriority(uint8_t priority)
{
	vTaskPrioritySet(_hnd, priority);
}

uint8_t Task::getPriority()
{
	return uxTaskPriorityGet(_hnd);
}
