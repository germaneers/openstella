/*
 * Task.h
 *
 * Copyright 2011,2012 Germaneers GmbH
 * Copyright 2011,2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
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

#ifndef TASK_H_
#define TASK_H_

#include <stdint.h>
#include "freertos/include/FreeRTOS.h"
#include "freertos/include/task.h"

void taskfunwrapper(void* parm);
void taskfunctorwrapper(void* parm);

class TaskFunctorBase {
public:
	virtual void call()=0;
};

template <class TClass> class TaskFunctor : public TaskFunctorBase {
	private:
		TClass *_calledObject;
		void (TClass::*_calledMethod)();
	public:
		TaskFunctor(TClass *calledObject, void (TClass::*calledMethod)()) :
			_calledObject(calledObject),
			_calledMethod(calledMethod)
		{
		}
		virtual void call() {
			(*_calledObject.*_calledMethod)();
		}
};

/**
 * @class Task
 * @brief A FreeRTOS Task
 *
 * Derive your own classes from Task and overwrite the virtual Task::execute() method.
 *
 * Task::execute() should never return, e.g. include a while(1) loop.
 *
 * libopenstellas main.cpp expects a "MainTask.h" to exist in your project,
 * containing a class named MainTask derived from Task.
 *
 * It will instantiate a MainTask and launch it as FreeRTOSes first Task.
 *
 * \include MainTask.h
 *
 */

class Task {
	friend void taskfunwrapper(void* parm);

	private:
		char *_name;
		uint16_t _stackSize;
		uint8_t _priority;
		xTaskHandle _hnd;
		static int unnamed_task_counter;

	public:
		/// create a new Task instance.
		/**
		 * The task will not be started automatically. Call Task::run() to start it.
		 * @param name name of the task (for debugging purposes only)
		 * @param stackSize size of the stack (in words, e.g. 4 bytes) created for the new task
		 * @param priority priority of the new task
		 *
		 * \warning the stackSize is NOT given in bytes,
		 *          but in multiples of the number of bytes used to save on word
		 *          (e.g. 4 bytes on ARM Cortex M3)
		 */
		Task(char const *name=0, uint16_t stackSize=200, uint8_t priority=1);
		virtual ~Task();

		/// run the task.
		/**
		 * a new task is inserted into the freeRTOS scheduler and
		 * execute() will be called in the context of the new task.
		 */
		void run(void);

		/// stop the task.
		/** remove the task from the freeRTOS scheduler */
		void stop();

		/// execute a arbitrary method of any object as a new task.
		/**
		 * create a TaskFunctor and call runFunctor() to execute any method as a new task.
		 *
		 * @param functor TaskFunctor class used to call the method
		 * @param name name of the new task (for debugging purposes only)
		 * @param stackSize size of the stack (in words, e.g. 4 bytes) created for the new task
		 * @param priority initial priority of the newly created task
		 *
		 * \warning the stackSize is NOT given in bytes,
		 *          but in multiples of the number of bytes used to save on word
		 *          (e.g. 4 bytes on ARM Cortex M3)
		 *
		 * \include FunctorTask.h
		 *
		 */
		void runFunctor(TaskFunctorBase *functor, char const *name=0, uint16_t stackSize=200, uint8_t priority=1);

		/// suspend the task.
		/** the task will not be scheduled any more until resume() is called. */
		void suspend();

		/// resume the suspend()ed task.
		/**
		 * execution will continue after the last executed instruction before suspending.
		 *
		 * @warning don't call this method from a ISR. use resumeFromISR() instead!
		 * @see Task::resumeFromISR
		 */
		void resume();

		/// resume the suspend()ed task from an ISR
		/**
		 * @warning only call this method from within interrupt handlers. otherwise, use resume() instead.
		 * @see Task::resume
		 */
		void resumeFromISR();

		/// set Task priority
		/**
		 * @param priority the new task priority
		 */
		void setPriority(uint8_t priority);

		/// get Task priority
		/**
		 * @param priority the current task priority
		 */
		uint8_t getPriority();

		/// pass control to the next scheduled Task
		static void yield();

		/// wait for delay ms
		/**
		 * delay the current task for at least delay milliseconds.
		 * the task will only be rescheduled after the given time
		 * @param delay minimum time to wait in milliseconds
		 */
		static void delay_ms(uint32_t delay);

		/// wait for delay ticks
		/**
		 * delay the current task for at least delay systicks.
		 * the task will only be rescheduled after the given time.
		 *
		 * one systick defaults to 250us in libopenstella.
		 *
		 * @param delay minimum time to wait in systicks
		 */
		static void delay_ticks(uint32_t delay);

		/// get system time
		/**
		 * @return the number of milliseconds passed since the scheduler has been started.
		 */
		static uint32_t getTime();

	protected:
		/// task worker routine
		/**
		 * overwrite this pure virtual function.
		 *
		 * it will be executed as a new freeRTOS task when run() is called.
		 *
		 * it should never return - e.g. include a while(1) loop
		 */
		virtual void execute()=0;

		/// set a new task name
		/**
		 * (for debugging purposes only).
		 * the task name is really set in Task::run() so this call only works before run() is called.
		 */
		void setTaskName(char const *newName);
};

#endif /* TASK_H_ */
