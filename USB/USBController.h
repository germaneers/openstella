/*
 * USBController.h
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


#ifndef USBCONTROLLER_H_
#define USBCONTROLLER_H_

#include <stdint.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/usblib/usblib.h>

class USBController {
public:
	typedef enum {
		controller_0
	} controller_num_t;

	typedef enum {
		mode_device = 0,
		mode_host = 1,
		mode_otg = 2,
		mode_none = 3,
		mode_force_host = 4,
		mode_force_device = 5
	} mode_t;

private:
	static USBController *_instances[1];
	controller_num_t _num;
	uint32_t _base;
	uint32_t _int;
	uint32_t _periph;
	void(*_intHandler)(void);
	void(*_otgIntHandler)(void);
	mode_t _mode;
	USBController(controller_num_t num);
public:
	static USBController *get(controller_num_t num);
	void setMode(mode_t mode, tUSBModeCallback modeHandlerCallback=0);
	mode_t getMode();
	controller_num_t getNumber() { return _num; }
};

#endif /* USBCONTROLLER_H_ */
