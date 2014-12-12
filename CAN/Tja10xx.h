/*
 * Tja10xx.h
 *
 * Copyright 2013 Germaneers GmbH
 * Copyright 2013 Hubert Denkmair (hubert.denkmair@germaneers.com)
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


#ifndef TJA10XX_H_
#define TJA10XX_H_
#include <openstella/GPIO.h>

class Tja10xx {
private:
	GPIOPin _pinEnable;
	GPIOPin _pinNotStandby;
	GPIOPin _pinNotError;
	GPIOPin _pinWake;
public:
	Tja10xx(GPIOPin pinEnable, GPIOPin pinNotStandby, GPIOPin pinNotError=GPIOPin::invalid, GPIOPin pinWake=GPIOPin::invalid);
	Tja10xx(Tja10xx* other);
	void enable(bool doWakeUp = true);
	void gotoSleepMode();
	void wakeTransceiver();
};

#endif /* TJA10XX_H_ */
