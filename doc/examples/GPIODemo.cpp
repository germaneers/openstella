 #include <openstella/OS/Task.h>
 #include <openstella/GPIO.h>

class GPIODemoTask : public Task {

	virtual void execute() {
		GPIOPin ledPin = GPIO::A[0];
		ledPin.enablePeripheral();
		ledPin.configureAsOutput();

		GPIOPin keyPin = GPIO::A[1];
		keyPin.enablePeripheral();
		keyPin.configureAsInput();

		while(1) {
			ledPin.set(keyPin.isLow());
			delay_ms(10);
		}
	}
};
