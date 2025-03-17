// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>
#include <crt_Mutex.h>

namespace crt
{

	// NOTE: it is forbidden to lock a new mutex with an id
	// that is lower than the highest id of currently locked mutexes.
	// By using MutexSections, that rule is safeguarded.

	class SyncedNumbersIncreaser : public Task
	{
	private:
		int32_t& sharedInt1;
		Mutex& mutexSharedInt1;

		int32_t& sharedInt2;
		Mutex& mutexSharedInt2;

		int32_t& sharedInt3;
		Mutex& mutexSharedInt3;

	public:
		static void StaticMain(void* pParam)
		{
			SyncedNumbersIncreaser* THIS = (SyncedNumbersIncreaser*)pParam;
			THIS->main();
		}

	public:
		SyncedNumbersIncreaser(const char* taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber,
			int32_t& sharedInt1, Mutex& mutexSharedInt1, int32_t& sharedInt2, Mutex& mutexSharedInt2, int32_t& sharedInt3, Mutex& mutexSharedInt3) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), sharedInt1(sharedInt1), mutexSharedInt1(mutexSharedInt1), 
			sharedInt2(sharedInt2), mutexSharedInt2(mutexSharedInt2), sharedInt3(sharedInt3), mutexSharedInt3(mutexSharedInt3)
		{
			start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.
			vTaskDelay(1000); // wait for other threads to have started up as well.
			while (true)
			{
				mutexSharedInt1.lock(this);
				mutexSharedInt2.lock(this);
				mutexSharedInt3.lock(this);

				for (int i = 0; i < 10000; i++) // make it last longer, increasing chances of concurrent locking.
				{
					sharedInt1++;
					sharedInt2++;
					sharedInt3++;
				}

				mutexSharedInt1.unlock(this);
				mutexSharedInt2.unlock(this);
				mutexSharedInt3.unlock(this);

				vTaskDelay(2);
			}
		}
	}; // end class SharedNumberIncreaser

	class SyncedNumbersDisplayer : public Task
	{
	private:
		int32_t& sharedInt1;
		Mutex& mutexSharedInt1;

		int32_t& sharedInt2;
		Mutex& mutexSharedInt2;

		int32_t& sharedInt3;
		Mutex& mutexSharedInt3;

	public:
		static void StaticMain(void* pParam)
		{
			SyncedNumbersDisplayer* THIS = (SyncedNumbersDisplayer*)pParam;
			THIS->main();
		}

	public:
		SyncedNumbersDisplayer(const char* taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber,
			int32_t& sharedInt1, Mutex& mutexSharedInt1, int32_t& sharedInt2, Mutex& mutexSharedInt2, int32_t& sharedInt3, Mutex& mutexSharedInt3) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), sharedInt1(sharedInt1), mutexSharedInt1(mutexSharedInt1),
			sharedInt2(sharedInt2), mutexSharedInt2(mutexSharedInt2), sharedInt3(sharedInt3), mutexSharedInt3(mutexSharedInt3)
		{
			start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased();

				ESP_LOGI("SharedNumbersDisplayer", "Sleeping 1000ms, during which other tasks can update the numbers");
				vTaskDelay(1000);
				{
					ESP_LOGI("SharedNumbersDisplayer", "%s", taskName);
					ESP_LOGI("SharedNumbersDisplayer", "Waiting to obtain the mutexes");
					mutexSharedInt1.lock(this);
					mutexSharedInt2.lock(this);
					mutexSharedInt3.lock(this);
					ESP_LOGI("SharedNumbersDisplayer", "Mutexes obtained. no one else can alter the ints now");
					ESP_LOGI("sharedIntA", "%d", sharedInt1);
					ESP_LOGI("sharedIntB", "%d", sharedInt2);
					ESP_LOGI("sharedIntC", "%d", sharedInt3);
					ESP_LOGI("SharedNumbersDisplayer", "Sleeping 1000ms, but in spite of that no other task can change the ints");
					vTaskDelay(1);
					ESP_LOGI("SharedNumbersDisplayer", "Next three values should thus be the same as the previous three:");
					ESP_LOGI("sharedIntA", "%d", sharedInt1);
					ESP_LOGI("sharedIntB", "%d", sharedInt2);
					ESP_LOGI("sharedIntC", "%d", sharedInt3);

					ESP_LOGI("SharedNumbersDisplayer", "Releasing the mutexes");
					mutexSharedInt1.unlock(this);
					mutexSharedInt2.unlock(this);
					mutexSharedInt3.unlock(this);
				}
			}
		}
	}; // end class SharedNumbersDisplayer
};// end namespace crt
