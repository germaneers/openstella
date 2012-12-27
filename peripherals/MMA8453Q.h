/*
 * MMA8453Q.h
 *
 * Copyright 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
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

#ifndef MMA8453Q_H_
#define MMA8453Q_H_

#include <stdint.h>
#include "../I2CController.h"

class MMA8453Q {
public:

	typedef enum {
		Range2G = 0x00,
		Range8G = 0x01,
		Range4G = 0x02
	} RangeMode_t;

private:
	uint8_t _readAddr;
	uint8_t _writeAddr;

	I2CController *_i2c;
	uint16_t _x, _y, _z;

public:
	void set8bitRegister(uint8_t reg, uint8_t value);
	uint8_t read8bitRegister(uint8_t reg);
	uint8_t read16bitRegister(uint8_t reg);

	MMA8453Q(I2CController *i2c, bool SA0);
	bool checkWhoAmI();
	void setRange(RangeMode_t range);
	void setActive(bool isActive);
	void setLowNoiseMode(bool isLowNoiseModeActive);

	void updatePosition();
	uint16_t getLastX();
	uint16_t getLastY();
	uint16_t getLastZ();

	int16_t getLastXsigned();
	int16_t getLastYsigned();
	int16_t getLastZsigned();

	uint16_t getCurrentX();
	uint16_t getCurrentY();
	uint16_t getCurrentZ();
};

#endif /* MMA8453Q_H_ */
