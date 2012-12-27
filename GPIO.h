/*
 * GPIO.h
 *
 * Copyright 2011, 2012 Germaneers GmbH
 * Copyright 2011, 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
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

/*
 * TODO: interrupt configuration / handling
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

class GPIOPin;
class GPIOPort;

class GPIO {

	public:
		static GPIOPort A;
		static GPIOPort B;
		static GPIOPort C;
		static GPIOPort D;
		static GPIOPort E;
		static GPIOPort F;
		static GPIOPort G;
		static GPIOPort H;
		static GPIOPort J;

};

/**
 * @class GPIOPin
 * @brief Abstraction of a single GPIO Pin
 *
 * A GPIOPin cannot be directly constructed. \n
 * Get a copy of a certain GPIOPin from the corresponding GPIOPort,
 * e.g. GPIO::A.get(0) or GPIO::A[0]
 *
 * \include GPIODemo.cpp
 */
class GPIOPin {
	friend class GPIOPort;

	public:

		typedef enum {
			input=0,
			output=1,
			hardware=2
		} direction_t;

		typedef enum {
			ADC,
			CAN,
			Comparator,
			EPI,
			EthernetLED,
			EthernetMII,
			Fan,
			GPIOInput,
			GPIOOutput,
			GPIOOutputOD,
			I2C,
			I2CSCL,
			I2S,
			LPC,
			PECIRx,
			PECITx,
			PWM,
			QEI,
			SSI,
			Timer,
			UART,
			USBAnalog,
			USBDigital
		} mode_t;

		typedef enum {
			strength_2ma = 0x01,
			strength_4ma = 0x02,
			strength_8ma = 0x04,
			strength_8ma_slewrate_control = 0x0C
		} strength_t;

		typedef enum {
			push_pull			 = 0x08,
			pull_up				 = 0x0A,
			pull_down			 = 0x0C,
			open_drain			 = 0x09,
			open_drain_pull_up	 = 0x0B,
			open_drain_pull_down = 0x0D,
			analog				 = 0x00
		} padType_t;

		typedef enum {
			falling_edge = 0x00000000,
			rising_edge  = 0x00000004,
			both_edges   = 0x00000001,
			low_level    = 0x00000002,
			high_level   = 0x00000007,
			discrete_int = 0x00010000
		} interruptType_t;

	private:
		uint8_t _port_pin;
		GPIOPin(uint8_t port, uint8_t pin);

	public:
		static GPIOPin invalid;
		GPIOPin();
		bool isValid() { return _port_pin!=0xFF; }

		/// enable GPIO port peripheral
		/**
		 * enables the pin's port peripheral.
		 * doing anything with GPIO hardware prior to enabling the port results in an hardfault!
		 */
		void enablePeripheral();

		/// configure pin for a mode of operation
		/** executes setDirection() and configurePad() with values matching the desired mode of operation
		 *  @param cfg the new operational mode for the pin\n
		 *  Valid operation modes are:
		 *  \li ADC
		 *  \li CAN
		 *  \li Comparator
		 *  \li EPI
		 *  \li EthernetLED
		 *  \li EthernetMII
		 *  \li Fan
		 *  \li GPIOInput
		 *  \li GPIOOutput
		 *  \li GPIOOutputOD - GPIO Output in open drain mode
		 *  \li I2C    - I²C SDA
		 *  \li I2CSCL - I²C SCL
		 *  \li I2S
		 *  \li LPC
		 *  \li PECIRx
		 *  \li PECITx
		 *  \li PWM
		 *  \li QEI
		 *  \li SSI
		 *  \li Timer
		 *  \li UART
		 *  \li USBAnalog
		 *  \li USBDigital
		 */
		void configure(mode_t cfg);

		/// configure output mode (input, output, pullup/down..) and driving strength
		/** usually, this is done via configure()
		 *  @param padType the pad type (push_pull, pull_up, pull_down..)
		 *  @param strength driving strength (2, 4, or 8mA..)
		 */
		void configurePad(padType_t padType, strength_t strength);

		/// get the set driving strength
		/** @return driving strength */
		strength_t getPadStrength();

		/// get the current pad type
		/** @return current pad type */
		padType_t  getPadType();

		/// set the pin direction mode
		/** usually, this is done via configure()
		 *  @param dir the new direction mode
		 */
		void setDirection(direction_t dir);

		/// get the pin direction mode
		/** @return the current direction mode */
		direction_t getDirection();

