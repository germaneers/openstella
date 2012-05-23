/*
 * GPIO.cpp
 *
 * Copyright 2011, 2012 Germaneers GmbH
 * Copyright 2011, 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
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

#include "GPIO.h"
#include <freertos/include/FreeRTOSConfig.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/inc/hw_ints.h>
#include <StellarisWare/driverlib/rom.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/gpio.h>
#include <StellarisWare/driverlib/interrupt.h>

GPIOPort GPIO::A(0, SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE);
GPIOPort GPIO::B(1, SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE);
GPIOPort GPIO::C(2, SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE);
GPIOPort GPIO::D(3, SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE);
GPIOPort GPIO::E(4, SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE);
GPIOPort GPIO::F(5, SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE);
GPIOPort GPIO::G(6, SYSCTL_PERIPH_GPIOG, GPIO_PORTG_BASE);
GPIOPort GPIO::H(7, SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE);
GPIOPort GPIO::J(9, SYSCTL_PERIPH_GPIOJ, GPIO_PORTJ_BASE);

GPIOPort::GPIOPort(uint8_t portNumber, uint32_t periph, uint32_t base) :
		_portNumber(portNumber), _periph(periph), _base(base) {
}

void GPIOPort::enablePeripheral(void) {
	ROM_SysCtlPeripheralEnable(_periph);
}

void GPIOPort::disablePeripheral(void) {
	ROM_SysCtlPeripheralDisable(_periph);
}

void GPIOPort::configurePins(uint8_t pins, GPIOPin::mode_t cfg) {
	uint32_t mode = GPIO_DIR_MODE_HW;
	switch (cfg) {
		case GPIOPin::ADC:
		case GPIOPin::Comparator:
		case GPIOPin::GPIOInput:
		case GPIOPin::PECIRx:
		case GPIOPin::USBAnalog:
			mode = GPIO_DIR_MODE_IN;
			break;
		case GPIOPin::GPIOOutput:
		case GPIOPin::GPIOOutputOD:
			mode = GPIO_DIR_MODE_OUT;
			break;
		default:
			break;
	}
	ROM_GPIODirModeSet(_base, pins, mode);

	uint32_t type = GPIO_PIN_TYPE_STD;
	uint32_t strength = GPIO_STRENGTH_2MA;

	switch (cfg) {
		case GPIOPin::ADC:
		case GPIOPin::Comparator:
		case GPIOPin::PECIRx:
		case GPIOPin::USBAnalog:
			type = GPIO_PIN_TYPE_ANALOG;
			break;

		case GPIOPin::CAN:
		case GPIOPin::EPI:
		case GPIOPin::EthernetLED:
		case GPIOPin::EthernetMII:
		case GPIOPin::LPC:
			strength = GPIO_STRENGTH_8MA;
			break;

		case GPIOPin::GPIOOutputOD:
		case GPIOPin::I2CSCL:
			type = GPIO_PIN_TYPE_OD;
			break;

		case GPIOPin::I2C:
			type = GPIO_PIN_TYPE_OD_WPU;
			break;

		case GPIOPin::QEI:
			type = GPIO_PIN_TYPE_STD_WPU;
			break;

/*		case GPIOPin::Fan:
		case GPIOPin::GPIOInput:
		case GPIOPin::GPIOOutput:
		case GPIOPin::I2S:
		case GPIOPin::PECITx:
		case GPIOPin::PWM:
		case GPIOPin::SSI:
		case GPIOPin::Timer:
		case GPIOPin::UART:
		case GPIOPin::USBDigital: */
		default:
			break;
	}

	ROM_GPIOPadConfigSet(_base, pins, strength, type);

}

void GPIOPort::configurePads(uint8_t pins, GPIOPin::strength_t strength, GPIOPin::padType_t padType) {
	ROM_GPIOPadConfigSet(_base, pins, strength, padType);
}

GPIOPin::strength_t GPIOPort::getPadStrength(uint8_t pin) {
	uint32_t pulStrength;
	uint32_t pulPadType;
	ROM_GPIOPadConfigGet(_base, pin, &pulStrength, &pulPadType);
	return (GPIOPin::strength_t) pulStrength;
}

GPIOPin::padType_t GPIOPort::getPadType(uint8_t pin) {
	uint32_t pulStrength;
	uint32_t pulPadType;
	ROM_GPIOPadConfigGet(_base, pin, &pulStrength, &pulPadType);
	return (GPIOPin::padType_t) pulPadType;
}

void GPIOPort::setPinsDirection(uint8_t pins, GPIOPin::direction_t dir) {
	ROM_GPIODirModeSet(_base, pins, dir);
}

GPIOPin::direction_t GPIOPort::getPinDirection(uint8_t pin) {
	return (GPIOPin::direction_t) ROM_GPIODirModeGet(_base, pin);
}

uint16_t GPIOPort::readPins(uint8_t pins) {
	return ROM_GPIOPinRead(_base, pins);
}

void GPIOPort::setPins(uint8_t pins) {
	ROM_GPIOPinWrite(_base, pins, pins);
}

void GPIOPort::writePins(uint8_t pins, uint8_t values) {
	ROM_GPIOPinWrite(_base, pins, values);
}

void GPIOPort::clearPins(uint8_t pins) {
	ROM_GPIOPinWrite(_base, pins, 0);
}

