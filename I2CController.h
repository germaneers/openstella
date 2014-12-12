/*
 * I2CController.h
 *
 * Copyright 2012-2013 Germaneers GmbH
 * Copyright 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
 * Copyright 2012-2013 Stefan Rupp (stefan.rupp@germaneers.com)
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

#ifndef I2CCONTROLLER_H_
#define I2CCONTROLLER_H_

#include <stdint.h>
#include "GPIO.h"
#include "OS/RecursiveMutex.h"
#include "OS/Semaphore.h"
#include "AbstractI2cController.h"

class I2CController : public AbstractI2cController {
	friend void I2C0IntHandler();
	friend void I2C1IntHandler();

private:
	controller_t _controller;
	uint32_t _periph;
	uint32_t _base;
	uint32_t _defaultTimeout;

	GPIOPin _sda;
	GPIOPin _scl;

	RecursiveMutex _lock;
	Semaphore _interruptSemaphore;

	bool _interruptsEnabled;

	static I2CController *_controllers[controller_count];
	I2CController(controller_t controller, uint32_t periph, uint32_t base);

	void handleInterrupt();
	inline unsigned long waitFinish(uint32_t timeout_ms);


public:
	RecursiveMutex *getLock() { return &_lock; }

	static I2CController* get(controller_t controller);

	void setup(GPIOPin sda, GPIOPin scl, speed_t speed, bool doEnableInterrupts = true, uint32_t defaultTimeout=0xFFFFFFFF);
	void enableInterrupts(bool enableTimeoutInterrupt=true, bool enableDataInterrupt=true);

	virtual unsigned long read(uint8_t addr, void *buf, int count, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long read8(uint8_t addr, uint8_t *data, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write(uint8_t addr, const void *buf, int count, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write8(uint8_t addr, uint8_t data, bool sendStartCondition=true, bool sendStopCondition=true);

	virtual unsigned long writeRead(
		uint8_t addr,
		uint8_t *writeBuf, uint8_t writeCount,
		uint8_t *readBuf, uint8_t readCount,
		bool sendStartCondition=true,
		bool sendStopCondition=true
	);

	virtual unsigned long write8read(uint8_t addr, uint8_t writeData, uint8_t *readBuf, int readCount, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long  write8read8(uint8_t addr, uint8_t data_w, uint8_t *data_r, bool sendStartCondition=true, bool sendStopCondition=true);

	virtual unsigned long read16(uint8_t addr, uint16_t *data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long read32(uint8_t addr, uint32_t *data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write16(uint8_t addr, uint16_t data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write32(uint8_t addr, uint32_t data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);

	virtual unsigned long write8read16(uint8_t addr, uint8_t data_w, uint16_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write8read32(uint8_t addr, uint8_t data_w, uint32_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write16read(uint8_t addr, uint16_t writeData, uint8_t *readBuf, int readCount, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write16read8(uint16_t addr,  uint16_t data_w, uint8_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write16read16(uint16_t addr, uint16_t data_w, uint16_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write16read32(uint16_t addr, uint16_t data_w, uint32_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
};

#endif /* I2CCONTROLLER_H_ */
