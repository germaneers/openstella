/*
 * TCL59116Controller.h
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


#ifndef TLC59116CONTROLLER_H_
#define TLC59116CONTROLLER_H_

#include "../I2CController.h"

class TLC59116Controller {
public:
	typedef enum {
		state_off = 0,
		state_full_on = 1,
		state_pwm = 2,
		state_grouppwm = 3
	} state_t;

private:
	uint8_t _addr;
	I2CController *_i2c;
	GPIOPin _resetPin;
	uint8_t _ledoutShadow[4]; // shadow registers
	void setRegister(uint8_t reg, uint8_t value);

public:
	TLC59116Controller(I2CController *i2c, uint8_t addr, GPIOPin resetPin);
	void setup(uint8_t mode1=0x01, uint8_t mode2=0x00);
	void setPWM(uint8_t ledNum, uint8_t value);
	void setLedState(uint8_t ledNum, state_t state);
	void setLedFullOn(uint8_t ledNum, bool isOn=true);
	void setAllOn();
};

#endif /* TLC59116CONTROLLER_H_ */
