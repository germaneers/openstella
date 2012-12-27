/*
 * ADCModule.h
 *
 *  Created on: 18.06.2012
 *      Author: hd
 */

#ifndef ADCMODULE_H_
#define ADCMODULE_H_

#include <stdint.h>
#include "ADCTypes.h"

class ADCSequencer;

class ADCModule {
private:
	ADCModule(ADC::module_num_t module);
	static ADCModule *_modules[2];
	ADC::module_num_t _module;
	uint32_t _periph;
	uint32_t _base;
	ADCSequencer *_sequencers[4];
public:
	uint32_t getBase();
	static ADCModule *get(ADC::module_num_t module);
	void enablePeripheral();
	ADCSequencer *getSequencer(ADC::sequencer_num_t sequence_num);
};

#endif /* ADCMODULE_H_ */
