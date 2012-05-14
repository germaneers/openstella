/*
 * I2CController.h
 *
 * Copyright 2012 Germaneers GmbH
 * Copyright 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
 * Copyright 2012 Stefan Rupp (stefan.rupp@germaneers.com)
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
#include "OS/Mutex.h"

class I2CController {
	friend void I2C0IntHandler();
	friend void I2C1IntHandler();

public:
	enum controller_t {
		controller_0,
		controller_1,
		controller_count
	};

	enum speed_t {
		speed_100kBit,
		speed_400kBit
	};

	enum byteorder_t {
		byteorder_big_endian,
		byteorder_little_endian
	};

private:
	controller_t _controller;
	uint32_t _periph;
	uint32_t _base;

	GPIOPin _sda;
	GPIOPin _scl;

	Mutex _lock;

	static I2CController *_controllers[controller_count];
	I2CController(controller_t controller, uint32_t periph, uint32_t base);

	void handleInterrupt();
	inline unsigned long waitFinish();
	unsigned long nolock_read(uint8_t addr, uint8_t *buf, int count, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long nolock_read8(uint8_t addr, uint8_t *data, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long nolock_write(uint8_t addr, uint8_t *buf, int count, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long nolock_write8(uint8_t addr, uint8_t data, bool sendStartCondition=true, bool sendStopCondition=true);


public:
	static I2CController* get(controller_t controller);
	void setup(GPIOPin sda, GPIOPin scl, speed_t speed);

	unsigned long read(uint8_t addr, uint8_t *buf, int count, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long read8(uint8_t addr, uint8_t *data, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long write(uint8_t addr, uint8_t *buf, int count, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long write8(uint8_t addr, uint8_t data, bool sendStartCondition=true, bool sendStopCondition=true);

	unsigned long writeRead(
		uint8_t addr,
		uint8_t *writeBuf, uint8_t writeCount,
		uint8_t *readBuf, uint8_t readCount,
		bool sendStartCondition=true,
		bool sendStopCondition=true
	);

	unsigned long write8read(uint8_t addr, uint8_t writeData, uint8_t *readBuf, int readCount, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long  write8read8(uint8_t addr, uint8_t data_w, uint8_t *data_r, bool sendStartCondition=true, bool sendStopCondition=true);

	unsigned long read16(uint8_t addr, uint16_t *data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long read32(uint8_t addr, uint32_t *data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long write16(uint8_t addr, uint16_t data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long write32(uint8_t addr, uint32_t data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);

	unsigned long write8read16(uint8_t addr, uint8_t data_w, uint16_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long write8read32(uint8_t addr, uint8_t data_w, uint32_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long write16read(uint8_t addr, uint16_t writeData, uint8_t *readBuf, int readCount, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long write16read8(uint16_t addr,  uint16_t data_w, uint8_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long write16read16(uint16_t addr, uint16_t data_w, uint16_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	unsigned long write16read32(uint16_t addr, uint16_t data_w, uint32_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
};

#endif /* I2CCONTROLLER_H_ */
