/*
 * Button.h
 *
 *  Created on: 13.11.2011
 *      Author: hd
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <vector>
#include <functional>

#include "GPIO.h"
#include "OS/Event.h"

using namespace std;

class Button
{
	private:
		GPIOPin _btn;
		bool _isActiveLow;
		bool _wasLastPressed;
		bool isPressed();

	public:
		Event onPress;
		Event onRelease;
		Event onClick;
		Event onDblClick;

		Button(GPIOPin btn, bool isActiveLow);
		void update();

};

#endif /* BUTTON_H_ */
