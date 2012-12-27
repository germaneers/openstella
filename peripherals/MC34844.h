/*
 * MC34844.h
 *
 * Copyright 2012 Germaneers GmbH
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


#ifndef MC34844_H_
#define MC34844_H_

#include <stdint.h>
#include "openstella/GPIO.h"
#include "openstella/I2CController.h"


class MC34844 {
public:

	MC34844();
	void setup(I2CController *i2c, GPIOPin chipSelect, GPIOPin pwmPin, uint8_t numChannels, uint8_t address=0x76);
	int setDimmung(uint8_t value);
	void forceOff(bool off);

private:

	void init();
	void initRegisters();

	GPIOPin _chipSelect;
	GPIOPin _pwmPin;

	I2CController *_i2c;
	uint8_t _address;
	uint8_t _numChannels;
	bool _forcedOff;
	uint8_t _lastValue;
};

#endif /* MC34844_H_ */
