/*
 * UART.cpp
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


#include "UART.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/inc/hw_uart.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_ints.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/uart.h>
#include <StellarisWare/driverlib/gpio.h>
#include <StellarisWare/driverlib/interrupt.h>
#include <StellarisWare/driverlib/rom.h>

UARTController UART::UART0(0, SYSCTL_PERIPH_UART0, UART0_BASE);
UARTController UART::UART1(1, SYSCTL_PERIPH_UART1, UART1_BASE);
UARTController UART::UART2(2, SYSCTL_PERIPH_UART2, UART2_BASE);


void UART0IntHandler(void) {
	UART::UART0.handleInterrupt();
}

void UART1IntHandler(void) {
	UART::UART1.handleInterrupt();
}

void UART2IntHandler(void) {
	UART::UART2.handleInterrupt();
}


UARTController::UARTController(uint8_t portNumber, uint32_t periph, uint32_t base) :
	_portNumber(portNumber),
	_periph(periph),
	_base(base),
	_queue(64)
{
	switch (portNumber) {
		case 0:
			_queue.addToRegistry("UART0");
			break;
		case 1:
			_queue.addToRegistry("UART1");
			break;
		case 2:
			_queue.addToRegistry("UART2");
			break;
	}
}

void UARTController::handleInterrupt()
{
    unsigned long ulStatus;
    ulStatus = ROM_UARTIntStatus(_base, true);

    ROM_UARTIntClear(_base, ulStatus);

    while(ROM_UARTCharsAvail(_base))
    {
    	uint8_t b = ROM_UARTCharGetNonBlocking(_base);
    	if (!_queue.sendToBackFromISR(b)) {
    		static int queue_full_err = 0;
    		queue_full_err++;
    	}
    }

}

void UARTController::enable()
{
	ROM_UARTEnable(_base);
}

void UARTController::disable()
{
	ROM_UARTDisable(_base);
}


void UARTController::setup(uint32_t baudrate, wordlength_t wordLength, parity_t parity, stopbits_t stopbits, GPIOPin rxpin, GPIOPin txpin)
{
	ROM_SysCtlPeripheralEnable(_periph);

	rxpin.getPort()->enablePeripheral();
	rxpin.configure(GPIOPin::UART);

	txpin.getPort()->enablePeripheral();
	txpin.configure(GPIOPin::UART);

	switch (_portNumber) {
		case 0:
			rxpin.mapAsU0RX();
			txpin.mapAsU0TX();
			break;
		case 1:
			rxpin.mapAsU1RX();
			txpin.mapAsU1TX();
			break;
		case 2:
			rxpin.mapAsU2RX();
			txpin.mapAsU2TX();
			break;
	}

	switch (_portNumber) {
		case 0:
			UARTIntRegister(UART0_BASE, UART0IntHandler);
			ROM_IntPrioritySet(INT_UART0, configMAX_SYSCALL_INTERRUPT_PRIORITY);
			ROM_IntEnable(INT_UART0);
			break;
		case 1:
			UARTIntRegister(UART1_BASE, UART1IntHandler);
			ROM_IntPrioritySet(INT_UART1, configMAX_SYSCALL_INTERRUPT_PRIORITY);
			ROM_IntEnable(INT_UART1);
			break;
		case 2:
			UARTIntRegister(UART2_BASE, UART2IntHandler);
			ROM_IntPrioritySet(INT_UART2, configMAX_SYSCALL_INTERRUPT_PRIORITY);
			ROM_IntEnable(INT_UART2);
			break;
	}

	ROM_UARTFIFOEnable(_base);
	ROM_UARTIntEnable(_base, UART_INT_RX | UART_INT_RT);

	disable();
	ROM_UARTConfigSetExpClk(_base, ROM_SysCtlClockGet(), baudrate, ((uint8_t) wordLength | (uint8_t) parity | (uint8_t) stopbits));
    enable();
}

void UARTController::setup(uint32_t baudrate, wordlength_t wordLength, parity_t parity, stopbits_t stopbits)
{
	setup(baudrate, wordLength, parity, stopbits, GPIO::A[0], GPIO::A[1]);
}

void UARTController::setParityMode(UARTController::parity_t parity)
{
	ROM_UARTParityModeSet(_base, parity);
}

UARTController::parity_t UARTController::getParityMode(void)
{
	return (UARTController::parity_t) ROM_UARTParityModeGet(_base);
}

void UARTController::setBreakState(bool breakState)
{
	ROM_UARTBreakCtl(_base, breakState);
}

bool UARTController::isTransmitting()
{
	return (ROM_UARTBusy(_base) == false);
}

void UARTController::putChar(uint8_t c)
{
	ROM_UARTCharPut(_base, c);
}

uint8_t UARTController::getChar(void)
{
	//return UARTCharGet(_base);
	uint8_t result = 0;
	while (!_queue.receive(&result)) {
		static int receive_err = 0;
		receive_err++;
	}
	return result;
}

void UARTController::enableFIFO()
{
	ROM_UARTFIFOEnable(_base);
}

void UARTController::disableFIFO()
{
	ROM_UARTFIFODisable(_base);
}

void UARTController::setFIFOTxLevel(fifo_tx_level_t level)
{
	ROM_UARTFIFOLevelSet(_base, level, getFIFORxLevel());
}

void UARTController::setFIFORxLevel(fifo_rx_level_t level)
{
	ROM_UARTFIFOLevelSet(_base, getFIFOTxLevel(), level);
}

UARTController::fifo_tx_level_t UARTController::getFIFOTxLevel()
{
	uint32_t rxLevel, txLevel;
	ROM_UARTFIFOLevelGet(_base, &txLevel, &rxLevel);
	return (fifo_tx_level_t) txLevel;
}

UARTController::fifo_rx_level_t UARTController::getFIFORxLevel()
{
	uint32_t rxLevel, txLevel;
	ROM_UARTFIFOLevelGet(_base, &txLevel, &rxLevel);
	return (fifo_rx_level_t) rxLevel;
}

bool UARTController::isFIFOCharAvail()
{
	return ROM_UARTCharsAvail(_base) != 0;
}

bool UARTController::isFIFOSpaceAvail()
{
	return ROM_UARTSpaceAvail(_base) != 0;
}

void UARTController::putCharNonblocking(uint8_t c)
{
	ROM_UARTCharPutNonBlocking(_base, c);
}

int16_t UARTController::getCharNonBlocking()
{
	return ROM_UARTCharGetNonBlocking(_base);
}

void UARTController::enableSIR(bool lowPower)
{
	disable();
	ROM_UARTEnableSIR(_base, lowPower);
	enable();
}

void UARTController::disableSIR()
{
	disable();
	ROM_UARTDisableSIR(_base);
	enable(); // TODO: check whether UART was enabled in the first place
}

uint32_t UARTController::getRxError()
{
	return ROM_UARTRxErrorGet(_base);
}

void UARTController::clearRxError()
{
	ROM_UARTRxErrorClear(_base);
}

void UARTController::enableSmartCard()
{
	UARTSmartCardEnable(_base);
}

void UARTController::disableSmartCard()
{
	UARTSmartCardDisable(_base);
}


void UARTController::write(const char *s, int len) {

	//char *s = (char *) ptr;
	int i = 0;

	while (len--) {
		putChar(s[i++]);
		//s++;
	}
}

void UARTController::write(char s[0])
{
	while (s[0]!=0) {
		putChar(s[0]);
		s++;
	}
}

RecursiveMutex *UARTController::getMutex()
{
	return &_mutex;
}

void UARTController::setupLinMaster(uint32_t baudrate, GPIOPin rxpin, GPIOPin txpin)
{
	setup(baudrate, wordlength_8bit, parity_none, stopbits_1, rxpin, txpin);
	enableFIFO();
	HWREG(_base + UART_O_LCTL) |= UART_LCTL_MASTER | UART_LCTL_BLEN_16T;
	HWREG(_base + UART_O_CTL)  |= UART_CTL_LIN;
}

void UARTController::setupLinSlave(uint32_t baudrate, GPIOPin rxpin, GPIOPin txpin)
{
	setup(baudrate, wordlength_8bit, parity_none, stopbits_1, rxpin, txpin);
	enableFIFO();
	HWREG(_base + UART_O_LCTL) |= UART_LCTL_BLEN_16T;
	HWREG(_base + UART_O_CTL)  |= UART_CTL_LIN;
}

void UARTController::printf( const char* format, ... ) {
    va_list args;
    va_start( args, format );
    int size = vsniprintf(0, 0, format, args);
    char *buf = (char*) malloc(size+1);
    vsiprintf(buf, format, args);
    va_end( args );
    write(buf);
    free(buf);
}

