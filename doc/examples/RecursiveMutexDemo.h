#include <openstella/OS/Task.h>
#include <openstella/OS/RecursiveMutex.h>
#include <openstella/CAN.h>

class MutexDemo : public Task {

	private:
		RecursiveMutex _lock;
		int i;

	public:

		void commonFunction(int a)
		{
			/*
			 * note: using a non-recursive mutex, this would deadlock because the _lock
			 * is already taken by the calling method.
			 * It's still thread-safe because only one thread can have the _lock at a time.
			 */
			RecursiveMutexGuard(_lock);
			i += a;
		}

		void secondTask(void)
		{
			while(1) {
				{
					RecursiveMutexGuard guard(_lock);
					commonFunction(-1);
				}
				delay_ms(1);
			}
		}

		virtual void execute(void) {
			TaskFunctor<MainTask> functor(this, &MainTask::secondTask);
			Task::runFunctor(&functor);

			while(1) {
				_lock.take();
				commonFunction(+1);
				_lock.give();
				delay_ms(1);
			}
		}
};
