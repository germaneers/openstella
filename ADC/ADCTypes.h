/*
 * ADCTypes.h
 *
 *  Created on: 18.06.2012
 *      Author: hd
 */

#ifndef ADCTYPES_H_
#define ADCTYPES_H_

namespace ADC {

	typedef enum {
		module_0,
		module_1
	}  module_num_t;

	typedef enum {
		sequencer_0,
		sequencer_1,
		sequencer_2,
		sequencer_3
	} sequencer_num_t;

	typedef enum {
		trigger_processor,
		trigger_comp0,
		trigger_comp1,
		trigger_comp2,
		trigger_external,
		trigger_timer,
		trigger_pwm0,
		trigger_pwm1,
		trigger_pwm2,
		trigger_pwm3,
		trigger_always
	} trigger_t;

	typedef enum {
		priority_0,
		priority_1,
		priority_2,
		priority_3
	} priority_t;

	typedef enum {
		step_0,
		step_1,
		step_2,
		step_3,
		step_4,
		step_5,
		step_6,
		step_7
	} step_num_t;

	typedef enum {
		channel_0   = 0x000,
		channel_1   = 0x001,
		channel_2   = 0x002,
		channel_3   = 0x003,
		channel_4   = 0x004,
		channel_5   = 0x005,
		channel_6   = 0x006,
		channel_7   = 0x007,
		channel_8   = 0x008,
		channel_9   = 0x009,
		channel_10  = 0x00A,
		channel_11  = 0x00B,
		channel_12  = 0x00C,
		channel_13  = 0x00D,
		channel_14  = 0x00E,
		channel_15  = 0x00F,
		channel_16  = 0x100,
		channel_17  = 0x101,
		channel_18  = 0x102,
		channel_19  = 0x103,
		channel_20  = 0x104,
		channel_21  = 0x105,
		channel_22  = 0x106,
		channel_23  = 0x107,
		channel_temperature = 0x80
	} channel_num_t;
}


#endif /* ADCTYPES_H_ */
