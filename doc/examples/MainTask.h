 #include <openstella/OS/Task.h>
 #include <openstella/GPIO.h>

class MainTask : public Task {
	virtual void execute() {
		GPIOPin ledPin = GPIO::A[0];
		ledPin.enablePeripheral();
		ledPin.configureAsOutput();
		while(1) {
			ledPin.setHigh();
			delay_ms(200);
			ledPin.setLow();
			delay_ms(200);
		}
	}
}
