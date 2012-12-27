/*
 * PWMGenerator.cpp
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

#include "PWMGenerator.h"
#include "PWMChannel.h"

#include <openstella/OS/Mutex.h>

#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/pwm.h>

PWMGenerator* PWMGenerator::_generators[3] = {0,0,0};

PWMGenerator::PWMGenerator(generator_num_t generator_num)
	: _num(generator_num), _periph(SYSCTL_PERIPH_PWM), _base(PWM_BASE)
{
	switch (_num) {
		case generator_0:
			_gen = PWM_GEN_0;
			_channels[0] = new PWMChannel(this, PWMChannel::channel_0);
			_channels[1] = new PWMChannel(this, PWMChannel::channel_1);
			break;
		case generator_1:
			_gen = PWM_GEN_1;
			_channels[0] = new PWMChannel(this, PWMChannel::channel_2);
			_channels[1] = new PWMChannel(this, PWMChannel::channel_3);
			break;
		case generator_2:
			_gen = PWM_GEN_2;
			_channels[0] = new PWMChannel(this, PWMChannel::channel_4);
			_channels[1] = new PWMChannel(this, PWMChannel::channel_5);
			break;
	}
}

PWMGenerator *PWMGenerator::get(generator_num_t generator_num)
{
	static Mutex mutex;
	MutexGuard guard(&mutex);
	if (_generators[generator_num]==0)
	{
		_generators[generator_num] = new PWMGenerator(generator_num);
	}
	return _generators[generator_num];
}

void PWMGenerator::setup(divisor_t clock_divisor, counting_mode_t counting_mode, bool stopOnDebug, bool syncCounting, sync_t modeSync, sync_t deadbandSync, bool faultLatched, bool faultMinimalPeriod, bool extendedFaultMode)
{
	SysCtlPWMClockSet(clock_divisor);
	SysCtlPeripheralEnable(_periph);

	uint32_t config = 0;

	config |= (counting_mode==counting_mode_down) ? PWM_GEN_MODE_DOWN : PWM_GEN_MODE_UP_DOWN;
	config |= (stopOnDebug) ? PWM_GEN_MODE_DBG_STOP : PWM_GEN_MODE_DBG_RUN;
	config |= (syncCounting) ? PWM_GEN_MODE_SYNC : PWM_GEN_MODE_NO_SYNC;

	switch (modeSync) {
		case sync_none:   config |= PWM_GEN_MODE_GEN_NO_SYNC; break;
		case sync_local:  config |= PWM_GEN_MODE_GEN_SYNC_LOCAL; break;
		case sync_global: config |= PWM_GEN_MODE_GEN_SYNC_GLOBAL; break;
	}
	switch (deadbandSync) {
		case sync_none:   config |= PWM_GEN_MODE_DB_NO_SYNC; break;
		case sync_local:  config |= PWM_GEN_MODE_DB_SYNC_LOCAL; break;
		case sync_global: config |= PWM_GEN_MODE_DB_SYNC_GLOBAL; break;
	}

	config |= (faultLatched) ? PWM_GEN_MODE_FAULT_LATCHED : PWM_GEN_MODE_FAULT_UNLATCHED;
	config |= (faultMinimalPeriod) ? PWM_GEN_MODE_FAULT_MINPER : PWM_GEN_MODE_FAULT_NO_MINPER;
	config |= (extendedFaultMode) ? PWM_GEN_MODE_FAULT_EXT : PWM_GEN_MODE_FAULT_LEGACY;

	PWMGenConfigure(_base, _gen, config);
}

void PWMGenerator::setPeriod(uint16_t period)
{
    PWMGenPeriodSet(_base, _gen, period);
}

void PWMGenerator::enable()
{
    PWMGenEnable(_base, _gen);
}

PWMChannel *PWMGenerator::getChannel(channel_t channel)
{
	return _channels[channel];
}




