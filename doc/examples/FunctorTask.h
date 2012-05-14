#include <openstella/OS/Task.h>
#include <openstella/CAN.h>

class FunctorTask : public Task {
public:
	void secondTask(void)
	{
		GPIOPin usr_led = GPIO::D[0];
		usr_led.enablePeripheral();
		usr_led.configureAsOutput();
		while(1) {
			usr_led.setHigh();
			Task::delay_ms(100);
			usr_led.setLow();
			Task::delay_ms(100);
		}
	}

	virtual void execute(void) {
		TaskFunctor<MainTask> functor(this, &MainTask::secondTask);
		Task::runFunctor(&functor);
		suspend();		
	}
};