		/// quickly configure pin as a GPIO input
		/** @param padType the pad type (e.g. pull_up, pull_down, open_drain or push_pull) */
		void configureAsInput(padType_t padType = push_pull);

		/// quickly configure pin as a GPIO output
		/** @param strength the driving strength */
		void configureAsOutput(strength_t strength = strength_2ma);

		/// read GPIO input pin
		/** @return true if the pin is high, false if it is low */
		bool isHigh();

		/// read GPIO input pin
		/** @return true if the pin is low, false if it is high */
		bool isLow();

		/// set the pin in high state
		/** sets the pad state when in output mode */
		void setHigh();

		/// set the pin in low state
		/** sets the pad state when in output mode */
		void setLow();

		/// set the pin to a boolean state
		/** @param b set pin to high if b is true, otherwise low */
		void set(bool b);

		void enableDMATrigger();
		void disableDMATrigger();
		void enableADCTrigger();
		void disableADCTrigger();

		void enableInterrupt();
		void disableInterrupt();
		void clearInterrupt();
		void setInterruptType(interruptType_t intType);


		/// get the corresponding GPIO port
		/** @result the gpio port the pin belongs to */
		GPIOPort *getPort();
		uint8_t  getPins();

		bool operator==(const GPIOPin &other) const { return other._port_pin == _port_pin; }
		bool operator!=(const GPIOPin &other) const { return other._port_pin != _port_pin; }

	public:
		void mapAsC0O();
		void mapAsC1O();
		void mapAsC2O();
		void mapAsCAN0RX();
		void mapAsCAN0TX();
		void mapAsCAN1RX();
		void mapAsCAN1TX();
		void mapAsCAN2RX();
		void mapAsCAN2TX();
		void mapAsCCP0();
		void mapAsCCP1();
		void mapAsCCP2();
		void mapAsCCP3();
		void mapAsCCP4();
		void mapAsCCP5();
		void mapAsCCP6();
		void mapAsCCP7();
		void mapAsEPI0S0();
		void mapAsEPI0S1();
		void mapAsEPI0S10();
		void mapAsEPI0S11();
		void mapAsEPI0S12();
		void mapAsEPI0S13();
		void mapAsEPI0S14();
		void mapAsEPI0S15();
		void mapAsEPI0S16();
		void mapAsEPI0S17();
		void mapAsEPI0S18();
		void mapAsEPI0S19();
		void mapAsEPI0S2();
		void mapAsEPI0S20();
		void mapAsEPI0S21();
		void mapAsEPI0S22();
		void mapAsEPI0S23();
		void mapAsEPI0S24();
		void mapAsEPI0S25();
		void mapAsEPI0S26();
		void mapAsEPI0S27();
		void mapAsEPI0S28();
		void mapAsEPI0S29();
		void mapAsEPI0S3();
		void mapAsEPI0S30();
		void mapAsEPI0S31();
		void mapAsEPI0S4();
		void mapAsEPI0S5();
		void mapAsEPI0S6();
		void mapAsEPI0S7();
		void mapAsEPI0S8();
		void mapAsEPI0S9();
		void mapAsI2C0SCL();
		void mapAsI2C0SDA();
		void mapAsI2C1SCL();
		void mapAsI2C1SDA();
		void mapAsI2C2SCL();
		void mapAsI2C2SDA();
		void mapAsI2C3SCL();
		void mapAsI2C3SDA();
		void mapAsI2S0RXMCLK();
		void mapAsI2S0RXSCK();
		void mapAsI2S0RXSD();
		void mapAsI2S0RXWS();
		void mapAsI2S0TXMCLK();
		void mapAsI2S0TXSCK();
		void mapAsI2S0TXSD();
		void mapAsI2S0TXWS();
		void mapAsLED0();
		void mapAsLED1();
		void mapAsNMI();
		void mapAsSSI0CLK();
		void mapAsSSI0FSS();
		void mapAsSSI0RX();
		void mapAsSSI0TX();
		void mapAsSSI1CLK();
		void mapAsSSI1FSS();
		void mapAsSSI1RX();
		void mapAsSSI1TX();
		void mapAsSSI2CLK();
		void mapAsSSI2FSS();
		void mapAsSSI2RX();
		void mapAsSSI2TX();
		void mapAsSSI3CLK();
		void mapAsSSI3FSS();
		void mapAsSSI3RX();
		void mapAsSSI3TX();
		void mapAsSWCLK();
		void mapAsSWDIO();
		void mapAsSWO();
		void mapAsTCK();
		void mapAsTDI();
		void mapAsTDO();
		void mapAsTMS();
		void mapAsU0RX();
		void mapAsU0TX();
		void mapAsU1CTS();
		void mapAsU1DCD();
		void mapAsU1DSR();
		void mapAsU1DTR();
		void mapAsU1RI();
		void mapAsU1RTS();
		void mapAsU1RX();
		void mapAsU1TX();
		void mapAsU2RX();
		void mapAsU2TX();
		void mapAsU3RX();
		void mapAsU3TX();
		void mapAsU4RX();
		void mapAsU4TX();
		void mapAsU5RX();
		void mapAsU5TX();
		void mapAsU6RX();
		void mapAsU6TX();
		void mapAsU7RX();
		void mapAsU7TX();
		void mapAsUSB0EPEN();
		void mapAsUSB0PFLT();
		void mapAsPWM0();
		void mapAsPWM1();
		void mapAsPWM2();
		void mapAsPWM3();
		void mapAsPWM4();
		void mapAsPWM5();
		void mapAsPWM6();
		void mapAsPWM7();
		void mapAsPHA0();
		void mapAsPHA1();
		void mapAsPHB0();
		void mapAsPHB1();
		void mapAsFAULT0();
		void mapAsFAULT1();
		void mapAsFAULT2();
		void mapAsFAULT3();
		void mapAsIDX0();
		void mapAsIDX1();

