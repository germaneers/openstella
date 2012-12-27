/*
 * ADCSequence.h
 *
 *  Created on: 18.06.2012
 *      Author: hd
 */

#ifndef ADCSEQUENCE_H_
#define ADCSEQUENCE_H_

#include <stdint.h>

#include "ADCTypes.h"
class ADCModule;

class ADCSequencer {
private:
	ADCModule *_module;
	ADC::sequencer_num_t _sequence_num;
	uint32_t _data[8];
	uint32_t _base;
public:
	ADCSequencer(ADCModule *module, ADC::sequencer_num_t sequence_num);
	void configure(ADC::trigger_t trigger=ADC::trigger_processor, ADC::priority_t priority=ADC::priority_0);
	void configureStep(ADC::step_num_t step, ADC::channel_num_t channel, bool lastInSequence=false, bool interruptOnComplete=false, bool differentialSampling=false);
	void enable();
	bool isDataReady();
	void readData();
	void trigger();
	void clearInterrupt();
	void triggerAndWait();
	uint16_t getValue(ADC::step_num_t step);
};

#endif /* ADCSEQUENCE_H_ */
