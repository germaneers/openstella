/*
 * Timer.cpp
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


#include "Timer.h"
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_ints.h>
#include <StellarisWare/inc/hw_timer.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/timer.h>
#include <StellarisWare/driverlib/interrupt.h>
#include <StellarisWare/driverlib/rom.h>
#include <StellarisWare/driverlib/rom_map.h>
#include <freertos/include/FreeRTOSConfig.h>

Timer Timer::_timers[] = {
		Timer(timer_0),
		Timer(timer_1),
		Timer(timer_2),
		Timer(timer_3)
};

void Timer0Handler(void)
{
	Timer::_timers[0].handleInterrupt();
}

void Timer1Handler(void)
{
	Timer::_timers[1].handleInterrupt();
}

void Timer2Handler(void)
{
	Timer::_timers[2].handleInterrupt();
}

void Timer3Handler(void)
{
	Timer::_timers[3].handleInterrupt();
}

Timer::Timer(num_t num) :
	_num(num),
	_channel_A(new TimerChannel(num, channel_A)),
	_channel_B(new TimerChannel(num, channel_B)),
	_last_config(0)
{
}

Timer::Timer(const Timer& t) :
	_num(t._num),
	_channel_A(t._channel_A),
	_channel_B(t._channel_B),
	_interruptCallback(0)
{
}

Timer *Timer::getTimer(num_t num)
{
	return &_timers[(uint8_t) num];
}

TimerChannel *Timer::getChannel(channel_t channel)
{
	if (channel==channel_A) {
		return _channel_A;
	} else {
		return _channel_B;
	}
}

TimerChannel *Timer::getChannelA()
{
	return _channel_A;
}

TimerChannel *Timer::getChannelB()
{
	return _channel_B;
}

uint32_t Timer::getBase()
{
	switch (_num) {
		case timer_0:
			return TIMER0_BASE;
		case timer_1:
			return TIMER1_BASE;
		case timer_2:
			return TIMER2_BASE;
		case timer_3:
			return TIMER3_BASE;
		default:
			return 0;
	}
}

uint32_t Timer::getPeriph()
{
	switch (_num) {
		case timer_0:
			return SYSCTL_PERIPH_TIMER0;
		case timer_1:
			return SYSCTL_PERIPH_TIMER1;
		case timer_2:
			return SYSCTL_PERIPH_TIMER2;
		case timer_3:
			return SYSCTL_PERIPH_TIMER3;
		default:
			return 0;
	}
}

void Timer::enablePeripheral()
{
	MAP_SysCtlPeripheralEnable(getPeriph());
}

void Timer::disablePeripheral()
{
	MAP_SysCtlPeripheralDisable(getPeriph());
}

void Timer::setType(type_t type)
{
	MAP_TimerConfigure(getBase(), (uint32_t) type);
	_last_config = (uint32_t) type;
}

void Timer::setChannelType(channel_t channel, type_t type)
{
	uint32_t config = (_last_config & 0x0000FFFF) | TIMER_CFG_SPLIT_PAIR;
	if (channel==channel_A) {
		config &= ~0x00FF; // clear channel A config
		config |= type;
	} else {
		config &= ~0xFF00; // clear channel B config
		config |= type<<8;
	}
	MAP_TimerConfigure(getBase(), config);
	_last_config = config;
}

void Timer::setInvertation(invertation_t invertation)
{
	MAP_TimerControlLevel(getBase(), TIMER_BOTH, (uint8_t) invertation);
}

void Timer::setTriggerOutput(bool enableTrigger)
{
	MAP_TimerControlTrigger(getBase(), TIMER_BOTH, enableTrigger ? 1 : 0);
}

void Timer::setEventType(edge_t edge)
{
	TimerControlEvent(getBase(), TIMER_BOTH, edge);
}

void Timer::setDebugMode(debug_mode_t mode)
{
	MAP_TimerControlStall(getBase(), TIMER_BOTH, (uint8_t)mode);
}

void Timer::enable()
{
	MAP_TimerEnable(getBase(), TIMER_BOTH);
}

void Timer::disable()
{
	MAP_TimerDisable(getBase(), TIMER_BOTH);
}

void Timer::enableRTC()
{
	MAP_TimerRTCEnable(getBase());
}

void Timer::disableRTC()
{
	MAP_TimerRTCDisable(getBase());
}

void Timer::setLoadValue(uint32_t load)
{
	MAP_TimerLoadSet(getBase(), TIMER_A, load);
}

uint32_t Timer::getLoadValue()
{
	return MAP_TimerLoadGet(getBase(), TIMER_A);
}

uint32_t Timer::getValue()
{
	return MAP_TimerValueGet(getBase(), TIMER_A);
}

void Timer::setMatchValue(uint32_t match)
{
	MAP_TimerMatchSet(getBase(), TIMER_A, match);
}

uint32_t Timer::getMatchValue()
{
	return MAP_TimerMatchGet(getBase(), TIMER_A);
}

void Timer::handleInterrupt(void)
{
	if (_interruptCallback!=0) {
		_interruptCallback->call();
	}
}

TimerChannel::TimerChannel(Timer::num_t timer_num, Timer::channel_t channel) :
	_timer_num(timer_num),
	_channel(channel)
{
}

uint32_t TimerChannel::getBase()
{
	switch (_timer_num) {
		case Timer::timer_0:
			return TIMER0_BASE;
		case Timer::timer_1:
			return TIMER1_BASE;
		case Timer::timer_2:
			return TIMER2_BASE;
		case Timer::timer_3:
			return TIMER3_BASE;
		default:
			return 0;
	}
}

uint16_t TimerChannel::getChannel()
{
	if (_channel==Timer::channel_A) {
		return TIMER_A;
	} else {
		return TIMER_B;
	}
}

void TimerChannel::setType(Timer::type_t type)
{
	Timer::getTimer(_timer_num)->setChannelType(_channel, type);
}


Timer::type_t TimerChannel::getType()
{
	uint32_t reg = HWREG(getBase() + ((_channel==Timer::channel_B) ? TIMER_O_TBMR : TIMER_O_TAMR));
	reg &= 0x37;
	return (Timer::type_t) reg;
}

void TimerChannel::setInvertation(Timer::invertation_t invertation)
{
	MAP_TimerControlLevel(getBase(), getChannel(), (uint8_t) invertation);
}

void TimerChannel::setTriggerOutput(bool enableTrigger)
{
	MAP_TimerControlTrigger(getBase(), getChannel(), enableTrigger ? 1 : 0);
}

void TimerChannel::setEventType(Timer::edge_t edge)
{
	TimerControlEvent(getBase(), getChannel(), edge);
}

void TimerChannel::setDebugMode(Timer::debug_mode_t mode)
{
	MAP_TimerControlStall(getBase(), getChannel(), (uint8_t)mode);
}

void TimerChannel::setLoadValue(uint16_t load)
{
	MAP_TimerLoadSet(getBase(), getChannel(), load);
}

uint16_t TimerChannel::getLoadValue()
{
	return MAP_TimerLoadGet(getBase(), getChannel());
}

uint16_t TimerChannel::getValue()
{
	return MAP_TimerValueGet(getBase(), getChannel());
}

void TimerChannel::setMatchValue(uint16_t match)
{
	MAP_TimerMatchSet(getBase(), getChannel(), match);
}

uint16_t TimerChannel::getMatchValue()
{
	return MAP_TimerMatchGet(getBase(), getChannel());
}

void TimerChannel::enableTimer()
{
	Timer::getTimer(_timer_num)->enable();
}

void TimerChannel::disableTimer()
{
	Timer::getTimer(_timer_num)->disable();
}

void TimerChannel::configurePWM(GPIOPin pin, uint16_t maxValue, uint16_t initialValue)
{
	Timer::getTimer(_timer_num)->enablePeripheral();
	pin.enablePeripheral();
	pin.configure(GPIOPin::Timer);

	switch (_timer_num)  {
		case Timer::timer_0:
			if (_channel == Timer::channel_A) { pin.mapAsCCP0(); } else { pin.mapAsCCP1(); }
			break;
		case Timer::timer_1:
			if (_channel == Timer::channel_A) { pin.mapAsCCP2(); } else { pin.mapAsCCP3(); }
			break;
		case Timer::timer_2:
			if (_channel == Timer::channel_A) { pin.mapAsCCP4(); } else { pin.mapAsCCP5(); }
			break;
		case Timer::timer_3:
			if (_channel == Timer::channel_A) { pin.mapAsCCP6(); } else { pin.mapAsCCP7(); }
			break;
		default:
			while(1);
	}

	setType(Timer::pwm);
	setLoadValue(maxValue);
	setMatchValue(initialValue);
}

Timer *TimerChannel::getTimer()
{
	return Timer::getTimer(_timer_num);
}

void TimerChannel::configureCounter(GPIOPin pin, Timer::edge_t edgeType, uint16_t loadValue, uint16_t matchValue)
{
	Timer::getTimer(_timer_num)->enablePeripheral();
	pin.enablePeripheral();
	pin.configure(GPIOPin::Timer);

	switch (_timer_num)  {
		case Timer::timer_0:
			if (_channel == Timer::channel_A) { pin.mapAsCCP0(); } else { pin.mapAsCCP1(); }
			break;
		case Timer::timer_1:
			if (_channel == Timer::channel_A) { pin.mapAsCCP2(); } else { pin.mapAsCCP3(); }
			break;
		case Timer::timer_2:
			if (_channel == Timer::channel_A) { pin.mapAsCCP4(); } else { pin.mapAsCCP5(); }
			break;
		case Timer::timer_3:
			if (_channel == Timer::channel_A) { pin.mapAsCCP6(); } else { pin.mapAsCCP7(); }
			break;
		default:
			while(1);
	}

	setType(Timer::event_counter);
	setEventType(edgeType);
	setLoadValue(loadValue);
	setMatchValue(matchValue);

}

uint8_t TimerChannel::getChannelNumber()
{
	switch (_timer_num)  {
		case Timer::timer_0:
			return (_channel == Timer::channel_A) ? 0 : 1;
		case Timer::timer_1:
			return (_channel == Timer::channel_A) ? 2 : 3;
		case Timer::timer_2:
			return (_channel == Timer::channel_A) ? 4 : 5;
		case Timer::timer_3:
			return (_channel == Timer::channel_A) ? 6 : 7;
		default:
			while(1);
	}
}

uint32_t Timer::getInterruptNumber(channel_t channel)
{
	switch (_num)  {
		case Timer::timer_0:
			return (channel==channel_A) ? INT_TIMER0A : INT_TIMER0B;
			break;
		case Timer::timer_1:
			return (channel==channel_A) ? INT_TIMER1A : INT_TIMER1B;
			break;
		case Timer::timer_2:
			return (channel==channel_A) ? INT_TIMER2A : INT_TIMER2B;
			break;
		case Timer::timer_3:
			return (channel==channel_A) ? INT_TIMER3A : INT_TIMER3B;
			break;
		default:
			while (1);
			break;
	}
	return 0;
}


void Timer::registerInterruptHandler(void(*pfnHandler)(void), half_t channel)
{
	TimerIntRegister(getBase(), channel, pfnHandler);
}

void Timer::enableInterrupt(uint32_t flags)
{
	MAP_IntPrioritySet(getInterruptNumber(channel_A), configDEFAULT_SYSCALL_INTERRUPT_PRIORITY);
	MAP_IntPrioritySet(getInterruptNumber(channel_B), configDEFAULT_SYSCALL_INTERRUPT_PRIORITY);
	MAP_TimerIntEnable(getBase(), flags);
	IntEnable(getInterruptNumber(channel_A));
	//IntEnable(getInterruptNumber(channel_B));
}

uint32_t Timer::getInterruptStatus(bool returnMaskedStatus)
{
	return MAP_TimerIntStatus(getBase(), returnMaskedStatus);
}

void Timer::clearInterrupt(uint32_t flags)
{
	TimerIntClear(getBase(), flags);
}

void Timer::setInterruptCallback(VoidFunctorBase *callback)
{
	void(*handler)(void);
	switch (_num) {
		case timer_0:
			handler = Timer0Handler;
			break;
		case timer_1:
			handler = Timer1Handler;
			break;
		case timer_2:
			handler = Timer2Handler;
			break;
		case timer_3:
			handler = Timer3Handler;
			break;
		default:
			handler=0;
	}
	if (handler) {
		TimerIntRegister(getBase(), TIMER_BOTH, handler);
	}
	_interruptCallback = callback;
}

void Timer::setPrescaler(half_t half, uint8_t clockticks)
{
	MAP_TimerPrescaleSet(getBase(), half, clockticks);
}


