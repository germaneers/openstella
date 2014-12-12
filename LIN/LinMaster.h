/*
 * LinMaster.h
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

#ifndef LINMASTER_H_
#define LINMASTER_H_

#include "LinSlave.h"

class LinMaster : public LinSlave {
private:

protected:
	virtual void execute(void) = 0;
	bool receiveFrame(uint8_t id, void* data, uint8_t len);
	void sendIdAndDelay(uint8_t id, uint32_t delay);

public:
	LinMaster(UARTController *uart, uint32_t baudrate, GPIOPin rxPin, GPIOPin txPin);
	void setup();

};

#endif /* LINMASTER_H_ */
