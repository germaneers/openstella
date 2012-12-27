/*
 * Timer.h
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


#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include "GPIO.h"
#include "generics/VoidFunctor.h"

class TimerChannel;

class Timer
{
	friend class TimerChannel;
	friend void Timer0Handler(void);
	friend void Timer1Handler(void);
	friend void Timer2Handler(void);
	friend void Timer3Handler(void);

	public:

		typedef enum {
			timer_0,
			timer_1,
			timer_2,
			timer_3,
			timer_count
		} num_t;

		typedef enum {
			channel_A,
			channel_B
		} channel_t;

		typedef enum {
			timer_both = 0xFFFF,
			timer_A    = 0x00FF,
			timer_B	   = 0xFF00
		} half_t;

		typedef enum {
			one_32bit,
			two_16bit
		} split_t;

		typedef enum {
			timer_b_match   = 0x00000800,
			capture_b_event = 0x00000400,
			captute_b_match = 0x00000200,
			timer_b_timeout = 0x00000100,

			timer_a_match   = 0x00000010,
			capture_a_event = 0x00000004,
			captute_a_match = 0x00000002,
			timer_a_timeout = 0x00000001,

			rtc_match       = 0x00000010
		} interrupt_flag_t;

		typedef enum {
			one_shot    	 = 0x21,
			one_shot_up  	 = 0x31,
			periodic		 = 0x22,
			periodic_up 	 = 0x32,
			rtc				 = 0x01000000,
			event_counter    = 0x03,
			event_counter_up = 0x13,
			event_timer		 = 0x07,
			event_timer_up   = 0x17,
			pwm				 = 0x0A
		} type_t;

		typedef enum {
			edge_positive = 0x0000,
			edge_negative = 0x0404,
			edge_both	  = 0x0C0C
		} edge_t;

		typedef enum {
			active_high = 0,
			active_low  = 1
		} invertation_t;

		typedef enum {
			debug_mode_stall,
			debug_mode_continue
		} debug_mode_t;

	private:
		static Timer _timers[timer_count];
		Timer(num_t num);
		uint32_t getBase();
		uint32_t getPeriph();
		num_t _num;
		TimerChannel *_channel_A;
		TimerChannel *_channel_B;
		uint32_t _last_config;

		VoidFunctorBase *_interruptCallback;
		void handleInterrupt(void);
		uint32_t getInterruptNumber(channel_t channel);


	public:
		Timer(const Timer& t);
		static Timer *getTimer(num_t num);
		TimerChannel *getChannel(channel_t channel);
		TimerChannel *getChannelA();
		TimerChannel *getChannelB();

		void enablePeripheral();
		void disablePeripheral();
		void setType(type_t type);
		void setChannelType(channel_t channel, type_t type);

		void setInvertation(invertation_t invertation);
		void setTriggerOutput(bool enableTrigger);
		void setEventType(edge_t edge);
		void setDebugMode(debug_mode_t mode);

		void enable();
		void disable();

		void enableRTC();
		void disableRTC();

		void setLoadValue(uint32_t load);
		uint32_t getLoadValue();

		uint32_t getValue();

		void setMatchValue(uint32_t match);
		uint32_t getMatchValue();

		void registerInterruptHandler(void (*pfnHandler)(void), half_t channel);
		void enableInterrupt(uint32_t flags);
		void clearInterrupt(uint32_t flags);

		uint32_t getInterruptStatus(bool returnMaskedStatus=true);
		void setInterruptCallback(VoidFunctorBase *callback);

};


class TimerChannel {
	friend class Timer;
	private:
		Timer::num_t  _timer_num;
		Timer::channel_t _channel;
		TimerChannel(Timer::num_t timer_num, Timer::channel_t channel);
		uint32_t getBase();
		uint16_t getChannel();

	public:
		Timer *getTimer();
		void setType(Timer::type_t type);
		Timer::type_t getType();
		void setInvertation(Timer::invertation_t invertation);
		void setTriggerOutput(bool enableTrigger);
		void setEventType(Timer::edge_t edge);
		void setDebugMode(Timer::debug_mode_t mode);
		void setLoadValue(uint16_t load);
		uint16_t getLoadValue();
		uint16_t getValue();
		void setMatchValue(uint16_t match);
		uint16_t getMatchValue();
		void enableTimer();
		void disableTimer();

		void configurePWM(GPIOPin pin, uint16_t maxValue=100, uint16_t initialValue=0);
		void configureCounter(GPIOPin pin, Timer::edge_t edgeType=Timer::edge_positive, uint16_t loadValue=0xffff, uint16_t matchValue=0x0000);

		uint8_t getChannelNumber();

};


#endif /* TIMER_H_ */
