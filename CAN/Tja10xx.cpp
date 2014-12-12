/*
 * Tja10xx.cpp
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


#include "Tja10xx.h"
#include <openstella/OS/Task.h>



Tja10xx::Tja10xx(GPIOPin pinEnable, GPIOPin pinNotStandby, GPIOPin pinError, GPIOPin pinWake)
  :_pinEnable(pinEnable), _pinNotStandby(pinNotStandby), _pinNotError(pinError), _pinWake(pinWake)
{
}

Tja10xx::Tja10xx(Tja10xx *other)
  : _pinEnable(other->_pinEnable),
    _pinNotStandby(other->_pinNotStandby),
    _pinNotError(other->_pinNotError),
    _pinWake(other->_pinWake)
{
}


void Tja10xx::enable(bool doWakeUp)
{
	_pinEnable.configureAsOutput();
	_pinEnable.setHigh();
	_pinNotStandby.configureAsOutput();
	_pinNotStandby.setHigh();
	if (_pinNotError.isValid()) {
		_pinNotError.configureAsInput();
	}
	if (_pinWake.isValid()) {
		_pinWake.configureAsOutput();
		_pinWake.setLow();
		if (doWakeUp) {
			wakeTransceiver();
		}
	}
}


void Tja10xx::gotoSleepMode()
{
	_pinEnable.setHigh();
	_pinNotStandby.setLow();
}


void Tja10xx::wakeTransceiver()
{
	_pinWake.setHigh();
	Task::delay_ticks(1);
	_pinWake.setLow();
}
