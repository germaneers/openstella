/*
 * RotaryEncoder.h
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

#ifndef ROTARYENCODER_H_
#define ROTARYENCODER_H_

#include <openstella/GPIO.h>

class RotaryEncoder {
	public:
		typedef enum {
			single_step,
			double_step,
			quad_step
		} type_t;

	private:
		int _delta;
		int _last;
		int8_t _table[16];
		type_t _type;
		bool _isInitialized;

	public:
		RotaryEncoder(type_t type);
		void update(bool a, bool b);
		int read();
};

#endif /* ROTARYENCODER_H_ */
