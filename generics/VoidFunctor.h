/*
 * VoidFunctor.h
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

#ifndef VOIDFUNCTOR_H_
#define VOIDFUNCTOR_H_

class VoidFunctorBase {
public:
	virtual void call()=0;
};

template <class TClass> class VoidFunctor : public VoidFunctorBase {
	private:
		TClass *_calledObject;
		void (TClass::*_calledMethod)();
	public:
		VoidFunctor(TClass *calledObject, void (TClass::*calledMethod)()) :
			_calledObject(calledObject),
			_calledMethod(calledMethod)
		{
		}
		virtual void call() {
			(*_calledObject.*_calledMethod)();
		}
};


#endif /* VOIDFUNCTOR_H_ */
