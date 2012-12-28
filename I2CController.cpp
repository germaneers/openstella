/*
 * I2CController.cpp
 *
 * Copyright 2012 Germaneers GmbH
 * Copyright 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
 * Copyright 2012 Stefan Rupp (stefan.rupp@germaneers.com)
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


#include "I2CController.h"

#include <StellarisWare/driverlib/rom.h>
#include <StellarisWare/driverlib/rom_map.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_ints.h>
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/i2c.h>
#include <StellarisWare/driverlib/interrupt.h>
#include <openstella/OS/Task.h>
#include "OS/CriticalSection.h"

void I2C0IntHandler(void) {
	I2CController::_controllers[0]->handleInterrupt();
}

void I2C1IntHandler(void) {
	I2CController::_controllers[1]->handleInterrupt();
}

I2CController::I2CController(controller_t controller, uint32_t periph, uint32_t base)
 : _controller(controller), _periph(periph), _base(base),
   _sda(GPIO::A[0]), _scl(GPIO::A[0]),
   _lock(),
   _interruptSemaphore(),
   _interruptsEnabled(false)
{
}

void I2CController::handleInterrupt()
{
	I2CMasterIntClear(_base);
	_interruptSemaphore.giveFromISR();
}

I2CController *I2CController::_controllers[controller_count];
I2CController *I2CController::get(controller_t controller)
{
	CriticalSection critical();

	if (!_controllers[controller])
	{
		switch (controller) {
			case controller_0:
				_controllers[controller] = new I2CController(controller, SYSCTL_PERIPH_I2C0, I2C0_MASTER_BASE);
				I2CIntRegister(I2C0_MASTER_BASE, I2C0IntHandler);
				break;
			case controller_1:
				_controllers[controller] = new I2CController(controller, SYSCTL_PERIPH_I2C1, I2C1_MASTER_BASE);
				I2CIntRegister(I2C1_MASTER_BASE, I2C1IntHandler);
				break;
			case controller_count:
				while(1); break;
		}
	}

	return _controllers[controller];
}

void I2CController::setup(GPIOPin sda, GPIOPin scl, speed_t speed, bool doEnableInterrupts)
{
	RecursiveMutexGuard guard(&_lock);
	_sda = sda;
	_scl = scl;

	MAP_SysCtlPeripheralEnable(_periph);
	SysCtlPeripheralReset(_periph);

	_sda.enablePeripheral();
	if (_base == I2C0_MASTER_BASE) {
		_sda.mapAsI2C0SDA();
	}
	else if (_base == I2C1_MASTER_BASE) {
		_sda.mapAsI2C1SDA();
	}
	else {
		while(1) { /* we should never get here! */ }
	}
	_sda.configure(GPIOPin::I2C);

	_scl.enablePeripheral();
	if (_base == I2C0_MASTER_BASE) {
		_scl.mapAsI2C0SCL();
	}
	else if (_base == I2C1_MASTER_BASE) {
		_scl.mapAsI2C1SCL();
	}
	else {
		while(1) { /* we should never get here! */ }
	}
	_scl.configure(GPIOPin::I2CSCL);

	I2CMasterInitExpClk(_base, MAP_SysCtlClockGet(), (speed==speed_400kBit) ? 1 : 0);
	I2CMasterEnable(_base);

	if (doEnableInterrupts) enableInterrupts(true, true);

    // Do a dummy receive to make sure you don't get junk on the first receive.
    I2CMasterControl(_base, I2C_MASTER_CMD_SINGLE_RECEIVE);
    waitFinish(1);
}

unsigned long I2CController::waitFinish(uint32_t timeout_ms)
{
	unsigned long ret;

	if (_interruptsEnabled) {
		if (_interruptSemaphore.take(timeout_ms)) {
			return I2CMasterErr(_base);
		} else {
			return 0xFFFFFFFF; // timeout;
		}
	} else {
		uint32_t timeout_time = Task::getTime() + timeout_ms;
		while (I2CMasterBusy(_base)) {
			ret = I2CMasterErr(_base);
			if (ret != I2C_MASTER_ERR_NONE) {
				return ret;
			}
			if ( (timeout_ms!=0xFFFFFFFF) && (Task::getTime() > timeout_time) ) {
				return 0xFFFFFFFF; // timeout;
			}
			Task::yield();
		}
		return I2CMasterErr(_base);
	}
}

unsigned long I2CController::read(uint8_t addr, uint8_t *buf, int count, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	unsigned long ret;

	if (count<1) return 0;
	ret = read8(addr, buf, sendStartCondition, (sendStopCondition && count==1));
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}

	for (uint8_t i=1; i<count; i++) {
		I2CMasterControl(_base, ((i==(count-1)) && sendStopCondition) ? I2C_MASTER_CMD_BURST_RECEIVE_FINISH : I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		ret = I2CMasterErr(_base);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = waitFinish();
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		buf[i] = I2CMasterDataGet(_base);
		ret = I2CMasterErr(_base);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}

	}

	return 0;
}

