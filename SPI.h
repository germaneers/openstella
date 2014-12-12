/*
 * SPI.h
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


#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <openstella/GPIO.h>
#include <openstella/OS/Queue.h>
#include <openstella/OS/Semaphore.h>
#include <openstella/UDMAController.h>

extern "C" {
	void SSI0IntHandler(void);
	void SSI1IntHandler(void);
}

class SPIController {
	friend void SSI0IntHandler(void);
	friend void SSI1IntHandler(void);
public:

	typedef enum {
		CONTROLLER_0,
		CONTROLLER_1,
		CONTROLLER_COUNT
	} controller_num_t;

	typedef enum {
		motorola_0 = 0x00000000, // polarity 0, phase 0
		motorola_1 = 0x00000002, // polarity 0, phase 1
		motorola_2 = 0x00000001, // polarity 1, phase 0
		motorola_3 = 0x00000003, // polarity 1, phase 1
		ti         = 0x00000010,
		microwire  = 0x00000020
	} protocol_t;

	typedef enum {
		master = 0,
		slave  = 1,
		slave_no_output = 2
	} mode_t;

	typedef enum {
		data_width_4bit = 4,
		data_width_5bit = 5,
		data_width_6bit = 6,
		data_width_7bit = 7,
		data_width_8bit = 8,
		data_width_9bit = 9,
		data_width_10bit = 10,
		data_width_11bit = 11,
		data_width_12bit = 12,
		data_width_13bit = 13,
		data_width_14bit = 14,
		data_width_15bit = 15,
		data_width_16bit = 16
	} data_width_t;

private:
	controller_num_t _num;
	uint32_t _periph;
	uint32_t _base;
	data_width_t _data_width;
	uint16_t _read_mask;
	uint32_t _dummyRx;
	uint32_t _dummyTx;

	GPIOPin _clk;
	GPIOPin _fss;
	GPIOPin _rx;
	GPIOPin _tx;
	GPIOPin _interruptPin;

	mode_t _mode;
	bool _dmaTxActive;
	bool _dmaRxActive;
	Semaphore _dmaTxReadySema;
	Semaphore _dmaRxReadySema;
	Queue<uint8_t> _rxData;

	UDMAController *_dma;
	UDMAController::channel_t _dmaRxChannel;
	UDMAController::channel_t _dmaTxChannel;

	static SPIController* _controllers[CONTROLLER_COUNT];
	SPIController(controller_num_t num);
	void setupHardware();
	void handleInterrupt();

public:
	void enablePeripheral();
	void enableInterrupt();
	void disableInterrupt();

	void setup(GPIOPin clk=GPIOPin(), GPIOPin rx=GPIOPin(), GPIOPin tx=GPIOPin(), GPIOPin fss=GPIOPin(), GPIOPin interrupt=GPIOPin());
	void configure(protocol_t protocol, mode_t mode, uint32_t bitrate, data_width_t data_width);
	void reconfigure(protocol_t protocol, mode_t mode, uint32_t bitrate, data_width_t data_width);

	int write(const void *buf, int len);
	int read(void *buf, int len);
	int writeAndRead(const void *writeBuf, void *readBuf, int len);
	int writeNoRead(const void *buf, int len);
	int writeNoDMA(const void *buf, int len);
	uint8_t readByte();

	bool isTxFIFOSpaceAvail();
	bool isTxFIFOEmpty();
	bool isBusy();

	void waitFinish();


/*	uint8_t readByte();
	void writeByte(uint8_t data);
	uint8_t writeAndReadByte(uint8_t writeData);
	int readByteFIFO(uint32_t *data); */

	static SPIController* get(controller_num_t num);

};


#endif /* SPI_H_ */