		void mapAsTRCLK();
		void mapAsTRD0();
		void mapAsTRD1();
		void mapAsTRD2();

		void mapAsWT0CCP0();
		void mapAsWT0CCP1();
		void mapAsWT1CCP0();
		void mapAsWT1CCP1();
		void mapAsWT2CCP0();
		void mapAsWT2CCP1();
		void mapAsWT3CCP0();
		void mapAsWT3CCP1();
		void mapAsWT4CCP0();
		void mapAsWT4CCP1();
		void mapAsWT5CCP0();
		void mapAsWT5CCP1();

		void mapAsT0CCP0();
		void mapAsT0CCP1();
		void mapAsT1CCP0();
		void mapAsT1CCP1();
		void mapAsT2CCP0();
		void mapAsT2CCP1();
		void mapAsT3CCP0();
		void mapAsT3CCP1();
		void mapAsT4CCP0();
		void mapAsT4CCP1();
		void mapAsT5CCP0();
		void mapAsT5CCP1();
};

class GPIOPort {
	friend class GPIO;

	private:
		uint8_t  _portNumber;
		uint32_t _periph;
		uint32_t _base;
		GPIOPort(uint8_t portNumber, uint32_t periph, uint32_t base);
		inline uint32_t getInterruptNumber();

	public:
		void enablePeripheral(void);
		void disablePeripheral(void);

		void configurePins(uint8_t pins, GPIOPin::mode_t cfg);
		void configurePads(uint8_t pins, GPIOPin::strength_t strength, GPIOPin::padType_t padType);
		GPIOPin::strength_t getPadStrength(uint8_t pin);
		GPIOPin::padType_t  getPadType(uint8_t pin);

		void setPinsDirection(uint8_t pins, GPIOPin::direction_t dir);
		GPIOPin::direction_t getPinDirection(uint8_t pin);

		uint16_t readPins(uint8_t pins);
		void writePins(uint8_t pins, uint8_t values);

		void setPins(uint8_t pins);
		void clearPins(uint8_t pins);

		void enableDMATriggerPins(uint8_t pins);
		void disableDMATriggerPins(uint8_t pins);

		void enableADCTriggerPins(uint8_t pins);
		void disableADCTriggerPins(uint8_t pins);

		void registerInterruptHandler(void (*intHandler)(void));
		void unregisterInterruptHandler();
		void disableInterrupt(uint8_t pins);
		void setInterruptType(uint8_t pins, GPIOPin::interruptType_t intType);
		void enableInterrupt();
		void enableInterruptPins(uint8_t pins);
		void clearInterruptPins(uint8_t pins);


		GPIOPin getPin(uint8_t pin)  { return GPIOPin( _portNumber, pin ); }
		GPIOPin operator[] (uint8_t pin) { return GPIOPin( _portNumber, pin ); }

		uint32_t getBase() { return _base; }
};

#endif /* GPIO_H_ */
