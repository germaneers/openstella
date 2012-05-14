/*
 * SPI.cpp
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

#include "SPI.h"
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/ssi.h>
#include <StellarisWare/driverlib/rom.h>

SPIController* SPIController::_controllers[SPIController::CONTROLLER_COUNT];

SPIController::SPIController(controller_num_t num)
: _num(num),
  _clk(num==CONTROLLER_0 ? GPIO::A[2] : GPIO::F[2]),
  _fss(num==CONTROLLER_0 ? GPIO::A[3] : GPIO::F[3]),
  _rx (num==CONTROLLER_0 ? GPIO::A[4] : GPIO::F[4]),
  _tx (num==CONTROLLER_0 ? GPIO::A[5] : GPIO::F[5])
{
	switch (num) {
		case CONTROLLER_0:
			_base = SSI0_BASE;
			_periph = SYSCTL_PERIPH_SSI0;
			break;
		case CONTROLLER_1:
			_base = SSI1_BASE;
			_periph = SYSCTL_PERIPH_SSI1;
			break;
		default:
			break;
	}
	_read_mask = 0xFFFF;
}

void SPIController::setup()
{
	ROM_SysCtlPeripheralEnable(_periph);

	_clk.enablePeripheral();
	_fss.enablePeripheral();
	_rx.enablePeripheral();
	_tx.enablePeripheral();

	if (_num==CONTROLLER_0) {
		_clk.mapAsSSI0CLK();
		_fss.mapAsSSI0FSS();
		_rx.mapAsSSI0RX();
		_tx.mapAsSSI0TX();
	} else {
		_clk.mapAsSSI1CLK();
		_fss.mapAsSSI1FSS();
		_rx.mapAsSSI1RX();
		_tx.mapAsSSI1TX();
	}

	_clk.configure(GPIOPin::SSI);
	_fss.configure(GPIOPin::SSI);
	_rx.configure(GPIOPin::SSI);
	_tx.configure(GPIOPin::SSI);
}

void SPIController::configure(protocol_t protocol, mode_t mode, uint32_t bitrate, data_width_t data_width)
{
	_data_width = data_width;
	SSIConfigSetExpClk(_base, ROM_SysCtlClockGet(), protocol, mode, bitrate, data_width);
	SSIEnable(_base);

    // empty receive fifos
	uint32_t dummy_read;
    while(SSIDataGetNonBlocking(_base, &dummy_read)) {}


    _read_mask = 0;
    for (uint8_t i=0; i<data_width; i++) {
    	_read_mask |= (1<<i);
    }

}

SPIController* SPIController::get(controller_num_t num)
{
	if (_controllers[num]==0) {
		_controllers[num] = new SPIController(num);
	}
	return _controllers[num];
}

uint16_t SPIController::writeAndReadBlocking(uint16_t writeData)
{
	uint32_t result;
	SSIDataPut(_base, writeData);
    while(SSIBusy(_base)) {}
    SSIDataGet(_base, &result);
    return result & _read_mask;
}


