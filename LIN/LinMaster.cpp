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

bool LinMaster::receiveFrame(uint8_t id, void *vdata, uint8_t len)
{
	uint8_t *data = (uint8_t*)vdata;

	uint32_t checksum = protectIdentifier(id);
	uint32_t timeout = getTime() + 3 + (2*11*1000)/_baudrate;

	while (1) { // wait for sync byte
		if (getTime() > timeout) {
			return false;
		}
		int16_t read = _lin->getCharNonBlocking();
		if (read==0x55) break;
		delay_ticks(1);
	}
	while (1) { // read id [ throw away, it's from us anyways ]
		if (getTime() > timeout) {
			return false;
		}
		int16_t read = _lin->getCharNonBlocking();
		if (read>=0) break;
		delay_ticks(1);
	}

	uint8_t recv_count = 0;
	timeout = getTime() + 5 + (len*10*1000)/_baudrate;
	while (recv_count <= len) {
		if (getTime() > timeout) {
			return false;
		}

		int16_t read;
		while ((read = _lin->getCharNonBlocking()) >= 0 ) {
			uint8_t ch = (uint8_t) read & 0xFF;
			if (recv_count==len) {
				checksum %= 0xFF;
				return ((uint8_t)(~checksum)) == ch;
			} else {
				checksum += ch;
				data[recv_count++] = ch;
			}
		}
		delay_ticks(1);

	}
	return false;
}
