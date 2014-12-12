/*
 * PWMChannel.cpp
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

#include "PWMChannel.h"

#ifdef HAS_PWM_GENERATORS

#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/pwm.h>

PWMChannel::PWMChannel(PWMGenerator *generator, channel_t channel)
	: _generator(generator), _base(PWM_BASE), _channel(channel), _pin(GPIO::A[0])
{
	switch (channel) {
		case channel_0:
			_out = PWM_OUT_0;
			_out_bit = PWM_OUT_0_BIT;
			_pin = GPIO::D[0];
			break;
		case channel_1:
			_out = PWM_OUT_1;
			_out_bit = PWM_OUT_1_BIT;
			_pin = GPIO::D[1];
			break;
		case channel_2:
			_out = PWM_OUT_2;
			_out_bit = PWM_OUT_2_BIT;
			_pin = GPIO::D[2];
			break;
		case channel_3:
			_out = PWM_OUT_3;
			_out_bit = PWM_OUT_3_BIT;
			_pin = GPIO::D[3];
			break;
		case channel_4:
			_out = PWM_OUT_4;
			_out_bit = PWM_OUT_4_BIT;
			_pin = GPIO::E[6];
			break;
		case channel_5:
			_out = PWM_OUT_5;
			_out_bit = PWM_OUT_5_BIT;
			_pin = GPIO::E[7];
			break;
#ifdef HAS_PWM_GENERATOR3
		case channel_6:
			_out = PWM_OUT_6;
			_out_bit = PWM_OUT_6_BIT;
			_pin = GPIO::C[4];
			break;
		case channel_7:
			_out = PWM_OUT_7;
			_out_bit = PWM_OUT_7_BIT;
			_pin = GPIO::C[6];
			break;
#endif
		}
}

void PWMChannel::setOutputState(bool enable)
{
    PWMOutputState(_base, _out_bit, enable);
}

void PWMChannel::configurePin(GPIOPin pin)
{
	_pin = pin;
	_pin.enablePeripheral();
	_pin.configureAsOutput();
	_pin.configure(GPIOPin::PWM);
	switch (_channel) {
		case channel_0:
			_pin.mapAsPWM0();
			break;
		case channel_1:
			_pin.mapAsPWM1();
			break;
		case channel_2:
			_pin.mapAsPWM2();
			break;
		case channel_3:
			_pin.mapAsPWM3();
			break;
		case channel_4:
			_pin.mapAsPWM4();
			break;
		case channel_5:
			_pin.mapAsPWM5();
			break;
#ifdef HAS_PWM_GENERATOR3
		case channel_6:
			_pin.mapAsPWM6();
			break;
		case channel_7:
			_pin.mapAsPWM7();
			break;
#endif
		}
}

void PWMChannel::setPulseWidth(uint16_t pulseWidth)
{
	PWMPulseWidthSet(_base, _out, pulseWidth);
	setOutputState(pulseWidth>0);
}

PWMGenerator *PWMChannel::getGenerator()
{
	return _generator;
}


#endif /* HAS_PWM_GENERATORS */