unsigned long I2CController::read8(uint8_t addr, uint8_t *data, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	unsigned long ret;

	I2CMasterSlaveAddrSet(_base, addr, 1);
	if (sendStartCondition) {
		I2CMasterControl(_base, sendStopCondition ? I2C_MASTER_CMD_SINGLE_SEND : I2C_MASTER_CMD_BURST_RECEIVE_START);
		ret = I2CMasterErr(_base);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	} else {
		I2CMasterControl(_base, sendStopCondition ? I2C_MASTER_CMD_BURST_RECEIVE_FINISH : I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		ret = I2CMasterErr(_base);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	}
	ret = waitFinish();
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}

	*data = I2CMasterDataGet(_base);
	ret = I2CMasterErr(_base);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	return 0;
}

unsigned long I2CController::write(uint8_t addr, uint8_t *buf, int count, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	unsigned long ret;

	if (count<1) return 0;
	write8(addr, buf[0], sendStartCondition, (count==1) && sendStopCondition);
	ret = I2CMasterErr(_base);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	for(int i=1; i<count; i++) {
		I2CMasterDataPut(_base, buf[i]);
		I2CMasterControl(_base, (sendStopCondition && (i == count-1)) ? I2C_MASTER_CMD_BURST_SEND_FINISH : I2C_MASTER_CMD_BURST_SEND_CONT);
		ret = I2CMasterErr(_base);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = waitFinish();
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	}
  return 0;
}

unsigned long I2CController::write8(uint8_t addr, uint8_t data, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	unsigned long ret;

	I2CMasterSlaveAddrSet(_base, addr, 0);
	I2CMasterDataPut(_base, data);
	if (sendStartCondition) {
		I2CMasterControl(_base, sendStopCondition ? I2C_MASTER_CMD_SINGLE_SEND : I2C_MASTER_CMD_BURST_SEND_START);
		ret = I2CMasterErr(_base);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}

	} else {
		I2CMasterControl(_base, sendStopCondition ? I2C_MASTER_CMD_BURST_SEND_FINISH : I2C_MASTER_CMD_BURST_SEND_CONT);
		ret = I2CMasterErr(_base);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}

	}
	ret = waitFinish();
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}

	return 0;
}


unsigned long I2CController::writeRead(uint8_t addr, uint8_t *writeBuf, uint8_t writeCount, uint8_t *readBuf, uint8_t readCount, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	unsigned long ret;

	ret = write(addr, writeBuf, writeCount, sendStartCondition, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read(addr, readBuf, readCount, true, sendStopCondition);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	return 0;
}

unsigned long I2CController::write8read(uint8_t addr, uint8_t writeData, uint8_t *readBuf, int readCount, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	unsigned long ret;

	ret = write8(addr, writeData, sendStartCondition, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read(addr, readBuf, readCount, true, sendStopCondition);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	return 0;
}

unsigned long I2CController::write8read8(uint8_t addr, uint8_t data_w, uint8_t *data_r, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	unsigned long ret;

	ret = write8(addr, data_w, sendStartCondition, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, data_r, true, sendStopCondition);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	return 0;
}

unsigned long I2CController::read16(uint8_t addr, uint16_t *data, byteorder_t byteorder, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	uint8_t b1, b2;
	unsigned long ret;

	ret = read8(addr, &b1, sendStartCondition, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, &b2, false, sendStopCondition);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}

	if (byteorder==byteorder_big_endian) {
		*data = (b1<<8) | b2;
	} else {
		*data =  (b2<<8) | b1;
	}
	return 0;
}

unsigned long I2CController::read32(uint8_t addr, uint32_t *data, byteorder_t byteorder, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	uint8_t b1, b2, b3, b4;
	unsigned long ret;

	ret = read8(addr, &b1, sendStartCondition, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, &b2, false, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, &b3, false, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, &b4, false, sendStopCondition);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}

	if (byteorder==byteorder_big_endian) {
		*data = (b1<<24) | (b2<<16) | (b3<<8) | b4;
	} else {
		*data = (b4<<24) | (b3<<16) | (b2<<8) | b1;
	}
	return 0;
}

unsigned long I2CController::write16(uint8_t addr, uint16_t data, byteorder_t byteorder, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	unsigned long ret;

	if (byteorder==byteorder_big_endian) {
		ret = write8(addr, data>>8,   sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data&0xFF, false, sendStopCondition);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	} else {
		ret = write8(addr, data&0xFF, sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data>>8,   false, sendStopCondition);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	}
	return 0;
}

unsigned long I2CController::write32(uint8_t addr, uint32_t data, byteorder_t byteorder, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);
	unsigned long ret;

	if (byteorder==byteorder_big_endian) {
		ret = write8(addr, data>>24, sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data>>16, false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data>>8,  false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data,     false, sendStopCondition);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	} else {
		ret = write8(addr, data,     sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data>>8,  false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data>>16, false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data>>24, false, sendStopCondition);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	}
	return 0;
}

