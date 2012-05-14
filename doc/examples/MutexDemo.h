#include <openstella/OS/Task.h>
#include <openstella/OS/Mutex.h>
#include <openstella/CAN.h>

class MutexDemo : public Task {

	private:
		Mutex _lock;
		int i;

	public:

		void secondTask(void)
		{
			while(1) {
				{
					MutexGuard guard(_lock);
					i--;
				}
				delay_ms(1);
			}
		}

		virtual void execute(void) {
			TaskFunctor<MainTask> functor(this, &MainTask::secondTask);
			Task::runFunctor(&functor);

			while(1) {
				_lock.take();
				i++;
				_lock.give();
				delay_ms(1);
			}
		}
};
