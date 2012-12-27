/*
 * PWMChannel.h
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

#ifndef PWMCHANNEL_H_
#define PWMCHANNEL_H_

#include <stdint.h>
#include <openstella/GPIO.h>

class PWMGenerator;

class PWMChannel {
	friend class PWMGenerator;
public:
	typedef enum {
		channel_0,
		channel_1,
		channel_2,
		channel_3,
		channel_4,
		channel_5
	} channel_t;
private:
	PWMGenerator *_generator;
	uint32_t _base;
	channel_t _channel;
	uint32_t _out;
	uint32_t _out_bit;
	GPIOPin _pin;
	PWMChannel(PWMGenerator *generator, channel_t channel);
public:
	void configurePin(GPIOPin pin);
	void setOutputState(bool enable);
	void setPulseWidth(uint16_t pulseWidth);
	PWMGenerator *getGenerator();
};

#endif /* PWMCHANNEL_H_ */
