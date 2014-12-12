/*
 * Pca9544A.h
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

#ifndef PCA9544A_H_
#define PCA9544A_H_

#include <openstella/I2CController.h>
#include <openstella/AbstractI2cController.h>
#include <openstella/OS/Mutex.h>

class Pca9544AI2cChannel;

class Pca9544A
{
public:
	typedef enum { Pca9544A_CHANNEL0, Pca9544A_CHANNEL1, Pca9544A_CHANNEL2, Pca9544A_CHANNEL3, Pca9544A_CHANNEL_NONE } Pca9544aI2cChannel_t;

	Pca9544A();
	void setup(I2CController *i2c, uint8_t i2cAddress=0x70);
	bool writeToBus(Pca9544aI2cChannel_t bus, uint8_t i2cAddress, const void *data, int len, bool sendStart=true, bool sendStop=true);
	bool readFromBus(Pca9544aI2cChannel_t bus, uint8_t i2cAddress, void *data, int len, bool sendStart=true, bool sendStop=true);
	bool selectBus(Pca9544aI2cChannel_t bus);

	virtual ~Pca9544A();

	Pca9544AI2cChannel *getChannel(Pca9544aI2cChannel_t bus) { return _i2cChannels[bus]; };


private:
	I2CController *_i2c;
	Pca9544aI2cChannel_t _activeBus;
	uint8_t _i2cAddress;
	bool _initialized;
	RecursiveMutex *_lock;
	Pca9544AI2cChannel *_i2cChannels[4];


	friend class Pca9544AI2cChannel;

	virtual RecursiveMutex *getLock() { if (_initialized) {return _lock;} else { return 0; } };
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
	virtual unsigned long read16(uint8_t addr, uint16_t *data, AbstractI2cController::byteorder_t byteorder=AbstractI2cController::byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long read32(uint8_t addr, uint32_t *data, AbstractI2cController::byteorder_t byteorder=AbstractI2cController::byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write16(uint8_t addr, uint16_t data, AbstractI2cController::byteorder_t byteorder=AbstractI2cController::byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write32(uint8_t addr, uint32_t data, AbstractI2cController::byteorder_t byteorder=AbstractI2cController::byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write8read16(uint8_t addr, uint8_t data_w, uint16_t *data_r, AbstractI2cController::byteorder_t byteorder=AbstractI2cController::byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write8read32(uint8_t addr, uint8_t data_w, uint32_t *data_r, AbstractI2cController::byteorder_t byteorder=AbstractI2cController::byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write16read(uint8_t addr, uint16_t writeData, uint8_t *readBuf, int readCount, AbstractI2cController::byteorder_t byteorder=AbstractI2cController::byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write16read8(uint16_t addr,  uint16_t data_w, uint8_t *data_r, AbstractI2cController::byteorder_t byteorder=AbstractI2cController::byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write16read16(uint16_t addr, uint16_t data_w, uint16_t *data_r, AbstractI2cController::byteorder_t byteorder=AbstractI2cController::byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);
	virtual unsigned long write16read32(uint16_t addr, uint16_t data_w, uint32_t *data_r, AbstractI2cController::byteorder_t byteorder=AbstractI2cController::byteorder_big_endian, bool sendStartCondition=true, bool sendStopCondition=true);

};



/*
 * this include is intentionally put at the bottom of this file.
 * we can't put it at the top of this file, or ww'll encounter problems with
 * this the circular dependencies this class has.
 * Also, we don't want to remove it due to getChannel() returning a Pca9544aI2cChannel.
*/
#include "Pca9544aI2cChannel.h"



#endif /* PCA9544A_H_ */
