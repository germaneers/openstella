/*
 * LinearInterpolatedCharacteristicLine.h
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

// FIXME doesn't work for signed numbers?!
#ifndef LINEARINTERPOLATEDCHARACTERISTICLINE_H_
#define LINEARINTERPOLATEDCHARACTERISTICLINE_H_

#include <string.h>

template <class lookupType, class resultType, int numPoints, int minX, int maxX>
class LinearInterpolatedCharacteristicLine {
private:
	static const lookupType xStep = (maxX-minX)/(numPoints-1);
	resultType _points[numPoints];
public:
	LinearInterpolatedCharacteristicLine() {
		memset(_points, 0, sizeof(_points));
	}

	LinearInterpolatedCharacteristicLine(const resultType points[numPoints])
	{
		memcpy(_points, points, sizeof(_points));
	}

	void setPoints(const resultType points[numPoints]) {
		memcpy(_points, points, sizeof(_points));
	}

	resultType getValue(lookupType x) {
		if (x<=minX) return _points[0];
		if (x>=maxX) return _points[numPoints-1];

		x -= minX;

		int x1 = x / xStep;
		int x2 = x1 + 1;

		resultType y1 = _points[x1];
		resultType y2 = _points[x2];

		int xOffset = (x - (x1*xStep)) * 4096 / xStep;

		return y1 + (xOffset * (y2-y1) / 4096);

	}
};

#endif /* LINEARINTERPOLATEDCHARACTERISTICLINE_H_ */
