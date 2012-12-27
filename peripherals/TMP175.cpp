/*
 * TMP175.cpp
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
#include "TMP175.h"

TMP175::TMP175(I2CController *i2c, pin_state_t a0, pin_state_t a1, pin_state_t a2)
:_i2c(i2c)
{
	_addr = calcAddress(a0, a1, a2);
	if (_addr==0) while(1); // should never reach this!
}

TMP175::TMP175(I2CController *i2c, uint8_t addr)
:_i2c(i2c), _addr(addr)
{
}

uint8_t TMP175::calcAddress(pin_state_t a0, pin_state_t a1, pin_state_t a2)
{
    switch (a2) {
    	case pin_state_low:
    		switch (a1) {
    			case pin_state_low:
    				switch (a0) {
    					case pin_state_low:   return 0b1001000;
    					case pin_state_high:  return 0b1001001;
    					case pin_state_float: return 0b0101100;
    				}
				case pin_state_high:
					switch (a0) {
						case pin_state_low:   return 0b1001010;
						case pin_state_high:  return 0b1001011;
						case pin_state_float: return 0b0101101;
					}
				case pin_state_float:
					switch (a0) {
						case pin_state_low:   return 0b0101000;
						case pin_state_high:  return 0b0101001;
						case pin_state_float: return 0b0110101;
					}
    		}

		case pin_state_high:
			switch (a1) {
				case pin_state_low:
					switch (a0) {
						case pin_state_low:   return 0b1001100;
						case pin_state_high:  return 0b1001101;
						case pin_state_float: return 0b0101110;
					}
				case pin_state_high:
					switch (a0) {
						case pin_state_low:   return 0b1001110;
						case pin_state_high:  return 0b1001111;
						case pin_state_float: return 0b0101111;
					}
				case pin_state_float:
					switch (a0) {
						case pin_state_low:   return 0b0101010;
						case pin_state_high:  return 0b0101011;
						case pin_state_float: return 0b0110110;
					}
			}

		case pin_state_float:
			switch (a1) {
				case pin_state_low:
					switch (a0) {
						case pin_state_low:   return 0b1110000;
						case pin_state_high:  return 0b1110010;
						case pin_state_float: return 0b1110001;
					}
				case pin_state_high:
					switch (a0) {
						case pin_state_low:   return 0b1110011;
						case pin_state_high:  return 0b1110101;
						case pin_state_float: return 0b1110100;
					}
				case pin_state_float:
					switch (a0) {
						case pin_state_low:   return 0b1110110;
						case pin_state_high:  return 0b1110111;
						case pin_state_float: return 0b0110111;
					}
			}
    }
    return 0;
}

bool TMP175::read8bitRegister(uint8_t reg, uint8_t *value)
{
	return (_i2c->write8read8(_addr, reg, value)==0);
}

bool TMP175::write8bitRegister(uint8_t reg, uint8_t value, bool doVerify)
{
	RecursiveMutexGuard guard(_i2c->getLock());
	if (_i2c->write16(_addr, (reg<<8) | value) != 0) {
		return false;
	}

	if (doVerify) {
		uint8_t readback;
		return (read8bitRegister(reg, &readback) && (readback==value));
	} else {
		return true;
	}
}

bool TMP175::read16bitRegister(uint8_t reg, uint16_t *value)
{
	return (_i2c->write8read16(_addr, reg, value)==0);
}

bool TMP175::write16bitRegister(uint8_t reg, uint16_t value, bool doVerify)
{
	RecursiveMutexGuard guard(_i2c->getLock());
	uint8_t data[] = { reg, (uint8_t)(value>>8), (uint8_t)(value & 0xFF) };
	if (_i2c->write(_addr, data, sizeof(data)) !=0) {
		return false;
	}

	if (doVerify) {
		uint16_t readback;
		return (read16bitRegister(reg, &readback) && (readback==value));
	} else {
		return true;
	}
}

bool TMP175::setStatusRegister(uint8_t status)
{
	return write8bitRegister(0x01, status, true);
}

bool TMP175::getStatusRegister(uint8_t *status)
{
	return read8bitRegister(0x01, status);
}

bool TMP175::setStatusBits(uint8_t clear_mask, uint8_t set_mask)
{
	RecursiveMutexGuard guard(_i2c->getLock());
	uint8_t value;
	if (!getStatusRegister(&value)) { return false; }
	value &= ~clear_mask;
	value |= set_mask;
	return setStatusRegister(value);
}

TMP175Result TMP175::getTemperatureRegister(uint8_t reg)
{
	int16_t result;
	if (!read16bitRegister(reg, (uint16_t*)&result)) {
		return TMP175Result(false);
	} else {
		return TMP175Result(true, result/16);
	}
}

bool TMP175::setShutdownMode(bool isShutdownMode)
{
	return setStatusBits(0x01, isShutdownMode ? 0x01 : 0x00);
}

bool TMP175::setThermostatMode(bool isThermostatMode)
{
	return setStatusBits(0x02, isThermostatMode ? 0x02 : 0x00);
}

bool TMP175::setAlertPolarity(alert_polarity_t mode)
{
	return setStatusBits(0x04, (mode==alert_polarity_active_high) ? 0x04 : 0x00);
}

bool TMP175::setFaultQueue(fault_queue_t mode)
{
	return setStatusBits(0x18, mode<<3);
}

bool TMP175::setResolution(resolution_t resolution)
{
	return setStatusBits(0x60, resolution<<5);
}

bool TMP175::setOneShotMode(bool isOneShotMode)
{
	return setStatusBits(0x80, isOneShotMode ? 0x80 : 0x00);
}

TMP175Result TMP175::getTemperature()
{
	return getTemperatureRegister(0x00);
}

TMP175Result TMP175::getLowTemperature()
{
	return getTemperatureRegister(0x02);
}

TMP175Result TMP175::getHighTemperature()
{
	return getTemperatureRegister(0x03);
}

bool TMP175::setTemperatureRegisterRaw(uint8_t reg, int temp_raw)
{
	uint16_t temp = ( (uint16_t) temp_raw ) & 0x0FFF;
	if (temp_raw<0xC90) return false;
	if (temp_raw>0x7FF) return false;
	return write16bitRegister(reg, temp<<4);
}

bool TMP175::setTemperatureRegisterCelsius(uint8_t reg, int temp_celsius)
{
	int temp_raw = temp_celsius*16;
	if (temp_raw==0x800) temp_raw = 0x7FF;
	return setTemperatureRegisterRaw(reg, temp_raw);
}

bool TMP175::setLowTemperatureRaw(int temp_raw)
{
	return setTemperatureRegisterRaw(0x02, temp_raw);
}

bool TMP175::setHighTemperatureRaw(int temp_raw)
{
	return setTemperatureRegisterRaw(0x03, temp_raw);
}

bool TMP175::setLowTemperatureCelsius(int temp_celsius)
{
	return setTemperatureRegisterCelsius(0x02, temp_celsius);
}

bool TMP175::setHighTemperatureCelsius(int temp_celsius)
{
	return setTemperatureRegisterCelsius(0x03, temp_celsius);
}

