/*
 * Scaler.h
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

#ifndef SCALER_H_
#define SCALER_H_

template <class T, T minIn, T maxIn, T minOut, T maxOut> class LinearScaler {
public:
	T scale(T x) {
		if (x<=minIn) return minOut;
		if (x>=maxIn) return maxOut;
		return minOut + (((x-minIn) * (maxOut - minOut)) / (maxIn-minIn));
	}
};

template <class T> class DynamicLinearScaler {
private:
	T _minIn;
	T _maxIn;
	T _minOut;
	T _maxOut;
	bool _swapIn;

public:
	DynamicLinearScaler(T minIn, T maxIn, T minOut, T maxOut)
	{
		setRanges(minIn, maxIn, minOut, maxOut);
	}

	void setInputRange(T minIn, T maxIn)
	{
		setRanges(minIn, maxIn, _minOut, _maxOut);
	}

	void setOutputRange(T minOut, T maxOut)
	{
		setRanges(_minIn, _maxIn, minOut, maxOut);
	}

	void setRanges(T minIn, T maxIn, T minOut, T maxOut)
	{
		_minIn = minIn;
		_maxIn = maxIn;
		_minOut = minOut;
		_maxOut = maxOut;
		_swapIn  = (minIn>maxIn);
	}

	T scale(T x) {
		if (_swapIn) {
			if (x>=_minIn) return _minOut;
			if (x<=_maxIn) return _maxOut;
		} else {
			if (x<=_minIn) return _minOut;
			if (x>=_maxIn) return _maxOut;
		}
		return _minOut + (((x-_minIn) * (_maxOut - _minOut)) / (_maxIn-_minIn));
	}
};

#endif /* SCALER_H_ */
