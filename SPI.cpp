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
#include <StellarisWare/inc/hw_ints.h>
#include <StellarisWare/inc/hw_ssi.h>
#include <StellarisWare/inc/hw_udma.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/ssi.h>
#include <StellarisWare/driverlib/rom.h>
#include <StellarisWare/driverlib/rom_map.h>
#include <StellarisWare/driverlib/udma.h>
#include "OS/Task.h"

SPIController* SPIController::_controllers[SPIController::CONTROLLER_COUNT];


extern "C" {
	void SSI0IntHandler(void) {
		SPIController::_controllers[SPIController::CONTROLLER_0]->handleInterrupt();
	}

	void SSI1IntHandler(void) {
		SPIController::_controllers[SPIController::CONTROLLER_1]->handleInterrupt();
	}
}

SPIController::SPIController(controller_num_t num)
: _num(num),
  _data_width(data_width_8bit),
  _dummyRx(0),
  _dummyTx(0),
  _clk(num==CONTROLLER_0 ? GPIO::A[2] : GPIO::F[2]),
  _fss(num==CONTROLLER_0 ? GPIO::A[3] : GPIO::F[3]),
  _rx (num==CONTROLLER_0 ? GPIO::A[4] : GPIO::F[4]),
  _tx (num==CONTROLLER_0 ? GPIO::A[5] : GPIO::F[5]),
  _interruptPin(GPIOPin::invalid),
  _mode(master),
  _dmaTxActive(false),
  _dmaRxActive(false),
  _dmaTxReadySema(),
  _dmaRxReadySema(),
  _rxData(128)
{
	_dmaRxReadySema.take(0);
	_dmaTxReadySema.take(0);
	_dma = UDMAController::getInstance();
	switch (num) {
		case CONTROLLER_0:
			_base = SSI0_BASE;
			_periph = SYSCTL_PERIPH_SSI0;
			_dmaRxChannel = UDMAController::channel_ssi0rx;
			_dmaTxChannel = UDMAController::channel_ssi0tx;
			_dmaRxReadySema.addToRegistry((char*)"SPI0RX");
			_dmaTxReadySema.addToRegistry((char*)"SPI0TX");
			break;
		case CONTROLLER_1:
			_base = SSI1_BASE;
			_periph = SYSCTL_PERIPH_SSI1;
			_dmaRxChannel = UDMAController::channel_ssi1rx;
			_dmaTxChannel = UDMAController::channel_ssi1tx;
			_dmaRxReadySema.addToRegistry((char*)"SPI1RX");
			_dmaTxReadySema.addToRegistry((char*)"SPI1TX");
			break;
		default:
			break;
	}
	_read_mask = 0xFFFF;
}

void SPIController::handleInterrupt()
{
	int32_t higherPriorityTaskWoken = 0;

	uint32_t status = MAP_SSIIntStatus(_base, true);
    MAP_SSIIntClear(_base, status);

    uint32_t data;

    if (status) {
    	while (MAP_SSIDataGetNonBlocking(_base, &data)) {
    		_rxData.sendToBackFromISR(data, &higherPriorityTaskWoken);
    	}
    }

	uint32_t reg = HWREG(UDMA_CHIS);
    if (_dmaTxActive && (reg & (1<<(_dmaTxChannel & 0x1F)))) {
		if ( _dma->getMode(_dmaTxChannel, UDMAController::structure_primary) == UDMAController::mode_stop )
		{
			_dmaTxActive = false;
			_dmaTxReadySema.giveFromISR(&higherPriorityTaskWoken);
		}
    }

    if (_dmaRxActive && (reg & (1<<(_dmaRxChannel & 0x1F)))) {
		if ( _dma->getMode(_dmaRxChannel, UDMAController::structure_primary) == UDMAController::mode_stop )
		{
			_dmaRxActive = false;
			_dmaRxReadySema.giveFromISR(&higherPriorityTaskWoken);
		}
    }

    if (higherPriorityTaskWoken!=0) {
    	Task::yieldFromISR();
    }
}

void SPIController::enablePeripheral()
{
	MAP_SysCtlPeripheralEnable(_periph);
}

