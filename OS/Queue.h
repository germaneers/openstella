/*
 * Queue.h
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

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>
#include <freertos/include/FreeRTOS.h>
#include <freertos/include/queue.h>

/**
 * @class Queue
 * @brief A template allowing to put C++ objects into FreeRTOS Queues
 *
 * \include QueueDemo.cpp
 */
template <class T> class Queue
{
	private:
		xQueueHandle _hnd;
		uint16_t _length;

	public:

		/**
		 * @param queueLength maximum number of elements in this queue \n
		 *        memory will be allocated for queueLength*sizeof(T)
		 *        at construction.
		 */
		Queue(uint16_t queueLength=10) : _length(queueLength) {
			_hnd = xQueueCreate(queueLength, sizeof(T));
		}

		virtual ~Queue() {
			vQueueDelete(_hnd);
		}

		/// number of elements waiting in the queue
		/** @return number of elements waiting, zero if the queue is empty */
		uint16_t messagesWaiting()
		{
			return uxQueueMessagesWaiting(_hnd);
		}

		uint16_t getLength() {
			return _length;
		}

		bool isFull() {
			return (messagesWaiting()>=_length);
		}

		/// put an element to the front of the queue
		/** puts the item at the front, so that it will be the next element to be received.
		 *  @param item the element to push
		 *  @param timeout_ms maximum time to block (in milliseconds) if the queue is full
		 *  @return true if the element has been placed in the queue, false if a timeout occurred.
		 *  \warning do not use this method from within an interrupt handler. use sendToFrontFromISR() instead!
		 */
		bool sendToFront(const T item, uint32_t timeout_ms=portMAX_DELAY)
		{
			return xQueueSendToFront(_hnd, &item, (timeout_ms>0x3FFFFFFF) ? 0xFFFFFFFF : 4*timeout_ms) == pdTRUE;
		}

		/// put an element to the front of the queue, from ISR context
		/** puts the item at the front, so that it will be the next element to be received.
		 *  @param item the element to push
		 *  @param [out] higherPriorityTaskWoken will be set 1 (otherwise: 0) if the operation
		 *         caused a task to unblock, and the unblocked task has a higher priority than the
		 *         currently running task. In this case, a context switch should be requested
		 *         before the interrupt is exited.
		 *  \warning only use this method from within an interrupt handler. use sendToFront() otherwise!
		 */
		bool sendToFrontFromISR(const T item, int32_t *higherPriorityTaskWoken=0)
		{
			int32_t dummy;
			if (higherPriorityTaskWoken==0) higherPriorityTaskWoken = &dummy;
			return xQueueSendToFrontFromISR(_hnd, &item, higherPriorityTaskWoken) == pdTRUE;
		}

		/// push an element to the back of the queue
		/** puts the item at the back of the queue, so that it will be the last element to be received.
		 *  @param item the element to push
		 *  @param timeout_ms maximum time to block (in milliseconds) if the queue is full
		 *  @return true if the element has been placed in the queue, false if a timeout occurred.
		 *  \warning do not use this method from within an interrupt handler. use sendToBackFromISR() instead!
		 */
		bool sendToBack(const T item, uint32_t timeout_ms=portMAX_DELAY)
		{
			return xQueueSendToBack(_hnd, &item, (timeout_ms>0x3FFFFFFF) ? 0xFFFFFFFF : 4*timeout_ms) == pdTRUE;
		}

		/// push an element to the back of the queue, from ISR context
		/** puts the item at the back of the queue, so that it will be the last element to be received.
		 *  @param item the element to push
		 *  @param [out] higherPriorityTaskWoken will be set 1 (otherwise: 0) if the operation
		 *         caused a task to unblock, and the unblocked task has a higher priority than the
		 *         currently running task. In this case, a context switch should be requested
		 *         before the interrupt is exited.
		 *  @return true if the element has been placed in the queue, false if a timeout occurred.
		 *  \warning only use this method from within an interrupt handler. use sendToBack() otherwise!
		 */
		bool sendToBackFromISR(const T item,  int32_t *higherPriorityTaskWoken=0)
		{
			int32_t dummy;
			if (higherPriorityTaskWoken==0) higherPriorityTaskWoken = &dummy;
			return xQueueSendToBackFromISR(_hnd, &item, higherPriorityTaskWoken) == pdTRUE;
		}

		/// receive the first element in the queue
		/** gets (and removes) the first element from the queue
		 *  @param buf pointer where the received element will be copied to
		 *  @param timeout_ms maximum time to block (in milliseconds) if the queue is empty
		 *  @return true if an element was received, false if a timeout occurred
		 */
		bool receive(T *buf, uint32_t timeout_ms=portMAX_DELAY)
		{
			return xQueueReceive(_hnd, buf, (timeout_ms>0x3FFFFFFF) ? 0xFFFFFFFF : 4*timeout_ms) == pdTRUE;
		}

		/// receive the first element in the queue, from ISR context
		/** gets (and removes) the first element from the queue
		 *  @param buf pointer where the received element will be copied to
		 *  @param [out] higherPriorityTaskWoken will be set 1 (otherwise: 0) if the operation
		 *         caused a task to unblock, and the unblocked task has a higher priority than the
		 *         currently running task. In this case, a context switch should be requested
		 *         before the interrupt is exited.
		 *  @return true if an element was received, false if a timeout occurred
		 *  \warning only use this method from within an interrupt handler. use sendToFront() otherwise!
		 */
		bool receiveFromISR(T *buf, int32_t *higherPriorityTaskWoken=0)
		{
			int32_t dummy;
			if (higherPriorityTaskWoken==0) higherPriorityTaskWoken = &dummy;
			return xQueueReceiveFromISR(_hnd, buf, higherPriorityTaskWoken) == pdTRUE;
		}

		/// inspect the first element in the queue
		/** returns the first element from the queue, but doesn't remove it
		 *  @param buf pointer where the received element will be copied to
		 *  @param timeout_ms maximum time to block (in milliseconds) if the queue is empty
		 *  @return true if an element was received, false if a timeout occurred
		 */
		bool peek(T *buf, uint32_t timeout_ms=portMAX_DELAY)
		{
			return xQueuePeek(_hnd, buf, (timeout_ms>0x3FFFFFFF) ? 0xFFFFFFFF : 4*timeout_ms) == pdTRUE;
		}

		/// give the queue a name in the freeRTOS queue registry
		/** for debugging purposes only
		 *  @param name the name for the queue in the registry
	 	 */
		void addToRegistry(char* name) {
			vQueueAddToRegistry(_hnd, (signed char*) name);
		}

};

#endif /* QUEUE_H_ */
