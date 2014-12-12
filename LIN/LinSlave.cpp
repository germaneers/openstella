/*
 * LinSlave.cpp
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

#include "LinSlave.h"

LinSlave::LinSlave(
		UARTController *uart, uint32_t baudrate, GPIOPin rxPin, GPIOPin txPin,
		char *taskName, uint16_t taskStackSize, uint8_t taskPriority)
  : Task(taskName, taskStackSize, taskPriority),
    _lin(uart), _baudrate(baudrate), _rxPin(rxPin), _txPin(txPin)
{
}

uint8_t LinSlave::protectIdentifier(uint8_t id) {
	id &= 0x3F;
	uint8_t p0 = (id&0x01) ^ ((id&0x02)>>1) ^ ((id&0x04)>>2) ^ ((id&0x10)>>4);
	uint8_t p1 = ~( ((id&0x02)>>1) ^ ((id&0x08)>>3) ^ ((id&0x10)>>4) ^ ((id&0x20)>>5) );
	return ((p1&0x01)<<7) | ((p0&0x01)<<6) | id;
}

uint8_t LinSlave::unprotectIdentifier(uint8_t id) {
	return id & 0x3F;
}

void LinSlave::sendFrame(uint8_t id, void *vdata, uint8_t len)
{
	uint8_t *data = (uint8_t*)vdata;
	_lin->write((char*)data, len);

	uint32_t checksum = protectIdentifier(id);
	for (uint8_t i=0; i<len; i++) {
		checksum += data[i];
	}
	checksum %= 0xFF;

	_lin->putChar(~checksum);
}

bool LinSlave::receiveFrame(uint8_t id, void *vdata, uint8_t len)
{
	uint8_t *data = (uint8_t*)vdata;

	uint32_t checksum = protectIdentifier(id);
	uint32_t timeout = getTime() + 3 + (2*11*1000)/_baudrate;

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

void LinSlave::setup()
{
	_lin->setupLinSlave(_baudrate, _rxPin, _txPin);
	_lin->enable();
}

void LinSlave::execute(void)
{
	while (1) {
		if (_lin->getChar()==0x55)
		{
			uint8_t id = _lin->getChar();
			onIdReceived(unprotectIdentifier(id));
		}
	}
}





