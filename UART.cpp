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
#include <stdarg.h>

#include <openstella/OS/Mutex.h>
#include <openstella/OS/Task.h>

#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/inc/hw_uart.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_ints.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/uart.h>
#include <StellarisWare/driverlib/gpio.h>
#include <StellarisWare/driverlib/interrupt.h>
#include <StellarisWare/driverlib/rom.h>
#include <StellarisWare/driverlib/rom_map.h>

#include "openstelladefaults.h"


UARTController *UARTController::_instances[] = { 0, 0, 0 };

UARTController *UARTController::get(controller_num_t num)
{
	if (!_instances[num]) {
		static Mutex mutex;
		MutexGuard guard(&mutex);
		_instances[num] = new UARTController(num);
	}
	return _instances[num];
}


void UART0IntHandler(void) {
	UARTController::_instances[UARTController::controller_0]->handleInterrupt();
}

void UART1IntHandler(void) {
	UARTController::_instances[UARTController::controller_1]->handleInterrupt();
}

void UART2IntHandler(void) {
	UARTController::_instances[UARTController::controller_2]->handleInterrupt();
}

UARTController::UARTController(controller_num_t num) :
	_num(num),
	_queue(OPENSTELLA_UART_QUEUESIZE),
	_enabled(false),
	_baudrate(115200),
	_wordlength(wordlength_8bit),
	_parity(parity_none),
	_stopbits(stopbits_1),
	_useRxInterrupt(true)
{
	switch (num) {
		case controller_0:
			_periph = SYSCTL_PERIPH_UART0;
			_base = UART0_BASE;
			_queue.addToRegistry((char*)"UART0");
			break;
		case controller_1:
			_periph = SYSCTL_PERIPH_UART1;
			_base = UART1_BASE;
			_queue.addToRegistry((char*)"UART1");
			break;
		case controller_2:
			_periph = SYSCTL_PERIPH_UART2;
			_base = UART2_BASE;
			_queue.addToRegistry((char*)"UART2");
			break;
	}
}

void UARTController::handleInterrupt()
{
    unsigned long ulStatus;
    ulStatus = MAP_UARTIntStatus(_base, true);

    MAP_UARTIntClear(_base, ulStatus);

    while(MAP_UARTCharsAvail(_base))
    {
    	uint8_t b = MAP_UARTCharGetNonBlocking(_base);
    	if (!_queue.sendToBackFromISR(b)) {
    		static int queue_full_err = 0;
    		queue_full_err++;
    	}
    }

}

void UARTController::enable()
{
	_enabled = true;
	MAP_UARTEnable(_base);
}

void UARTController::disable()
{
	MAP_UARTDisable(_base);
	_enabled = false;
}

void UARTController::enablePeripheral()
{
	MAP_SysCtlPeripheralEnable(_periph);
}

void UARTController::setup(GPIOPin rxpin, GPIOPin txpin, uint32_t baudrate, wordlength_t wordLength, parity_t parity, stopbits_t stopbits, bool use_rx_interrupt)
{
	_useRxInterrupt = use_rx_interrupt;
	enablePeripheral();

	if (!rxpin.isValid()) {
		switch (_num) {
			case controller_0:
				rxpin = GPIO::A[0];
				break;
			case controller_1:
				rxpin = GPIO::D[0];
				break;
			case controller_2:
				rxpin = GPIO::G[0];
				break;
		}
	}
	if (!txpin.isValid()) {
		switch (_num) {
			case controller_0:
				txpin = GPIO::A[1];
				break;
			case controller_1:
				txpin = GPIO::D[1];
				break;
			case controller_2:
				txpin = GPIO::G[1];
				break;
		}
	}

	rxpin.getPort()->enablePeripheral();
	rxpin.configure(GPIOPin::UART);

	txpin.getPort()->enablePeripheral();
	txpin.configure(GPIOPin::UART);

	switch (_num) {
		case controller_0:
			rxpin.mapAsU0RX();
			txpin.mapAsU0TX();
			if (_useRxInterrupt) {
				UARTIntRegister(UART0_BASE, UART0IntHandler);
				MAP_IntPrioritySet(INT_UART0, configDEFAULT_SYSCALL_INTERRUPT_PRIORITY);
				MAP_IntEnable(INT_UART0);
			}
			break;
		case controller_1:
			rxpin.mapAsU1RX();
			txpin.mapAsU1TX();
			if (_useRxInterrupt) {
				UARTIntRegister(UART1_BASE, UART1IntHandler);
				MAP_IntPrioritySet(INT_UART1, configDEFAULT_SYSCALL_INTERRUPT_PRIORITY);
				MAP_IntEnable(INT_UART1);
			}
			break;
		case controller_2:
			rxpin.mapAsU2RX();
			txpin.mapAsU2TX();
			if (_useRxInterrupt) {
				UARTIntRegister(UART2_BASE, UART2IntHandler);
				MAP_IntPrioritySet(INT_UART2, configDEFAULT_SYSCALL_INTERRUPT_PRIORITY);
				MAP_IntEnable(INT_UART2);
			}
			break;
	}

	MAP_UARTFIFOEnable(_base);

	if (_useRxInterrupt) {
		MAP_UARTIntEnable(_base, UART_INT_RX | UART_INT_RT);
	}

	setLineParameters(baudrate, wordLength, parity, stopbits);
}

