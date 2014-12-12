/*
 * Hysteresis.h
 *
 * Copyright 2013 Germaneers GmbH
 * Copyright 2013 Hubert Denkmair (hubert.denkmair@germaneers.com)
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

#ifndef HYSTERESIS_H_
#define HYSTERESIS_H_

/**
 * @class Hysteresis
 * @brief Apply hysteresis to signals
 *
 * @param T the data type of the signal. must support minus operator and abs()
 * @param threshold the threshold that must be exceeded for a new value to be applied
 *
 * \include HysteresisDemo.h
 */
template <class T, T threshold> class Hysteresis {
	private:
		T _lastValue;
		bool _isInitialized;
	public:
		Hysteresis() : _isInitialized(false) {}

		/// apply hysteresis function to value
		/** @return the new value, if threshold is exceeded; the old value, otherwise */
		T calculate(T value) {
			if ( (abs(value-_lastValue) > threshold)) {
				_lastValue = value;
			}
			if (!_isInitialized) {
				_lastValue = value;
				_isInitialized = true;
			}
			return _lastValue;
		}
};

#endif /* HYSTERESIS_H_ */
