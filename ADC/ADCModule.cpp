/*
 * ADCModule.cpp
 *
 *  Created on: 18.06.2012
 *      Author: hd
 */

#include "ADCModule.h"
#include <openstella/OS/Mutex.h>

#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/adc.h>

#include "ADCSequencer.h"

ADCModule *ADCModule::_modules[2] = { 0,0 };

ADCModule::ADCModule(ADC::module_num_t module)
: _module(module)
{
	switch (_module) {
		case ADC::module_0:
			_periph = SYSCTL_PERIPH_ADC0;
			_base = ADC0_BASE;
			break;
		case ADC::module_1:
			_periph = SYSCTL_PERIPH_ADC1;
			_base = ADC1_BASE;
			break;
		default:
			while(1);
	}

	_sequencers[0] = new ADCSequencer(this, ADC::sequencer_0);
	_sequencers[1] = new ADCSequencer(this, ADC::sequencer_1);
	_sequencers[2] = new ADCSequencer(this, ADC::sequencer_2);
	_sequencers[3] = new ADCSequencer(this, ADC::sequencer_3);
}

ADCModule* ADCModule::get(ADC::module_num_t module)
{
	static Mutex _mutex;
	MutexGuard guard(&_mutex);

	if (!_modules[module]) {
		_modules[module] = new ADCModule(module);
	}

	return _modules[module];
}

void ADCModule::enablePeripheral()
{
	SysCtlPeripheralEnable(_periph);
}

uint32_t ADCModule::getBase()
{
	return _base;
}

ADCSequencer *ADCModule::getSequencer(ADC::sequencer_num_t sequence_num)
{
	return _sequencers[sequence_num];
}





