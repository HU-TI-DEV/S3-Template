// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>

// This file contains the code of a task that sends strings to another task, which displays them.
// The strings are transferred via the Queue synchronization mechanism.

// To see the test output in the serial monitor, just press the button that is assigned
// to the logger.

namespace crt
{
	extern crt::ILogger& logger;
  
	class StringDisplayTask : public Task
	{
	private:
		Queue<const char*, 15> queueStrings;

	public:
		StringDisplayTask(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCorestring) :
			Task(taskName, taskPriority, taskSizeBytes, taskCorestring), queueStrings(this)
		{
			start();
		}

    // The function below is called by an other thread, thus passing on the strings that is wants to get displayed.
	void displaystring(const char* string)
	{
        bool bResult = queueStrings.write(string);
        if (!bResult)
        {
			ESP_LOGI("stringDisplayTask", "OOPS! queuestring was already full!");vTaskDelay(1);
        }
	}

	private:
		/*override keyword not supported*/
		void main()
		{
			const char* string="";
			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				wait(queueStrings);
                queueStrings.read(string);
				ESP_LOGI("stringDisplayTask","%s",string);
			}
		}
	}; // end class stringDisplayTask

	class StringSendTask : public Task
	{
	private:
		StringDisplayTask& stringDisplayTask;

	public:
		StringSendTask(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCorestring, StringDisplayTask& stringDisplayTask) :
			Task(taskName, taskPriority, taskSizeBytes, taskCorestring), stringDisplayTask(stringDisplayTask)
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
				
				// Send a burst of 5 strings to the NumerDisplayTask.
				for(int n=0;n<5;n++)
				{
					stringDisplayTask.displaystring("a");
					stringDisplayTask.displaystring("b");
					stringDisplayTask.displaystring("c");
				}
				
				vTaskDelay(1000);
			}
		}
	}; // end class stringSendTask
};// end namespace crt