void UARTController::setParityMode(UARTController::parity_t parity)
{
	MAP_UARTParityModeSet(_base, parity);
}

UARTController::parity_t UARTController::getParityMode(void)
{
	return (UARTController::parity_t) MAP_UARTParityModeGet(_base);
}

void UARTController::setBreakState(bool breakState)
{
	MAP_UARTBreakCtl(_base, breakState);
}

bool UARTController::isTransmitting()
{
	return (MAP_UARTBusy(_base) == false);
}

void UARTController::putChar(uint8_t c)
{
    while(HWREG(_base + UART_O_FR) & UART_FR_TXFF)
    {
    	Task::yield();
    }
    HWREG(_base + UART_O_DR) = c;
}

uint8_t UARTController::getChar(void)
{
	if (_useRxInterrupt) {
		uint8_t result = 0;
		while (!_queue.receive(&result)) {
			static int receive_err = 0;
			receive_err++;
		}
		return result;
	} else {
	    while(HWREG(_base + UART_O_FR) & UART_FR_RXFE) {
	    	Task::yield();
	    }
	    return(HWREG(_base + UART_O_DR));
	}
}

void UARTController::enableFIFO()
{
	MAP_UARTFIFOEnable(_base);
}

void UARTController::disableFIFO()
{
	MAP_UARTFIFODisable(_base);
}

void UARTController::setFIFOTxLevel(fifo_tx_level_t level)
{
	MAP_UARTFIFOLevelSet(_base, level, getFIFORxLevel());
}

void UARTController::setFIFORxLevel(fifo_rx_level_t level)
{
	MAP_UARTFIFOLevelSet(_base, getFIFOTxLevel(), level);
}

UARTController::fifo_tx_level_t UARTController::getFIFOTxLevel()
{
	uint32_t rxLevel, txLevel;
	MAP_UARTFIFOLevelGet(_base, &txLevel, &rxLevel);
	return (fifo_tx_level_t) txLevel;
}

UARTController::fifo_rx_level_t UARTController::getFIFORxLevel()
{
	uint32_t rxLevel, txLevel;
	MAP_UARTFIFOLevelGet(_base, &txLevel, &rxLevel);
	return (fifo_rx_level_t) rxLevel;
}

bool UARTController::isFIFOCharAvail()
{
	return MAP_UARTCharsAvail(_base) != 0;
}

bool UARTController::isFIFOSpaceAvail()
{
	return MAP_UARTSpaceAvail(_base) != 0;
}

void UARTController::putCharNonblocking(uint8_t c)
{
	MAP_UARTCharPutNonBlocking(_base, c);
}

int16_t UARTController::getCharNonBlocking()
{
	uint8_t data;
	if (_useRxInterrupt) {
		if (_queue.receive(&data, 0)) {
			return data;
		} else {
			return -1;
		}
	} else {
		return MAP_UARTCharGetNonBlocking(_base);
	}
}

int16_t UARTController::getCharTimeout(uint32_t timeout) {
	uint8_t data;
	if (_useRxInterrupt) {
		if (_queue.receive(&data, timeout)) {
			return data;
		} else {
			return -1;
		}
	} else {
		uint32_t t_max = Task::getTime() + timeout;
		while (Task::getTime() <= t_max) {
			int result = MAP_UARTCharGetNonBlocking(_base);
			if (result!=-1) { return result; }
			Task::delay_ticks(1);
		}
		return MAP_UARTCharGetNonBlocking(_base);
	}
}


void UARTController::enableSIR(bool lowPower)
{
	disable();
	MAP_UARTEnableSIR(_base, lowPower);
	enable();
}

void UARTController::disableSIR()
{
	disable();
	MAP_UARTDisableSIR(_base);
	enable(); // TODO: check whether UART was enabled in the first place
}

uint32_t UARTController::getRxError()
{
	return MAP_UARTRxErrorGet(_base);
}

void UARTController::clearRxError()
{
	MAP_UARTRxErrorClear(_base);
}

void UARTController::enableSmartCard()
{
	UARTSmartCardEnable(_base);
}

void UARTController::disableSmartCard()
{
	UARTSmartCardDisable(_base);
}


void UARTController::write(const void * const ptr, int len) {

	uint8_t *s = (uint8_t *) ptr;
	int i = 0;

	while (len--) {
		putChar(s[i++]);
		//s++;
	}
}

