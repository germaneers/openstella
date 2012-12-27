/*
 * ADCSequence.cpp
 *
 *  Created on: 18.06.2012
 *      Author: hd
 */

#include "ADCSequencer.h"
#include "ADCModule.h"
#include <string.h>
#include <openstella/OS/Task.h>

#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_memmap.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/adc.h>

ADCSequencer::ADCSequencer(ADCModule *module, ADC::sequencer_num_t sequence_num)
: _module(module), _sequence_num(sequence_num)
{
	memset(_data, 0, sizeof(_data));
	_base = _module->getBase();
}

void ADCSequencer::configure(ADC::trigger_t trigger, ADC::priority_t priority)
{
	_module->enablePeripheral();
	ADCSequenceConfigure(_base, _sequence_num, trigger, priority);
}

void ADCSequencer::configureStep(ADC::step_num_t step, ADC::channel_num_t channel, bool lastInSequence, bool interruptOnComplete, bool differentialSampling)
{
	uint32_t config = channel;
	if (lastInSequence)       config |= ADC_CTL_END;
	if (interruptOnComplete)  config |= ADC_CTL_IE;
	if (differentialSampling) config |= ADC_CTL_D;
	ADCSequenceStepConfigure(_base, _sequence_num, step, config);
}

void ADCSequencer::enable()
{
	ADCSequenceEnable(_base, _sequence_num);
	ADCIntClear(_base, _sequence_num);
}

bool ADCSequencer::isDataReady()
{
	return ADCIntStatus(_base, _sequence_num, false) != 0;
}

void ADCSequencer::readData()
{
	ADCSequenceDataGet(_base, _sequence_num, _data);
}

void ADCSequencer::trigger()
{
	ADCProcessorTrigger(_base, _sequence_num);
}

void ADCSequencer::clearInterrupt()
{
	ADCIntClear(_base, _sequence_num);
}

void ADCSequencer::triggerAndWait()
{
	trigger();
	while (!isDataReady()) { Task::yield(); }
	readData();
	clearInterrupt();
}

uint16_t ADCSequencer::getValue(ADC::step_num_t step)
{
	return _data[step];
}



