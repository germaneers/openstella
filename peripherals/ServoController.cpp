/*
 * ServoController.cpp
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

#include "ServoController.h"
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/timer.h>

GPIOPin* togglePins[8] = {0,0,0,0,0,0,0,0};

inline void ServerTimerHandler(uint8_t timerNum, uint32_t base, uint32_t periph) {
	uint32_t status = TimerIntStatus(base, true);
	TimerIntClear(base, TIMER_TIMA_TIMEOUT | TIMER_TIMB_TIMEOUT);
	if (status & TIMER_TIMA_TIMEOUT) {
		static uint8_t prescale = 50;
		if (--prescale==0) {
			prescale = 50;
			TimerDisable(base, TIMER_A);
			GPIOPin *pin = togglePins[timerNum*2];
			if (pin!=0) pin->setLow();
		}
	}
	if (status & TIMER_TIMB_TIMEOUT) {
		static uint8_t prescale = 50;
		if (--prescale==0) {
			prescale = 50;
			GPIOPin *pin = togglePins[timerNum*2+1];
			if (pin!=0) pin->setLow();
		}
	}
}

void ServoTimer0Handler(void) { ServerTimerHandler(0, TIMER0_BASE, SYSCTL_PERIPH_TIMER0); }
void ServoTimer1Handler(void) { ServerTimerHandler(1, TIMER1_BASE, SYSCTL_PERIPH_TIMER1); }
void ServoTimer2Handler(void) { ServerTimerHandler(2, TIMER2_BASE, SYSCTL_PERIPH_TIMER2); }
void ServoTimer3Handler(void) { ServerTimerHandler(3, TIMER3_BASE, SYSCTL_PERIPH_TIMER3); }

ServoController::ServoController(GPIOPin pin, TimerChannel *ch):
		Task("servo", 100),
		_pin(pin), _ch(ch), _degrees(0)
{
	togglePins[ch->getChannelNumber()] = &_pin;
}

ServoController::~ServoController()
{
	togglePins[_ch->getChannelNumber()] = 0;
}

void ServoController::setDegrees(int degrees)
{
	_degrees = degrees;
}

void ServoController::execute()
{
	_pin.enablePeripheral();
	_pin.configureAsOutput();

	Timer *timer = _ch->getTimer();
	timer->enablePeripheral();
	uint8_t timerNum = _ch->getChannelNumber() / 2;
	switch (timerNum) {
		case 0:
			timer->registerInterruptHandler(ServoTimer0Handler, Timer::timer_both);
			break;
		case 1:
			timer->registerInterruptHandler(ServoTimer1Handler, Timer::timer_both);
			break;
		case 2:
			timer->registerInterruptHandler(ServoTimer2Handler, Timer::timer_both);
			break;
		case 3:
			timer->registerInterruptHandler(ServoTimer3Handler, Timer::timer_both);
			break;
	}
	timer->enableInterrupt(Timer::timer_a_timeout | Timer::timer_b_timeout);
	timer->setType(Timer::periodic);
	while(1) {
		int us = (1000*_degrees) / 270;
		_pin.setHigh();
		_ch->setLoadValue(1000+us);
		timer->enable();
		delay_ms(20);
	}
}
