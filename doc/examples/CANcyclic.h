#include <openstella/OS/Task.h>
#include <openstella/GPIO.h>
#include <openstella/CAN.h>

class CAN_CyclicExample : public Task {

	virtual void execute() {

		CANController *can = CANController::get(CAN::channel_0);
		can->setup(CAN::bitrate_500kBit, GPIO::D[0], GPIO::D[1]);
		can->enable();

		CANCyclicMessage cmsg(0xc0ffee, 2, 100);
		cmsg.setData(0x23,0x00);
		can->registerCyclicMessage(&cmsg);

		uint8_t i=0;
		while(1) {
			cmsg->setDataByte(1, i++);
			delay_ms(1000);
		}

	}

};
