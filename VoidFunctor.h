/*
 * VoidFunctor.h
 *
 *  Created on: 23.05.2012
 *      Author: hd
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
