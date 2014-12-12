/*
 * LinSlave.h
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

#ifndef LINSLAVE_H_
#define LINSLAVE_H_

#include <openstella/OS/Task.h>
#include <openstella/UART.h>

class LinSlave : public Task {
protected:
	UARTController *_lin;
	uint32_t _baudrate;
	GPIOPin _rxPin;
	GPIOPin _txPin;

	uint8_t protectIdentifier(uint8_t id);
	uint8_t unprotectIdentifier(uint8_t id);

	void sendFrame(uint8_t id, void* data, uint8_t len);
	bool receiveFrame(uint8_t id, void* data, uint8_t len);

	virtual void onIdReceived(uint8_t id) {};
	virtual void execute(void);
public:
	LinSlave(UARTController *uart, uint32_t baudrate, GPIOPin rxPin, GPIOPin txPin, char *taskName="LIN", uint16_t taskStackSize=250, uint8_t taskPriority=1);
	void setup();
};

#endif /* LINSLAVE_H_ */
