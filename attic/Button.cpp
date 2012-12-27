/*
 * Button.cpp
 *
 *  Created on: 13.11.2011
 *      Author: hd
 */

#include "Button.h"

Button::Button(GPIOPin btn, bool isActiveLow) :
	_btn(btn),
	_isActiveLow(isActiveLow),
	_wasLastPressed(false)
{

}

void Button::update()
{
	bool pressed = isPressed();

	if (pressed != _wasLastPressed)
	{
		if (pressed) {
			onPress.notifyObservers(0);
		}
		_wasLastPressed = pressed;
	}
}

bool Button::isPressed()
{
	return _isActiveLow ? _btn.isLow() : _btn.isHigh();
}
