/*
 * LinMaster.cpp
 *
 * Copyright 2012 Germaneers GmbH
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

#include "LinMaster.h"
#include "LinSlave.h"

LinMaster::LinMaster(UARTController *uart, uint32_t baudrate, GPIOPin rxPin, GPIOPin txPin)
: LinSlave(uart, baudrate, rxPin, txPin)
{
}

void LinMaster::setup()
{
	_lin->setupLinMaster(_baudrate, _rxPin, _txPin);
	_lin->enable();
}


void LinMaster::sendIdAndDelay(uint8_t id, uint32_t delay)
{
	uint32_t start = getTime();

	// hack to ensure sync break is really being sent.
	_lin->disable();
	_lin->enable();

	// send syncbreak, then sync byte
	_lin->putChar(0x55);

	// send id
	_lin->putChar(protectIdentifier(id));

	// read from slaves / send to slaves
	onIdReceived(id);

	int32_t wait = delay - (getTime()-start);
	if (wait>0) delay_ms(wait);
}
