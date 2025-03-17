#pragma once
#include "../../../main/crt/crt_CleanRTOS.h"
#include "../../../main/crt/crt__io_ISharedOutputPin.h"
#include "../../../main/crt/crt__io_ISharedOutputPinUser.h"
#include "../../../main/libs/servo/src/ESP32Servo.h"
// crt::ServoSweeper
// by Marius Versteegen, 2022

namespace crt
{
	extern crt::ILogger& logger;
	namespace ard = crt::FromArduinoIDE;

	struct SweepCommand
	{
		int startAngle=0;
		int endAngle = 0;
		int sweepTimeMs = 0;

		void Init(int startAngle, int endAngle, int sweepTimeMs)
		{
			this->startAngle	= startAngle;
			this->endAngle		= endAngle;
			this->sweepTimeMs	= sweepTimeMs;
		}
	};

	class ServoSweeper : public Task, ISharedOutputPinUser
	{
	private: 
		Servo& servo;
		Queue<SweepCommand,10> queueSweepCommands;
		SweepCommand sweepCommand;

		// If one or more servos are currently in use (that is: supposed to move),
		// their shared power pin makes sure that ServoPower is switched on.
		// Otherwise, it is switched off (avoiding humming servo noise when not needed).
		ISharedOutputPin& sharedOutputPinServoPower;

	public:
		static void StaticMain(void *pParam)
		{
			ServoSweeper* THIS = (ServoSweeper*) pParam;
			THIS->main();
		}

	public:
		ServoSweeper(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber, Servo& servo, ISharedOutputPin& sharedOutputPinServoPower) :
			Task(taskName, taskPriority, 3000, taskCoreNumber), servo(servo), queueSweepCommands(this),
			sharedOutputPinServoPower(sharedOutputPinServoPower)
		{
			sharedOutputPinServoPower.addUser(this);
		}
		
		/*override keyword not supported in current compiler*/
		void setup()
		{
			//xTaskCreatePinnedToCore(
			//	StaticMain
			//	, taskName   // A name just for humans
			//	, taskStackSizeBytes  // This stack size can be checked & adjusted by reading the Stack Highwater
			//	, this
			//	, taskPriority
			//	, NULL
			//	, taskCoreNumber);
			start();
		}

		void addSweep(int startAngle, int endAngle, int sweepTimeMs)
		{
			logger.logText("adding sweep command");
			sweepCommand.Init(startAngle, endAngle, sweepTimeMs);
			queueSweepCommands.write(sweepCommand);
		}

	private:
		void main()
		{
			vTaskDelay(10); // wait for others to initialise
			while (true)
			{
				logger.logText("waiting for sweep command");
				wait(queueSweepCommands);
				logger.logText("processing sweep command");
				queueSweepCommands.read(sweepCommand);
				
				sharedOutputPinServoPower.usePin(this); // make sure that servo power is turned on.

				int startAngle = sweepCommand.startAngle;
				int endAngle = sweepCommand.endAngle;
				int sweepTimeMs = sweepCommand.sweepTimeMs;

				int msPerStep = max(sweepTimeMs / abs(endAngle - startAngle) , 1);
				int approxNofSteps = sweepTimeMs / msPerStep;
				int angleStepSize = (endAngle - startAngle) / approxNofSteps;
				int angle = startAngle;
				logger.logInt32(msPerStep);
				logger.logInt32(approxNofSteps);
				logger.logInt32(angleStepSize);

				if (endAngle != startAngle)
				{
					if (angleStepSize == 0)
					{
						if ((endAngle - startAngle) > 0)
						{
							angleStepSize = 1;
						}
						else
						{
							angleStepSize = -1;
						}
					}
					while (true)
					{
						if (angle == endAngle) break;
						logger.logText("Angle:");
						logger.logUint32(angle);
						servo.write(angle);
						vTaskDelay(msPerStep);
						angle += angleStepSize;
						if (((angleStepSize > 0) && (angle > endAngle)) ||
							((angleStepSize < 0) && (angle < endAngle)))
						{
							angle = endAngle;
							servo.write(angle);
							vTaskDelay(msPerStep);
							break;
						}
					}
				}
				sharedOutputPinServoPower.unUsePin(this);

				vTaskDelay(1);

#ifdef CRT_HIGH_WATERMARK_INCREASE_LOGGING
				auto temp = uxTaskGetStackHighWaterMark(nullptr);
				if (!prev_stack_hwm || temp < prev_stack_hwm)
				{
					prev_stack_hwm = temp;
					ESP_LOGI("hwm_check", "Task %s has left: %d stack bytes and %d heap bytes", taskName, prev_stack_hwm, unsigned(xPortGetFreeHeapSize()));
					//ESP_LOGI(taskName,"%d",unsigned(xPortGetFreeHeapSize()));vTaskDelay(1);
					//ESP_LOGI(taskName," heap bytes","");vTaskDelay(1);
				}
#endif
			}
		}
	}; // end class crt_ServoSweeper
}; // end namespace CleanRTOS
