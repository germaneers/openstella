/*
 * PWMGenerator.h
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

#ifndef PWMGENERATOR_H_
#define PWMGENERATOR_H_


#include <stdint.h>
#include "PWMChannel.h"

#ifdef HAS_PWM_GENERATORS

class PWMChannel;

class PWMGenerator {
public:
	typedef enum {
		generator_0,
		generator_1,
		generator_2,
#ifdef HAS_PWM_GENERATOR3
		generator_3
#endif
		} generator_num_t;

	typedef enum {
		div_1  = 0x00000000,
		div_2  = 0x00100000,
		div_4  = 0x00120000,
		div_8  = 0x00140000,
		div_16 = 0x00160000,
		div_32 = 0x00180000,
		div_64 = 0x001A0000
	} divisor_t;

	typedef enum {
		counting_mode_down,
		counting_mode_up_down
	} counting_mode_t;

	typedef enum  {
		sync_none,
		sync_local,
		sync_global
	} sync_t;

	typedef enum {
		channel_A = 0,
		channel_B = 1
	} channel_t;

	PWMChannel *_channels[2];

private:
	PWMGenerator(generator_num_t generator_num);
	static PWMGenerator* _generators[4];
	generator_num_t _num;
	uint32_t _periph;
	uint32_t _base;
	uint32_t _gen;
public:
	static PWMGenerator *get(generator_num_t generator_num);
	void setup(divisor_t clock_divisor, counting_mode_t counting_mode, bool stopOnDebug=true, bool syncCounting=false, sync_t modeSync=sync_none, sync_t deadbandSync=sync_none, bool faultLatched=false, bool faultMinimalPeriod=false, bool extendedFaultMode=false);
	void setPeriod(uint16_t period);
	void enable();
	PWMChannel *getChannel(channel_t channel);
};

#endif /* HAS_PWM_GENERATORS */

#endif /* PWMGENERATOR_H_ */
