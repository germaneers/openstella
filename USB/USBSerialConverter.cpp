/*
 * USBSerialConverter.cpp
 *
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
#include "USBSerialConverter.h"

USBSerialConverter::USBSerialConverter(UARTController *uart, USBController *controller)
  : USBCDCDevice(controller), Task("cdc2uart", 100), _uart(uart)
{
}

void USBSerialConverter::readFromUSBwriteToUARTTask()
{
	while (1) {
		if (isConnected()) {
			uint8_t ch = getChar();
			_uart->putChar(ch);
		} else {
			delay_ms(10);
		}
	}
}

void USBSerialConverter::execute()
{
	TaskFunctor<USBSerialConverter> functor(this, &USBSerialConverter::readFromUSBwriteToUARTTask);
	Task::runFunctor(&functor, "uart2cdc", 100);

	_uart->enable();

	while (1) {
		if (isConnected()) {
			uint8_t ch = _uart->getChar();
			putChar(ch);
		} else {
			delay_ms(10);
		}
	}
}

unsigned long USBSerialConverter::DeviceHandler(void *pvCBData, unsigned long ulEvent, unsigned long	 ulMsgData, void *pvMsgData)
{
	tLineCoding *lineCoding;
	switch (ulEvent) {

		case USBD_CDC_EVENT_GET_LINE_CODING:
			lineCoding = (tLineCoding*) pvMsgData;
			lineCoding->ulRate     = _uart->getBaudrate();

			switch (_uart->getWordLength()) {
				case UARTController::wordlength_5bit: lineCoding->ucDatabits = 5; break;
				case UARTController::wordlength_6bit: lineCoding->ucDatabits = 6; break;
				case UARTController::wordlength_7bit: lineCoding->ucDatabits = 7; break;
				default: lineCoding->ucDatabits = 8;
			}

			switch (_uart->getParity()) {
				case UARTController::parity_odd: lineCoding->ucParity = USB_CDC_PARITY_ODD; break;
				case UARTController::parity_even: lineCoding->ucParity = USB_CDC_PARITY_EVEN; break;
				default: lineCoding->ucParity = USB_CDC_PARITY_NONE;
			}

			switch (_uart->getStopBits()) {
				case UARTController::stopbits_2: lineCoding->ucStop = USB_CDC_STOP_BITS_2; break;
				default: lineCoding->ucStop = USB_CDC_STOP_BITS_1;
			}

			return 0;

		case USBD_CDC_EVENT_SET_LINE_CODING:
			lineCoding = (tLineCoding*) pvMsgData;

			UARTController::wordlength_t wordlength;
			UARTController::parity_t parity;
			UARTController::stopbits_t stopbits;

			switch (lineCoding->ucDatabits) {
				case 5: wordlength = UARTController::wordlength_5bit; break;
				case 6: wordlength = UARTController::wordlength_6bit; break;
				case 7: wordlength = UARTController::wordlength_7bit; break;
				default: wordlength = UARTController::wordlength_8bit; break;
			}

			switch (lineCoding->ucParity) {
				case USB_CDC_PARITY_ODD:	parity = UARTController::parity_odd; break;
				case USB_CDC_PARITY_EVEN:	parity = UARTController::parity_even; break;
				default:					parity = UARTController::parity_none; break;
			}

			switch (lineCoding->ucStop) {
				case USB_CDC_STOP_BITS_2: stopbits = UARTController::stopbits_2; break;
				default: stopbits = UARTController::stopbits_1;
			}

			_uart->setLineParameters(lineCoding->ulRate, wordlength, parity, stopbits);
			return 0;

        case USBD_CDC_EVENT_SET_CONTROL_LINE_STATE:
            break;

        case USBD_CDC_EVENT_SEND_BREAK:
        	_uart->setBreakState(true);
        	return 0;

        case USBD_CDC_EVENT_CLEAR_BREAK:
        	_uart->setBreakState(false);
        	return 0;

	}
	return USBCDCDevice::DeviceHandler(pvCBData, ulEvent, ulMsgData, pvMsgData);
}
