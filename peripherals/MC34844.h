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

	typedef enum {OVP_11V, OVP_15V, OVP_19V, OVP_23V, OVP_27V, OVP_31V, OVP_35V, OVP_39V, OVP_43V, OVP_47V, OVP_51V, OVP_55V, OVP_59V, OVP_62V}
		ovp_t;

	MC34844();
	void setup(I2CController *i2c, GPIOPin chipSelect, GPIOPin pwmPin, uint16_t channelMask, ovp_t ovp=OVP_62V, uint8_t address=0x76);
	int setDimmungGlobal(uint8_t value);
	int setDimmungChannel(uint8_t channel, uint8_t value);
	void forceOff(bool off);
private:

	void init();
	void initRegisters();
	void disableChannel(uint8_t channel);
	void enableChannel(uint8_t channel);
	void setRegisterValue(uint8_t reg, uint8_t value);
	bool isChannelDisabled(uint8_t channel) { return !_channelStates[channel]; }
	bool isChannelEnabled(uint8_t channel) { return _channelStates[channel]; }
	GPIOPin _chipSelect;
	GPIOPin _pwmPin;

	I2CController *_i2c;
	uint8_t _address;
	//uint16_t _channelsUsageMask;
	bool _forcedOff;
	uint8_t _lastValue;
	bool _channelStates[10];
	uint8_t _reg8, _reg9; // values of register8 and register9
	ovp_t _ovp;
};

#endif /* MC34844_H_ */
