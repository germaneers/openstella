/*
 * ServoController.h
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

#include "PCA9635.h"


PCA9635::PCA9635(I2CController *i2c, uint8_t addr, GPIOPin outputEnablePin)
	: _i2c(i2c), _addr(addr), _oePin(outputEnablePin)
{
}

bool PCA9635::get8BitRegister(uint8_t reg, uint8_t *value)
{
	return (_i2c->write8read8(_addr, reg, value)==0);
}

bool PCA9635::set8BitRegister(uint8_t reg, uint8_t value, bool doVerify)
{
	RecursiveMutexGuard guard(_i2c->getLock());
	if (_i2c->write16(_addr, (reg<<8) | value) != 0) {
		return false;
	}

	if (doVerify) {
		uint8_t readback;
		return (get8BitRegister(reg, &readback) && (readback==value));
	} else {
		return true;
	}
}

bool PCA9635::setRegisterBits(uint8_t reg, uint8_t clear_mask, uint8_t set_mask)
{
	RecursiveMutexGuard guard(_i2c->getLock());
	uint8_t value;
	if (!get8BitRegister(reg, &value)) { return false; }
	value &= ~clear_mask;
	value |= set_mask;
	return set8BitRegister(reg, value);
}

bool PCA9635::softwareReset(I2CController *i2c)
{
	return (i2c->write16(0x03, 0xA55A, I2CController::byteorder_big_endian, true, true) == 0);
}

void PCA9635::enableOutput(bool doEnable)
{
	_oePin.enablePeripheral();
	_oePin.set(!doEnable);
	_oePin.configureAsOutput();
}

bool PCA9635::setBrightness(uint8_t led, uint8_t brightness, bool doVerify)
{
	if (led>15) return false;
	return set8BitRegister(led+2, brightness, doVerify);
}

bool PCA9635::setDriverState(uint8_t led, driver_state_t state)
{
	uint8_t reg = 0x14 + (led/4);
	uint8_t i = (led % 4) * 2;
	return setRegisterBits(reg, 0x03<<i, state<<i);
}

bool PCA9635::setBrightnessAll(uint8_t brightness)
{
	uint8_t buf[17] = {
		0x82,
		brightness, brightness, brightness, brightness,
		brightness, brightness, brightness, brightness,
		brightness, brightness, brightness, brightness,
		brightness, brightness, brightness, brightness
	};

	return (_i2c->write(_addr, buf, sizeof(buf), true, true) == 0);
}

bool PCA9635::setDriverStateAll(driver_state_t state)
{
	uint8_t cfg = (state) | (state<<2) | (state<<4) | (state<<6);
	uint8_t buf[] = { 0x94, cfg, cfg, cfg, cfg};
	return (_i2c->write(_addr, buf, sizeof(buf)) == 0);
}
