// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>

// This file tests whether if WaitAny is used and only one HasFired() is checked,
// whether the next WaitAny immediately fires again when multiple bits were set in the first place.

namespace crt
{
	extern ILogger& logger;
  
	class ReceiveTask : public Task
	{
	private:
		Queue<int32_t, 5> queue1;
		Queue<int32_t, 5> queue2;
        Flag flag1;
		Flag flag2;

	public:
		static void StaticMain(void *pParam)
		{
			ReceiveTask* THIS = (ReceiveTask*) pParam;
			THIS->main();
		}

	public:
		ReceiveTask(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), queue1(this), queue2(this), flag1(this), flag2(this)
		{
            logger.logText("Init ReceiveTask");
			start();
		}

    // Sending buffered data to another task should be done via a member function like the 
	// one below, which writes into a Queue.
	void ReportNumber1(int32_t number)
	{
        //logger.logText("writing in q1:");
        //logger.logInt32(number);
        bool bResult = queue1.write(number);
        if (!bResult)
        {
            logger.logText("queue1 was already full");
        }
	}

	void ReportNumber2(int32_t number)
	{
        //logger.logText("writing in q2:");
        //logger.logInt32(number);
		bool bResult = queue2.write(number);
        if (!bResult)
        {
            logger.logText("queue2 was already full");
        }
	}

	// Simply Notifying another task should be done via a member function like the 
	// one below, which sets a Flag.
    void Notify1()
    {
        flag1.set();
    }

	void Notify2()
    {
        flag2.set();
    }

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			int32_t number=0;
			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

                waitAny(flag1 + queue1 + queue2 + flag2);   // Wait for any of the waitables to fire. (Note: WaitAll does exist as well).
                if (hasFired(flag1))                        // Note: using else if, only one event is handled at a time.
                {                                           
					ESP_LOGI("","Flag1 fired");
                }
                else if (hasFired(queue1))
                {
                    queue1.read(number);
                    ESP_LOGI("from queue1:","%" PRIi32,number);
                }
                else if (hasFired(queue2))
                {
                    queue2.read(number);
                    ESP_LOGI("from queue2:","%" PRIi32,number);
                }
                else if (hasFired(flag2))
                {
					ESP_LOGI("","Flag2 fired");
                }
			}
		}
	}; // end class ReceiveTask

	class SendTask : public Task
	{
	private:
		ReceiveTask& receiveTask;
		int32_t count;

	public:
		static void StaticMain(void *pParam)
		{
			SendTask* THIS = (SendTask*)pParam;
			THIS->main();
		}

	public:
		SendTask(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, ReceiveTask& receiveTask) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), receiveTask(receiveTask), count(0)
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
													  // A bit weird se3quence of processing by the receiver task.
				ESP_LOGI("","____________________");  // probably has to do with the ESP_LOGI delays.
				receiveTask.ReportNumber1(count);     
				receiveTask.ReportNumber2(count);     
				receiveTask.Notify1();				  
				receiveTask.Notify2();				  
				
				count++;
				vTaskDelay(3000);
			}
		}
	}; // end class SendTask
};// end namespace crt
