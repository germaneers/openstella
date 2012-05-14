/*
 * MovingAverage.h
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
#ifndef MOVINGAVERAGE_H_
#define MOVINGAVERAGE_H_

template <class valueType, class sumType, valueType initialValue, int bufferSize>
class MovingAverage {
	private:
		valueType buf[bufferSize];
		sumType sum;
		int pos;

	public:

		MovingAverage() :
			sum(0),
			pos(0)
		{
			for (int i=0; i<bufferSize; i++) {
				sum += initialValue;
				buf[i] = initialValue;
			}
		}

		void push(valueType value)
		{
			++pos %= bufferSize;
			sum -= buf[pos];
			buf[pos] = value;
			sum += value;
		}

		valueType avg() {
			return sum / bufferSize;
		}
};

#endif /* MOVINGAVERAGE_H_ */
