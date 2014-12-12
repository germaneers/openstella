/*
 * AbstractI2cController.h
 *
 * Copyright 2013 Germaneers GmbH
 * Copyright 2013 Stefan Rupp (stefan.rupp@germaneers.com)
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



#ifndef ABSTRACTI2CCONTROLLER_H_
#define ABSTRACTI2CCONTROLLER_H_

#include <openstella/OS/RecursiveMutex.h>
#include <stdint.h>


class AbstractI2cController {
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


	AbstractI2cController() {};

	virtual RecursiveMutex *getLock() = 0;


	virtual unsigned long read(uint8_t addr, void *buf, int count, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long read8(uint8_t addr, uint8_t *data, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long write(uint8_t addr, const void *buf, int count, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long write8(uint8_t addr, uint8_t data, bool sendStartCondition=true, bool sendStopCondition=true) = 0;

	virtual unsigned long writeRead(
		uint8_t addr,
		uint8_t *writeBuf, uint8_t writeCount,
		uint8_t *readBuf, uint8_t readCount,
		bool sendStartCondition=true,
		bool sendStopCondition=true
	) = 0;

	virtual unsigned long write8read(uint8_t addr, uint8_t writeData, uint8_t *readBuf, int readCount, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long  write8read8(uint8_t addr, uint8_t data_w, uint8_t *data_r, bool sendStartCondition=true, bool sendStopCondition=true) = 0;

	virtual unsigned long read16(uint8_t addr, uint16_t *data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long read32(uint8_t addr, uint32_t *data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long write16(uint8_t addr, uint16_t data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long write32(uint8_t addr, uint32_t data, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true) = 0;

	virtual unsigned long write8read16(uint8_t addr, uint8_t data_w, uint16_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long write8read32(uint8_t addr, uint8_t data_w, uint32_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long write16read(uint8_t addr, uint16_t writeData, uint8_t *readBuf, int readCount, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long write16read8(uint16_t addr,  uint16_t data_w, uint8_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long write16read16(uint16_t addr, uint16_t data_w, uint16_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true) = 0;
	virtual unsigned long write16read32(uint16_t addr, uint16_t data_w, uint32_t *data_r, byteorder_t byteorder=byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true) = 0;

};

#endif /* ABSTRACTI2CCONTROLLER_H_ */
