/*
 * QTouch.h
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

#ifndef QTOUCH_H_
#define QTOUCH_H_

#include "../SPI.h"
#include "../GPIO.h"

class QTouch {
private:
	bool _hasEnablePin;
	SPIController *_spi;
	GPIOPin _enablePin;
	uint32_t _delay_150us;

	uint8_t writeReadOneByte(uint8_t write_data);
	uint8_t sendCommandReadOneByte(uint8_t cmd);
	uint16_t sendCommandReadTwoBytes(uint8_t cmd);

public:
	QTouch(SPIController *spi, GPIOPin enablePin);
	QTouch(SPIController *spi);

	uint16_t getAllKeys(void);
	uint16_t getKey(uint8_t key);

	void configure(void);
	void saveConfiguration();
};

#endif /* QTOUCH_H_ */
