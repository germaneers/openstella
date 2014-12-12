/*
 * Pca9544aI2cChannel.h
 *
 *  Created on: 20.11.2013
 *      Author: ruppst
 */

#ifndef PCA9544AI2CCHANNEL_H_
#define PCA9544AI2CCHANNEL_H_

#include "Pca9544A.h"

class Pca9544A;

class Pca9544AI2cChannel : public AbstractI2cController {
friend class Pca9544A;
public:

	RecursiveMutex *getLock() {return _pca9544a->_lock;};

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

private:
	Pca9544AI2cChannel(Pca9544A::Pca9544aI2cChannel_t channel, Pca9544A *pca9544a);
	virtual ~Pca9544AI2cChannel();

	Pca9544A::Pca9544aI2cChannel_t _channel;
	Pca9544A *_pca9544a;
};

#endif /* PCA9544AI2CCHANNEL_H_ */

