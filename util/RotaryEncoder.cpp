/*
 * RotaryEncoder.cpp
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

#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(type_t type)
: _delta(0),
  _last(0),
  _table({ 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0 }),
  _type(type),
  _isInitialized(false)
{
}

void RotaryEncoder::update(bool a, bool b)
{
	if (!_isInitialized) {
		_last = 0;
		if (a) _last = 3;
		if (b) _last ^= 1;
		_delta = 0;
		_isInitialized = true;
	} else {
		int8_t x = 0;
		int8_t diff = 0;

		if (a) x = 3;
		if (b) x ^= 1;
		diff = _last - x;
		if( diff & 1 ) {
			_last = x;
			_delta += (diff & 2) - 1;
		}

	}
}

int RotaryEncoder::read()
{

	int result;

	switch (_type) {
		case single_step:
			result =  _delta;
			_delta = 0;
			break;
		case double_step:
			result =  (_delta >> 1);
			_delta &= 1;
			break;
		case quad_step:
		default:
			result =  (_delta >> 2);
			_delta &= 3;
			break;
	}
	return result;
}
