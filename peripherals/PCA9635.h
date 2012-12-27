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

#ifndef PCA9635_H_
#define PCA9635_H_

#include "../I2CController.h"

class PCA9635 {
public:
	typedef enum {
		driver_state_off,
		driver_state_on,
		driver_state_pwm,
		driver_state_group
	} driver_state_t;

	typedef enum {
		group_control_mode_dimming = 0,
		group_control_mode_blinking = 0x20
	} group_control_mode_t;

	typedef enum {
		output_change_on_stop = 0,
		output_change_on_ack  = 0x08
	} output_change_mode_t;

	typedef enum {
		output_driver_open_drain = 0,
		output_driver_totem_pole = 0x04
	} output_driver_mode_t;

	typedef enum {
		output_disable_mode_low = 0, // when !OE=1, LEDn=0
		output_disable_mode_high = 1, // if (outdrv=1) { when !OE=1, LEDn=1 }, if (outdrv=0) { when !OE=1, LEDn = high-impedance }
		output_disable_mode_high_impedance = 2 // when !OE=1, LEDn = high-impedance
	} output_disable_mode_t;

private:
	I2CController *_i2c;
	uint8_t _addr;
	GPIOPin _oePin;

	bool get8BitRegister(uint8_t reg, uint8_t *value);
	bool set8BitRegister(uint8_t reg, uint8_t value, bool doVerify=true);

	bool setRegisterBits(uint8_t reg, uint8_t clear_mask, uint8_t set_mask);

public:
	static bool softwareReset(I2CController *i2c);
	PCA9635(I2CController *i2c, uint8_t addr, GPIOPin outputEnablePin);

	void enableOutput(bool doEnable=true);
	void disableOutput() { enableOutput(false); }

	bool getModeRegister1(uint8_t *value) { return get8BitRegister(0x00, value); }
	bool getModeRegister2(uint8_t *value) { return get8BitRegister(0x01, value); }
	bool setModeRegister1(uint8_t mode1)  { return set8BitRegister(0x00, mode1); }
	bool setModeRegister2(uint8_t mode2)  { return set8BitRegister(0x01, mode2); }

	bool setSleepMode(bool doSleepMode)             { return setRegisterBits(0x00, 0x10, doSleepMode ? 0x10 : 0x00); }
	bool setRespondToSubAddress1(bool doRespond)    { return setRegisterBits(0x00, 0x08, doRespond ? 0x08 : 0x00); }
	bool setRespondToSubAddress2(bool doRespond)    { return setRegisterBits(0x00, 0x04, doRespond ? 0x04 : 0x00); }
	bool setRespondToSubAddress3(bool doRespond)    { return setRegisterBits(0x00, 0x02, doRespond ? 0x02 : 0x00); }
	bool setRespondToAllCallAddress(bool doRespond) { return setRegisterBits(0x00, 0x01, doRespond ? 0x01 : 0x00); }

	bool setGroupControlMode(group_control_mode_t mode) { return setRegisterBits(0x01, 0x20, mode); }
	bool setOutputInvertMode(bool doOutputInvert)       { return setRegisterBits(0x01, 0x10, doOutputInvert ? 0x10 : 0x00); }
	bool setOutputChangeMode(output_change_mode_t mode)	{ return setRegisterBits(0x01, 0x08, mode); }
	bool setOutputDriverMode(output_driver_mode_t mode)	{ return setRegisterBits(0x01, 0x04, mode); }
	bool setOutputDisableMode(output_disable_mode_t mode) { return setRegisterBits(0x01, 0x03, mode); }

	bool setBrightness(uint8_t led, uint8_t brightness, bool doVerify=true);
	bool setBrightnessAll(uint8_t brightness);
	bool setGroupBrightness(uint8_t brightness, bool doVerify=true) { return set8BitRegister(0x12, brightness, doVerify); }
	bool setGroupFrequency(uint8_t frequency, bool doVerify=true) { return set8BitRegister(0x13, frequency, doVerify); }

	bool setDriverState(uint8_t led, driver_state_t state);
	bool setDriverStateAll(driver_state_t state);

	bool getSubAddress1(uint8_t *value) { return get8BitRegister(0x18, value); }
	bool getSubAddress2(uint8_t *value) { return get8BitRegister(0x19, value); }
	bool getSubAddress3(uint8_t *value) { return get8BitRegister(0x1A, value); }
	bool getAllCallAddress(uint8_t *value) { return get8BitRegister(0x1B, value); }

	bool setSubAddress1(uint8_t addr) { return set8BitRegister(0x18, addr); }
	bool setSubAddress2(uint8_t addr) { return set8BitRegister(0x19, addr); }
	bool setSubAddress3(uint8_t addr) { return set8BitRegister(0x1A, addr); }
	bool setAllCallAddress(uint8_t addr) { return set8BitRegister(0x1B, addr); }

};

#endif /* PCA9635_H_ */
