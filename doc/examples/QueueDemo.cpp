#include <openstella/OS/Task.h>
#include <openstella/OS/Queue.h>

class QueueDemoPushTask: public Task {
public:
	Queue<int> queue;
	virtual void execute()
	{
		while(1) {
			queue.sendToBack(i++);
			delay_ms(1000);
		}
	}
};

class QueueDemoMainTask : public Task {
public:
	virtual void execute(void) {
		QueueDemoPushTask *pushTask = new QueueDemoPushTask();
		pushTask->run();
		while (1) {
			int x;
			pushTask->queue.receive(&x); // call blocks until a new element is in the queue
		}
	}
};
