#if !defined  __LCD_H
#define __LCD_H

#include <stdint.h>
#include <openstella/GPIO.h>


class HD44780
{

public:
	HD44780();

	void configure(GPIOPin pinEnable, GPIOPin pinRS, GPIOPin pinRW, GPIOPin data0, GPIOPin data1, GPIOPin data2, GPIOPin data3);
	int putChar(char c);
	int getChar(void);
	void clear(void);
	void locate(uint8_t row, uint8_t column);
	void setCursor(uint8_t blink);
	void write(char *data, int len);

private:

	GPIOPin _pinEnable;
	GPIOPin _pinRS;
	GPIOPin _pinRW;

	GPIOPin _data0, _data1, _data2, _data3;


	void setDataMode() { _pinRS.setHigh(); }
	void setCmdMode()  { _pinRS.setLow(); }
	void setEnable() { _pinEnable.setHigh(); }
	void clearEnable()  { _pinEnable.setLow(); }
	void setReadMode() {  _data0.configureAsInput(); _data1.configureAsInput(); _data2.configureAsInput(); _data3.configureAsInput();  _pinRW.setHigh(); }
	void setWriteMode() { _pinRW.setLow(); _data0.configureAsOutput(); _data1.configureAsOutput(); _data2.configureAsOutput(); _data3.configureAsOutput(); }

	void sendNibble(uint8_t nibble);
	void sendByte(uint8_t byte, bool RS);

	void waitWhileBusy(void);
	uint8_t readByte(uint8_t RS);

	uint32_t _delay_250ns;

};



#endif