void GPIOPort::enableDMATriggerPins(uint8_t pins) {
	GPIODMATriggerEnable(_base, pins);
}

void GPIOPort::disableDMATriggerPins(uint8_t pins) {
	GPIODMATriggerDisable(_base, pins);
}

void GPIOPort::enableADCTriggerPins(uint8_t pins) {
	GPIOADCTriggerEnable(_base, pins);
}

void GPIOPort::disableADCTriggerPins(uint8_t pins) {
	GPIOADCTriggerDisable(_base, pins);
}

void GPIOPort::registerInterruptHandler(void(*intHandler)(void))
{
	GPIOPortIntRegister(_base, intHandler);
	IntPrioritySet(getInterruptNumber(), configMAX_SYSCALL_INTERRUPT_PRIORITY);
}

void GPIOPort::unregisterInterruptHandler()
{
	GPIOPortIntUnregister(_base);
}

void GPIOPort::disableInterrupt(uint8_t pins) {
	ROM_GPIOPinIntDisable(_base, pins);
}

inline uint32_t GPIOPort::getInterruptNumber()
{
	switch (_portNumber) {
		case 0: return INT_GPIOA;
		case 1: return INT_GPIOB;
		case 2: return INT_GPIOC;
		case 3: return INT_GPIOD;
		case 4: return INT_GPIOE;
		case 5: return INT_GPIOF;
		case 6: return INT_GPIOG;
		case 7: return INT_GPIOH;
		case 9: return INT_GPIOJ;
	}
	return 0;
}




GPIOPin::GPIOPin(uint8_t port, uint8_t pin) {
	_port_pin = ((port & 0x0F) << 4) | (pin & 0x0F);
}

GPIOPort *GPIOPin::getPort() {
	switch (_port_pin >> 4) {
	case 0:
		return &GPIO::A;
	case 1:
		return &GPIO::B;
	case 2:
		return &GPIO::C;
	case 3:
		return &GPIO::D;
	case 4:
		return &GPIO::E;
	case 5:
		return &GPIO::F;
	case 6:
		return &GPIO::G;
	case 7:
		return &GPIO::H;
	case 9:
		return &GPIO::J;
	default:
		while (1)
			;
	}
}

uint8_t GPIOPin::getPins() {
	return (1 << (_port_pin & 0x0F));
}

void GPIOPin::enablePeripheral() {
	getPort()->enablePeripheral();
}

void GPIOPin::configure(GPIOPin::mode_t cfg) {
	getPort()->configurePins(getPins(), cfg);
}

void GPIOPin::configurePad(GPIOPin::padType_t padType,
		GPIOPin::strength_t strength) {
	getPort()->configurePads(getPins(), strength, padType);
}

GPIOPin::strength_t GPIOPin::getPadStrength() {
	return getPort()->getPadStrength(_port_pin & 0x0F);
}

GPIOPin::padType_t GPIOPin::getPadType() {
	return getPort()->getPadType(_port_pin & 0x0F);
}

void GPIOPin::setDirection(GPIOPin::direction_t dir) {
	getPort()->setPinsDirection(getPins(), dir);
}

GPIOPin::direction_t GPIOPin::getDirection() {
	return getPort()->getPinDirection(_port_pin & 0x0F);
}

void GPIOPin::configureAsInput(padType_t padType) {
	configure(GPIOInput);
	setDirection(input);
	configurePad(padType, strength_2ma);
}

void GPIOPin::configureAsOutput(strength_t strength) {
	configure(GPIOOutput);
	setDirection(output);
	configurePad(push_pull, strength);
}

bool GPIOPin::isHigh() {
	return (getPort()->readPins(getPins()) != 0);
}

bool GPIOPin::isLow() {
	return (getPort()->readPins(getPins()) == 0);
}

void GPIOPin::setHigh() {
	getPort()->setPins(getPins());
}

void GPIOPin::setLow() {
	getPort()->clearPins(getPins());
}

void GPIOPin::set(bool b) {
	if (b)
		setHigh();
	else
		setLow();
}

void GPIOPin::enableDMATrigger() {
	getPort()->enableDMATriggerPins(getPins());
}

void GPIOPin::disableDMATrigger() {
	getPort()->disableDMATriggerPins(getPins());
}

void GPIOPin::enableADCTrigger() {
	getPort()->enableADCTriggerPins(getPins());
}

void GPIOPin::disableADCTrigger() {
	getPort()->disableADCTriggerPins(getPins());
}

void GPIOPin::enableInterrupt()
{
	getPort()->enableInterruptPins(getPins());
}

void GPIOPin::disableInterrupt() {
	getPort()->disableInterrupt(getPins());
}

void GPIOPort::setInterruptType(uint8_t pins, GPIOPin::interruptType_t intType)
{
    ROM_GPIOIntTypeSet(_base, pins, intType);
}

void GPIOPin::setInterruptType(interruptType_t intType)
{
	getPort()->setInterruptType(getPins(), intType);
}

void GPIOPort::enableInterrupt()
{
	IntEnable(getInterruptNumber());
}

void GPIOPort::enableInterruptPins(uint8_t pins)
{
	ROM_GPIOPinIntEnable(_base, pins);
}

void GPIOPin::clearInterrupt()
{
	getPort()->clearInterruptPins(getPins());
}

void GPIOPort::clearInterruptPins(uint8_t pins)
{
	ROM_GPIOPinIntClear(_base, pins);
}
