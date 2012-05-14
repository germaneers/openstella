#include <openstella/OS/Task.h>
#include <openstella/GPIO.h>
#include <openstella/CAN.h>

class CAN_EchoExample : public Task, public CANObserver {

	virtual void execute() {

		CANController *can = CANController::get(CAN::channel_0);
		can->setup(CAN::bitrate_500kBit, GPIO::D[0], GPIO::D[1]);
		can->enable();

		CANMessage msg;
		listenCAN(CAN::channel_0);

		while(1) {
			getCANMessage(&msg);
			can->sendMessage(&msg);
		}

	}

};