void SPIController::setupHardware()
{
    _dma->setup();
	enablePeripheral();
	MAP_SysCtlPeripheralReset(_periph);

	if (_clk.isValid()) { _clk.enablePeripheral(); }
	if (_fss.isValid()) { _fss.enablePeripheral(); }
	if (_rx.isValid())  { _rx.enablePeripheral(); }
	if (_tx.isValid())  { _tx.enablePeripheral(); }

	if (_num==CONTROLLER_0) {
		if (_clk.isValid()) { _clk.mapAsSSI0CLK(); }
		if (_fss.isValid()) { _fss.mapAsSSI0FSS(); }
		if (_rx.isValid())  { _rx.mapAsSSI0RX(); }
		if (_tx.isValid())  { _tx.mapAsSSI0TX(); }
	} else {
		if (_clk.isValid()) { _clk.mapAsSSI1CLK(); }
		if (_fss.isValid()) { _fss.mapAsSSI1FSS(); }
		if (_rx.isValid()) { _rx.mapAsSSI1RX(); }
		if (_tx.isValid()) { _tx.mapAsSSI1TX(); }
	}

	if (_clk.isValid()) { _clk.configure(GPIOPin::SSI); }
	if (_fss.isValid()) { _fss.configure(GPIOPin::SSI); }
	if (_rx.isValid()) { _rx.configure(GPIOPin::SSI); }
	if (_tx.isValid()) { _tx.configure(GPIOPin::SSI); }
	if (_interruptPin.isValid()) { _interruptPin.configureAsOutput(); _interruptPin.setHigh(); }

	switch (_num) {
		case SPIController::CONTROLLER_0:
			SSIIntRegister(_base, SSI0IntHandler);
			MAP_IntPrioritySet(INT_SSI0, configMAX_SYSCALL_INTERRUPT_PRIORITY);
			MAP_IntEnable(INT_SSI0);
			break;
		case SPIController::CONTROLLER_1:
			SSIIntRegister(_base, SSI1IntHandler);
			MAP_IntPrioritySet(INT_SSI1, configMAX_SYSCALL_INTERRUPT_PRIORITY);
			MAP_IntEnable(INT_SSI1);
			break;
		default:
			while(1);
			break;
	}

    uint32_t data;
    while (MAP_SSIDataGetNonBlocking(_base, &data)) {}

	MAP_SSIIntEnable(_base, SSI_RXFF | SSI_RXTO);
	enableInterrupt();
	_dmaTxReadySema.take(0);
	_dmaRxReadySema.take(0);

	_dma->disableChannelAttribute(_dmaTxChannel, UDMAController::attribute_all);
	_dma->disableChannelAttribute(_dmaRxChannel, UDMAController::attribute_all);
}

void SPIController::setup(GPIOPin clk, GPIOPin rx, GPIOPin tx, GPIOPin fss, GPIOPin interrupt)
{
	_clk = clk;
	_rx = rx;
	_tx = tx;
	_fss = fss;
	_interruptPin = interrupt;
	setupHardware();
}

void SPIController::configure(protocol_t protocol, mode_t mode, uint32_t bitrate, data_width_t data_width)
{
	setupHardware();
	reconfigure(protocol, mode, bitrate, data_width);
}

void SPIController::reconfigure(protocol_t protocol, mode_t mode, uint32_t bitrate, data_width_t data_width)
{
	_mode = mode;
    _data_width = data_width;
    _read_mask = 0;
    for (uint8_t i=0; i<data_width; i++) {
    	_read_mask |= (1<<i);
    }

	SSIConfigSetExpClk(_base, MAP_SysCtlClockGet(), protocol, mode, bitrate, data_width);
	SSIEnable(_base);

    while(SSIDataGetNonBlocking(_base, &_dummyRx)) {}
}


bool SPIController::isTxFIFOSpaceAvail()
{
	 return (HWREG(_base + SSI_O_SR) & SSI_SR_TNF) != 0;
}

bool SPIController::isTxFIFOEmpty()
{
	 return (HWREG(_base + SSI_O_SR) & SSI_SR_TFE) != 0;
}

bool SPIController::isBusy()
{
	return MAP_SSIBusy(_base) != 0;
}

void SPIController::waitFinish()
{
	while (isBusy()) { Task::yield(); }
}

void SPIController::enableInterrupt()
{
	MAP_IntEnable((_num==SPIController::CONTROLLER_0) ? INT_SSI0 : INT_SSI1);
}

void SPIController::disableInterrupt()
{
	MAP_IntDisable((_num==SPIController::CONTROLLER_0) ? INT_SSI0 : INT_SSI1);
}

SPIController* SPIController::get(controller_num_t num)
{
	if (_controllers[num]==0) {
		_controllers[num] = new SPIController(num);
	}
	return _controllers[num];
}

int SPIController::read(void *buf, int len)
{
	MAP_SSIDMAEnable(_base, SSI_DMA_TX | SSI_DMA_RX);

	_dma->setControlParameters(
		_dmaTxChannel, UDMAController::structure_primary,
		UDMAController::param_size_8 | UDMAController::param_arb_4 |
		UDMAController::param_src_inc_none | UDMAController::param_dst_inc_none
	);
	_dma->setControlParameters(
		_dmaRxChannel, UDMAController::structure_primary,
		UDMAController::param_size_8 | UDMAController::param_arb_4 |
		UDMAController::param_src_inc_none | UDMAController::param_dst_inc_8
	);

	_dma->setTransferParameters(_dmaTxChannel, UDMAController::structure_primary, UDMAController::mode_basic, &_dummyTx, (void *)(_base + SSI_O_DR), len);
	_dma->setTransferParameters(_dmaRxChannel, UDMAController::structure_primary, UDMAController::mode_basic, (void *)(_base + SSI_O_DR), buf, len);

	_dmaTxReadySema.take(0);
	_dmaRxReadySema.take(0);

	_dmaRxActive = true;
	_dmaTxActive = true;
	_dma->enableChannel(_dmaRxChannel);
	_dma->enableChannel(_dmaTxChannel);

	_dmaTxReadySema.take();
	_dmaRxReadySema.take();

	MAP_SSIDMADisable(_base, SSI_DMA_TX | SSI_DMA_RX);

	waitFinish();

	return len;
}