void UARTController::write( const void * const string)
{
	uint8_t *s = (uint8_t *) string;
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
	setup(rxpin, txpin, baudrate, wordlength_8bit, parity_none, stopbits_1);
	enableFIFO();
	HWREG(_base + UART_O_LCTL) |= UART_LCTL_MASTER | UART_LCTL_BLEN_16T;
	HWREG(_base + UART_O_CTL)  |= UART_CTL_LIN;
}

void UARTController::setupLinSlave(uint32_t baudrate, GPIOPin rxpin, GPIOPin txpin)
{
	setup(rxpin, txpin, baudrate, wordlength_8bit, parity_none, stopbits_1, false);
	setFIFORxLevel(UARTController::fifo_rx_level_1_8);
	enableFIFO();
	HWREG(_base + UART_O_LCTL) |= UART_LCTL_BLEN_16T;
	HWREG(_base + UART_O_CTL)  |= UART_CTL_LIN;
}

void UARTController::setLineParameters(uint32_t baudrate, wordlength_t wordLength, parity_t parity, stopbits_t stopbits)
{
	bool _wasEnabled = _enabled;
	if (_enabled) {	disable(); }
	_baudrate = baudrate;
	_wordlength = wordLength;
	_parity = parity;
	_stopbits = stopbits;
	MAP_UARTConfigSetExpClk(_base, MAP_SysCtlClockGet(), baudrate, ((uint8_t) wordLength | (uint8_t) parity | (uint8_t) stopbits));
    if (_wasEnabled) { enable(); }
}

uint32_t UARTController::getBaudrate()
{
	return _baudrate;
}

UARTController::wordlength_t UARTController::getWordLength()
{
	return _wordlength;
}

UARTController::parity_t UARTController::getParity()
{
	return _parity;
}

UARTController::stopbits_t UARTController::getStopBits()
{
	return _stopbits;
}

void UARTController::printf( const char* format, ... ) {
    va_list args;
    va_start( args, format );
    int size = vsniprintf(0, 0, format, args);
    char *buf = (char*) pvPortMalloc(size+1);
    vsiprintf(buf, format, args);
    va_end( args );
    write(buf);
    vPortFree(buf);
}

/// read a number of bytes from the UART controller
/**
 * @param buf The buffer the data shall be read into
 * @param bufSize size of the buffer in bytes
 * @return the requested CANController object. newly constructed, if not requested before.
 */
void UARTController::read(void *buf, int bufSize)
{
	uint8_t *data = (uint8_t *) buf;

	while (bufSize--) {
		*(data++) = getChar();
	}
}


/// read from the UART controller until a terminating character appears in the byte stream
/**
 * at maximum, bufSize number of bytes are read if the terminator sequence doesn't appear prior to that
 * @param buf The buffer the data shall be read into
 * @param bufSize Size of the buffer in bytes
 * @param terminator Stop reading and return when this character is found.
 * @return The number of bytes read from the UART.
 */
int UARTController::readUntil(const void *buf, int bufSize, uint8_t terminator)
{
	uint8_t *data = (uint8_t *) buf;
	int numRead;
	bool terminated = false;

	for ( numRead = 1; bufSize > 0; bufSize--) {
		data[numRead-1] = getChar();
		if (data[numRead-1] == terminator) terminated = true;
		numRead++;
		if (terminated) {
			break;
		}
	}

	return (numRead - 1);
}


/// read from the UART controller until a terminating sequence of bytes appears in the stream
/**
 * at maximum, bufSize number of bytes are read if the terminator sequence doesn't appear prior to that
 * @param buf The buffer the data shall be read into
 * @param bufSize Size of the buffer in bytes
 * @param terminator Stop reading and return when this byte sequence is found.
 * @param terminatorLength Size of the terminator sequence in bytes
 * @return The number of bytes read from the UART.
 */
int UARTController::readUntil(const void *buf, int bufSize, const void * const terminator, int terminatorLength)
{
	uint8_t *data = (uint8_t *) buf;
	uint8_t *t = (uint8_t *) terminator;
	bool terminated;
	int numRead;

	for ( numRead = 1; bufSize > 0; bufSize--) {
		data[numRead-1] = getChar();

		if (numRead >= terminatorLength) {
			terminated = true;
			for (int i = terminatorLength; i > 0; i--) {
				if (t[i-1] != data[numRead - terminatorLength + i - 1]) {
					terminated = false;
					break;
				}
			}
		}
		numRead++;
		if (terminated) {
			break;
		}
	}

	return numRead - 1;
}

int UARTController::readLine(const void *buf, int bufSize)
{
	int i;
	uint8_t *data = (uint8_t *) buf;
	for (i=0; i<bufSize-1; i++) {
		data[i] = getChar();
		if ( (data[i]==10) || (data[i]==13)) break;
	}
	data[i] = 0x00;
	return i;
}