unsigned long I2CController::write8read16(uint8_t addr, uint8_t data_w, uint16_t *data_r, byteorder_t byteorder, bool sendStartCondition, bool sendStopCondition)
{
	unsigned long ret;
	uint8_t b1, b2;

	RecursiveMutexGuard guard(&_lock);

	ret = write8(addr, data_w, sendStartCondition, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, &b1, true, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, &b2, false, sendStopCondition);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}

	if (byteorder==byteorder_big_endian) {
		*data_r = (b1<<8) | b2;
	} else {
		*data_r = (b2<<8) | b1;
	}

	return 0;
}

unsigned long I2CController::write8read32(uint8_t addr, uint8_t data_w, uint32_t *data_r, byteorder_t byteorder, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);

	unsigned long ret;
	uint8_t b1, b2, b3, b4;

	ret = write8(addr, data_w, sendStartCondition, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, &b1, true, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, &b2, false, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, &b3, false, false);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	ret = read8(addr, &b4, false, sendStopCondition);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}

	if (byteorder==byteorder_big_endian) {
		*data_r = (b1<<24) | (b2<<16) | (b3<<8) | b4;
	} else {
		*data_r = (b4<<24) | (b3<<16) | (b2<<8) | b1;
	}

	return 0;
}

unsigned long I2CController::write16read(uint8_t addr, uint16_t writeData, uint8_t *readBuf, int readCount, byteorder_t byteorder, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);

	unsigned long ret;

	if (byteorder==byteorder_big_endian) {
		ret = write8(addr, writeData>>8,   sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, writeData&0xFF, false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	} else {
		ret = write8(addr, writeData&0xFF, sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, writeData>>8,   false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	}
	ret = read(addr, readBuf, readCount, true, sendStopCondition);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}

	return 0;
}

unsigned long I2CController::write16read8(uint16_t addr, uint16_t data_w, uint8_t *data_r, byteorder_t byteorder, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);

	unsigned long ret;

	if (byteorder==byteorder_big_endian) {
		ret = write8(addr, data_w>>8,   sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data_w&0xFF, false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	} else {
		ret = write8(addr, data_w&0xFF, sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data_w>>8,   false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
	}
	ret = read8(addr, data_r, true, sendStopCondition);
	if (ret != I2C_MASTER_ERR_NONE) {
		return ret;
	}
	return 0;
}

unsigned long I2CController::write16read16(uint16_t addr, uint16_t data_w, uint16_t *data_r, byteorder_t byteorder, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);

	unsigned long ret;
	uint8_t b1, b2;

	if (byteorder==byteorder_big_endian) {
		ret = write8(addr, data_w>>8,   sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data_w&0xFF, false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b1, true, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b2, false, sendStopCondition);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		*data_r = (b1<<8) | b2;
	} else {
		ret = write8(addr, data_w&0xFF, sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data_w>>8,   false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b1, true, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b2, false, sendStopCondition);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		*data_r = (b2<<8) | b1;
	}
	return 0;
}

void I2CController::enableInterrupts(bool enableTimeoutInterrupt, bool enableDataInterrupt)
{
	uint32_t INT;
	switch (_controller) {
		case controller_0:
			INT = INT_I2C0;
			break;
		case controller_1:
			INT = INT_I2C1;
			break;
		default:
			while(1);
	}
	uint32_t flags = 0;
	if (enableTimeoutInterrupt) flags |= I2C_MASTER_INT_TIMEOUT;
	if (enableDataInterrupt) flags |= I2C_MASTER_INT_DATA;

	_interruptSemaphore.take(0); // reset pending interrupt mutex
	MAP_IntPrioritySet(INT, configMAX_SYSCALL_INTERRUPT_PRIORITY);
	I2CMasterIntEnableEx(_base, flags);
	IntEnable(INT);
	_interruptsEnabled = true;
}

unsigned long I2CController::write16read32(uint16_t addr, uint16_t data_w, uint32_t *data_r, byteorder_t byteorder, bool sendStartCondition, bool sendStopCondition)
{
	RecursiveMutexGuard guard(&_lock);

	unsigned long ret;
	uint8_t b1, b2, b3, b4;

	if (byteorder==byteorder_big_endian) {
		ret = write8(addr, data_w>>8,   sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data_w&0xFF, false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b1, true, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b2, false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b3, false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b4, false, sendStopCondition);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		*data_r = (b1<<24) | (b2<<16) | (b3<<8) | b4;
	} else {
		ret = write8(addr, data_w&0xFF, sendStartCondition, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = write8(addr, data_w>>8,   false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b1, true, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b2, false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b3, false, false);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		ret = read8(addr, &b4, false, sendStopCondition);
		if (ret != I2C_MASTER_ERR_NONE) {
			return ret;
		}
		*data_r = (b4<<24) | (b3<<16) | (b2<<8) | b1;
	}
	return 0;

}