int SPIController::write(const void *buf, int len)
{
	MAP_SSIDMAEnable(_base, SSI_DMA_TX | SSI_DMA_RX);

	_dma->setControlParameters(
		_dmaTxChannel, UDMAController::structure_primary,
		UDMAController::param_size_8 | UDMAController::param_arb_4 |
		UDMAController::param_src_inc_8 | UDMAController::param_dst_inc_none
	);
	_dma->setControlParameters(
		_dmaRxChannel, UDMAController::structure_primary,
		UDMAController::param_size_8 | UDMAController::param_arb_4 |
		UDMAController::param_src_inc_none | UDMAController::param_dst_inc_none
	);

	_dma->setTransferParameters(_dmaTxChannel, UDMAController::structure_primary, UDMAController::mode_basic, (void *)buf, (void *)(_base + SSI_O_DR), len);
	_dma->setTransferParameters(_dmaRxChannel, UDMAController::structure_primary, UDMAController::mode_basic, (void *)(_base + SSI_O_DR), &_dummyRx, len);

	_dmaRxReadySema.take(0);
	_dmaTxReadySema.take(0);
	_dmaRxActive = true;
	_dmaTxActive = true;
	_dma->enableChannel(_dmaRxChannel);
	_dma->enableChannel(_dmaTxChannel);

	if (_interruptPin.isValid()) { _interruptPin.setLow(); }
	_dmaTxReadySema.take();
	_dmaTxReadySema.take();

	MAP_SSIDMADisable(_base, SSI_DMA_TX | SSI_DMA_RX);

	waitFinish();

	if (_interruptPin.isValid()) { _interruptPin.setHigh();	}

	return len;
}


int SPIController::writeAndRead(const void* writeBuf, void* readBuf, int len)
{
	MAP_SSIDMAEnable(_base, SSI_DMA_TX | SSI_DMA_RX);

	_dma->setControlParameters(
		_dmaTxChannel, UDMAController::structure_primary,
		UDMAController::param_size_8 | UDMAController::param_arb_4 |
		UDMAController::param_src_inc_8 | UDMAController::param_dst_inc_none
	);
	_dma->setTransferParameters(_dmaTxChannel, UDMAController::structure_primary, UDMAController::mode_basic, (void *)writeBuf, (void *)(_base + SSI_O_DR), len);

	_dma->setControlParameters(
		_dmaRxChannel, UDMAController::structure_primary,
		UDMAController::param_size_8 | UDMAController::param_arb_4 |
		UDMAController::param_src_inc_none | UDMAController::param_dst_inc_8
	);
	_dma->setTransferParameters(_dmaRxChannel, UDMAController::structure_primary, UDMAController::mode_basic, (void *)(_base + SSI_O_DR), readBuf, len);

	_dmaRxReadySema.take(0);
	_dmaTxReadySema.take(0);
	_dmaRxActive = true;
	_dmaTxActive = true;

	_dma->enableChannel(_dmaRxChannel);
	_dma->enableChannel(_dmaTxChannel);

	if (_interruptPin.isValid()) { _interruptPin.setLow(); }
	_dmaTxReadySema.take();
	_dmaRxReadySema.take();

	waitFinish();
	Task::delay_ms(10);

	MAP_SSIDMADisable(_base, SSI_DMA_TX | SSI_DMA_RX);

	if (_interruptPin.isValid()) { _interruptPin.setHigh(); }

	return len;
}

int SPIController::writeNoDMA(const void *buf, int len) {
	const uint8_t *p = (uint8_t*) buf;
	for (int i=0; i<len; i++) {
		MAP_SSIDataPut(_base, p[i]);
	}
	return len;
}

int SPIController::writeNoRead(const void *buf, int len)
{
	MAP_SSIDMAEnable(_base, SSI_DMA_TX);

	_dma->setControlParameters(
		_dmaTxChannel, UDMAController::structure_primary,
		UDMAController::param_size_8 | UDMAController::param_arb_4 |
		UDMAController::param_src_inc_8 | UDMAController::param_dst_inc_none
	);

	_dma->setTransferParameters(_dmaTxChannel, UDMAController::structure_primary, UDMAController::mode_basic, (void *)buf, (void *)(_base + SSI_O_DR), len);

	_dmaTxReadySema.take(0);

	_dmaTxActive = true;
	_dma->enableChannel(_dmaTxChannel);

	if (_interruptPin.isValid()) { _interruptPin.setLow(); }

	_dmaTxReadySema.take();

	if (_interruptPin.isValid()) {
		waitFinish();
		_interruptPin.setHigh();
	}

	MAP_SSIDMADisable(_base, SSI_DMA_TX);

	return len;
}

uint8_t SPIController::readByte() {
	uint8_t result;
	_rxData.receive(&result);
	return result;
}

