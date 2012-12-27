

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <math.h>

#include <openstella/OS/Task.h>
#include "HD44780_lcd.h"


HD44780::HD44780()
{
	_delay_250ns = ceilf( (SysCtlClockGet() / 3) / 4000000) + 1;
}



void HD44780::configure(GPIOPin pinEnable, GPIOPin pinRS, GPIOPin pinRW, GPIOPin data0, GPIOPin data1, GPIOPin data2, GPIOPin data3)
{
  /* After return from this function,
   * display we be set to 4-bit data mode
   */

  //lcd_set_contrast(0xef);

  // wait until LCD gets ready to accept instructions.
  Task::delay_ms(50);
  // set CONTROL Pins as outputs:
  // ENABLE
  // COMMAND/DATA
  // READ/WRITE
  pinEnable.enablePeripheral();
  pinEnable.configureAsOutput();
  pinEnable.setLow();
  pinRS.enablePeripheral();
  pinRS.configureAsOutput();
  pinRS.setLow();
  pinRW.enablePeripheral();
  pinRW.configureAsOutput();
  pinRW.setLow();

  data0.enablePeripheral();
  data0.configureAsOutput();
  data0.setLow();

  data1.enablePeripheral();
  data1.configureAsOutput();
  data1.setLow();

  data2.enablePeripheral();
  data2.configureAsOutput();
  data2.setLow();

  data3.enablePeripheral();
  data3.configureAsOutput();
  data3.setLow();


  _pinEnable = pinEnable;
  _pinRS = pinRS;
  _pinRW = pinRW;
  _data0 = data0;
  _data1 = data1;
  _data2 = data2;
  _data3 = data3;


  setCmdMode();
  setWriteMode();

  // set LCD to 8-bit mode
  sendNibble(0x03);

  Task::delay_ms(5);
  sendNibble(0x03);

  Task::delay_ms(5);
  sendNibble(0x03);

  // LCD definitely is in 8-bit mode now

  // set module to 4-bit mode now
  // Remember! As we're in 8-bit mode now,
  // send_nibble is the right thing, not send_byte!
  Task::delay_ms(5);
  sendNibble(0x02);
  Task::delay_ms(5);

  // set to 5 x 8 dots per character,
  // 16 characters per line, 2 lines
  sendByte(0x28, 0);

  // enable display, no cursor, no blinking
  setCursor(0);

  // Set display to
  // + Move cursor (and do not shift display)
  // + Move right
  sendByte(0x04, 0);

  // clear dislay
  sendByte(0x01, 0);

  // associate the Display with a FILE* stream
  // So we can use stdout on it
  // lcd_io = fdevopen(_lcd_put, _lcd_get);

  return;
}



int HD44780::putChar(char c)
{
	sendByte(c, 1);
	return 0;
}



int HD44780::getChar(void)
{
  waitWhileBusy();
  return readByte(1);
}



void HD44780::clear(void)
{
  sendByte(0x01, 0);

}



void HD44780::locate(uint8_t row, uint8_t column)
{
  if (row)
    column += 0x40;
  sendByte(0x80 + column, 0);
}



void HD44780::setCursor(uint8_t blink)
{
  if (blink) {
    // enable display with cursor + blinking
    sendByte(0x0f, 0);
  }
  else  {
    // enable display, no cursor, no blinking
  	sendByte(0x0c, 0);
  return;
  }

}

void HD44780::sendNibble(uint8_t nibble)
{
  setWriteMode();
  //PORTB = (PORTB & 0x0f) | (nibble<<4);
  _data0.set(nibble & 0x01);
  _data1.set(nibble & 0x02);
  _data2.set(nibble & 0x04);
  _data3.set(nibble & 0x08);
  setEnable();
  Task::delay_ticks(1);
  clearEnable();

}


/**
 * Send one byte to the LCD
 * \param byte The byte to send to the LCD
 * \param RS specifies whether byte sent is data (RS==1) or command (RS==0)
 */
void HD44780::sendByte(uint8_t byte, bool RS)
{
	waitWhileBusy();
  if (RS)
    setDataMode();
  else
    setCmdMode();

  sendNibble(byte >> 4);
  SysCtlDelay(3); // delay minimal amount of time
  sendNibble(0x0f & byte);

}


void HD44780::waitWhileBusy(void)
{
	uint8_t byte;
	  do {
	    byte = readByte(0);
	  } while (byte & 0x80);
	  return;
}


void HD44780::write(char *data, int len)
{
	int i = 0;

	while (len--) {
		putChar(data[i++]);
	}
}

uint8_t HD44780::readByte(uint8_t RS)
{
  uint8_t byte;

  if (RS)
    setDataMode();
  else
    setCmdMode();

  setReadMode();

  setEnable();
  SysCtlDelay(_delay_250ns);
  byte  = (_data0.isHigh()?0x10:0);
  byte |= (_data1.isHigh()?0x20:0);
  byte |= (_data2.isHigh()?0x40:0);
  byte |= (_data3.isHigh()?0x80:0);
  clearEnable();

  SysCtlDelay(_delay_250ns * 2);

  setEnable();
  SysCtlDelay(_delay_250ns);
  byte |= (_data0.isHigh()?0x01:0);
  byte |= (_data1.isHigh()?0x02:0);
  byte |= (_data2.isHigh()?0x04:0);
  byte |= (_data3.isHigh()?0x08:0);
  clearEnable();

  return byte;

}



