/*
 * TMP175.h
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
#ifndef TMP175_H_
#define TMP175_H_

#include <stdint.h>
#include "../I2CController.h"

class TMP175;

class TMP175Result {
	friend class TMP175;
	private:
		bool _valid;
		int16_t _temperature;
		TMP175Result(bool isValid, int16_t temperature=0) { _valid = isValid; _temperature = temperature; }
	public:
		TMP175Result() { _valid = false; _temperature = 0xFFFF; }
		int16_t getTemperatureRaw() { return _temperature; }
		int16_t getTemperatureCelsius()   { return _temperature / 16; }
		bool isValid() { return _valid; }
};

class TMP175 {
public:
	typedef enum {
		pin_state_high,
		pin_state_low,
		pin_state_float
	} pin_state_t;

	typedef enum {
		resolution_9bits  = 0,
		resolution_10bits = 1,
		resolution_11bits = 2,
		resolution_12bits = 3
	} resolution_t;

	typedef enum {
		alert_polarity_active_low = 0,
		alert_polarity_active_high = 1
	} alert_polarity_t;

	typedef enum {
		fault_queue_1_fault  = 0,
		fault_queue_2_faults = 1,
		fault_queue_4_faults = 2,
		fault_queue_6_faults = 3
	} fault_queue_t;

private:
	I2CController *_i2c;
	uint8_t _addr;

	bool read8bitRegister(uint8_t reg, uint8_t *value);
	bool write8bitRegister(uint8_t reg, uint8_t value, bool doVerify=true);

	bool read16bitRegister(uint8_t reg, uint16_t *value);
	bool write16bitRegister(uint8_t reg, uint16_t value, bool doVerify=true);

	TMP175Result getTemperatureRegister(uint8_t reg);
	bool setTemperatureRegisterCelsius(uint8_t reg, int temp_celsius);
	bool setTemperatureRegisterRaw(uint8_t reg, int temp_raw);

	bool setStatusBits(uint8_t clear_mask, uint8_t set_mask);

public:
	TMP175(I2CController *i2c, pin_state_t a0, pin_state_t a1, pin_state_t a2);
	TMP175(I2CController *i2c, uint8_t addr);

	static uint8_t calcAddress(pin_state_t a0, pin_state_t a1, pin_state_t a2);

	bool getStatusRegister(uint8_t *status);
	bool setStatusRegister(uint8_t status);

	bool setShutdownMode(bool isShutdownMode);
	bool setThermostatMode(bool isThermostatMode);
	bool setAlertPolarity(alert_polarity_t mode);
	bool setFaultQueue(fault_queue_t mode);
	bool setOneShotMode(bool isOneShotMode);
	bool setResolution(resolution_t resolution);

	bool setLowTemperatureRaw(int temp_raw);
	bool setHighTemperatureRaw(int temp_raw);
	bool setLowTemperatureCelsius(int temp_celsius);
	bool setHighTemperatureCelsius(int temp_celsius);

	TMP175Result getTemperature();
	TMP175Result getLowTemperature();
	TMP175Result getHighTemperature();

};

#endif /* TMP175_H_ */
