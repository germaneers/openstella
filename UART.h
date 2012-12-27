/*
 * UART.h
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


#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "OS/Queue.h"
#include "OS/RecursiveMutex.h"
#include "GPIO.h"

class UARTController
{
	friend void UART0IntHandler(void);
	friend void UART1IntHandler(void);
	friend void UART2IntHandler(void);

	public:

		void handleInterrupt();

		typedef enum {
			controller_0,
			controller_1,
			controller_2
		} controller_num_t;

		typedef enum {
			wordlength_5bit = 0x00,
			wordlength_6bit = 0x20,
			wordlength_7bit = 0x40,
			wordlength_8bit = 0x60,
		} wordlength_t;

		typedef enum {
			parity_none = 0x00,
			parity_even = 0x06,
			parity_odd  = 0x02,
			parity_one  = 0x82,
			parity_zero = 0x86
		} parity_t;

		typedef enum {
			stopbits_1 = 0x00,
			stopbits_2 = 0x08
		} stopbits_t;

		typedef enum {
			fifo_tx_level_1_8 = 0x00,
			fifo_tx_level_2_8 = 0x01,
			fifo_tx_level_4_8 = 0x02,
			fifo_tx_level_6_8 = 0x03,
			fifo_tx_level_7_8 = 0x04
		} fifo_tx_level_t;

		typedef enum {
			fifo_rx_level_1_8 = 0x00,
			fifo_rx_level_2_8 = 0x08,
			fifo_rx_level_4_8 = 0x10,
			fifo_rx_level_6_8 = 0x18,
			fifo_rx_level_7_8 = 0x20
		} fifo_rx_level_t;

	private:
		controller_num_t _num;
		uint32_t _periph;
		uint32_t _base;

		Queue<uint8_t> _queue;
		RecursiveMutex _mutex;

		static UARTController *_instances[3];
		UARTController(controller_num_t num);
		UARTController(uint8_t portNumber, uint32_t periph, uint32_t base);

		bool _enabled;
		uint32_t _baudrate;
		wordlength_t _wordlength;
		parity_t _parity;
		stopbits_t _stopbits;


	public:

		static UARTController *get(controller_num_t num);

		RecursiveMutex *getMutex();
		void enablePeripheral();
		void enable();
		void disable();

		void setup(GPIOPin rxpin=GPIOPin::invalid, GPIOPin txpin=GPIOPin::invalid, uint32_t baudrate=115200, wordlength_t wordLength=wordlength_8bit, parity_t parity=parity_none, stopbits_t stopbits=stopbits_1);

		void setupLinMaster(uint32_t baudrate, GPIOPin rxpin, GPIOPin txpin);
		void setupLinSlave(uint32_t baudrate, GPIOPin rxpin, GPIOPin txpin);
		void setLineParameters(uint32_t baudrate, wordlength_t wordLength, parity_t parity, stopbits_t stopbits);
		uint32_t getBaudrate();
		wordlength_t getWordLength();
		parity_t getParity();
		stopbits_t getStopBits();


		void setParityMode(parity_t parity);
		parity_t getParityMode(void);

		void setBreakState(bool breakState);
		bool isTransmitting();

		void putChar(uint8_t c);
		uint8_t getChar();

		void enableFIFO();
		void disableFIFO();
		void setFIFOTxLevel(fifo_tx_level_t level);
		void setFIFORxLevel(fifo_rx_level_t level);
		fifo_tx_level_t getFIFOTxLevel();
		fifo_rx_level_t getFIFORxLevel();
		bool isFIFOCharAvail();
		bool isFIFOSpaceAvail();
		void putCharNonblocking(uint8_t c);
		int16_t getCharNonBlocking();

		void enableSIR(bool lowPower); // disable() enableSIR(); enable()
		void disableSIR();



		uint32_t getRxError();
		void clearRxError();

		void enableSmartCard();
		void disableSmartCard();

		// TODO: UARTClockSource
		// todo: intEnable, intDisable, intRegister, intUnregister, intStatus, intClear, txIntMode
		// todo: dmaenable, DMAdisable

		// todo: ModemControl
		// TODO: Hardware Flow Control

		// TODO: 9Bit Mode

		// TODO: loopback

		// TODO: scanf?

		void write( const void * const s);
		void write(const void * const s, int len);
		void printf( const char* format, ... );

		void read(void *buf, int bufSize);
		int readUntil(const void *buf, int bufSize, uint8_t terminator);
		int readUntil(const void *buf, int bufSize, const void * const terminator, int terminatorLength);
		int readLine(const void *buf, int bufSize);

};

#endif /* UART_H_ */
