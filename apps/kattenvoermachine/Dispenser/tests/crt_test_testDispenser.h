#pragma once
#include "..\..\..\..\main\crt\crt_ILogger.h"
#include "..\..\..\..\main\crt\crt_Task.h"
#include "..\..\..\..\main\crt\crt_IHandler.h"
#include "..\..\..\..\main\crt\crt_Handler.h"

#include "..\..\..\..\main\crt\crt__io_IButtonListener.h"
#include "..\..\..\..\main\crt\crt__io_Button.h"

#include "..\..\..\..\main\crt\crt__io_SharedOutputPin.h"

#include "..\..\..\..\main\libs/servo/src/ESP32Servo.h"
#include "..\..\..\..\apps\kattenvoermachine\ServoSweeper\crt_ServoSweeper.h"
#include "..\..\..\..\apps\kattenvoermachine\Dispenser\crt_Dispenser.h"

namespace crt
{
	extern crt::ILogger& logger;

	class TestDispenser : public Task, public IButtonListener
	{
	private:
		Handler<10 /*MAXLISTENERCOUNT*/> buttonHandler;

		// static, or we'd need to initialize in the initializer list.
		static const unsigned int MaxNofListenersPerButton = 10;
		static const unsigned int nofButtons = 2;
		static const unsigned int pinButton1 = 22;
		static const unsigned int pinButton2 = 21;
		Button<MaxNofListenersPerButton> button1;
		Button<MaxNofListenersPerButton> button2;
		
		static const unsigned int pinServoPower = 25;

		static const unsigned int pinServoLuik0 = 12;
		static const unsigned int pinServoLuik1 = 13;
		static const unsigned int pinServoRoeren = 14;
	
		Servo servoLuik0;  // create servo object to control a servo
		Servo servoLuik1;  // create servo object to control a servo
		Servo servoRoeren;
		// 16 servo objects can be created on the ESP32

		static const unsigned int MaxNofUsersPerSharedOutputPin = 5;
		crt::SharedOutputPin<5> sharedOutputPin;

		crt::ServoSweeper servoSweeperLuik0;
		crt::ServoSweeper servoSweeperLuik1;
		crt::ServoSweeper servoSweeperRoeren;

		crt::Dispenser dispenser0;
		crt::Dispenser dispenser1;
			
	public:
		static void StaticMain(void *pParam)
		{
			TestDispenser* THIS = (TestDispenser*)pParam;
			THIS->main();
		}

	public:
		TestDispenser(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, 3000 + nofButtons * sizeof(Button<MaxNofListenersPerButton>) /*taskSizeBytes*/, taskCoreNumber),
			buttonHandler("ButtonHandler", 2 /*priority*/, ARDUINO_RUNNING_CORE, 70 /*periodMs*/),
			button1("Button1", pinButton1, false, buttonHandler),
			button2("Button2", pinButton2, false, buttonHandler),
			sharedOutputPin(pinServoPower, true /*positiveLogic*/),
			servoSweeperLuik0("ServoSweeperLuik0", 2 /*priority*/, ARDUINO_RUNNING_CORE, servoLuik0, sharedOutputPin),
			servoSweeperLuik1("ServoSweeperLuik1", 2 /*priority*/, ARDUINO_RUNNING_CORE, servoLuik1, sharedOutputPin),
			servoSweeperRoeren("ServoSweeper", 2 /*priority*/, ARDUINO_RUNNING_CORE, servoRoeren, sharedOutputPin),
			dispenser0(servoSweeperLuik0, servoSweeperRoeren), 
			dispenser1(servoSweeperLuik1, servoSweeperRoeren)
		{
			button1.addButtonListener(this);
			button2.addButtonListener(this);

			logger.logText("Bytes for buttons");
			logger.logInt32(nofButtons * sizeof(Button<MaxNofListenersPerButton>));
		}

		/*override keyword not supported in current compiler*/
		void setup()
		{
			//xTaskCreatePinnedToCore(
			//	StaticMain
			//	, taskName   // A name just for humans
			//	, taskStackSizeBytes  // This stack size can be checked & adjusted by reading the Stack Highwater
			//	, this
			//	, taskPriority  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
			//	, &taskHandle
			//	, taskCoreNumber);

			// Allow allocation of all timers
			ESP32PWM::allocateTimer(0);
			ESP32PWM::allocateTimer(1);
			ESP32PWM::allocateTimer(2);
			ESP32PWM::allocateTimer(3);
			servoLuik0.setPeriodHertz(50);    // standard 50 hz servo
			servoLuik0.attach(pinServoLuik0, 500, 2500);
			servoLuik1.setPeriodHertz(50);    // standard 50 hz servo
			servoLuik1.attach(pinServoLuik1, 500, 2500);
			servoRoeren.setPeriodHertz(50);    // standard 50 hz servo
			servoRoeren.attach(pinServoRoeren, 500, 2500);

			// setup and start the subtasks
			buttonHandler.setup();
			servoSweeperLuik0.setup();
			servoSweeperLuik1.setup();
			servoSweeperRoeren.setup();

			start(); // starts this Task.
		}

		/*override keyword not supported in current compiler*/
		void BnPress(IButton* pButton)
		{
			logger.logText("a button was pressed");
			if (pButton == &button1)
			{
				ESP_LOGI("button pressed", "button 1"); vTaskDelay(1);
				dispenser0.dispensePortion();
			}
			else if (pButton == &button2)
			{
				ESP_LOGI("button pressed", "button 2"); vTaskDelay(1);
				dispenser1.dispensePortion();
			}
			else
			{
				ESP_LOGI("button pressed", "unknown"); vTaskDelay(1);
			}
		}
		
	private:
		void main()
		{
			vTaskDelay(10);// allow others to initialize.

			while (true)
			{
				// Actually, this task does nothing.
				// Instead of setting flags upon receiving button presses, it prints them using
				// ESP_LOGI right away. (note: set logging level to info at least to make it work)

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
				vTaskDelay(1000);
			}
		}
	}; // end class Logger
}; // end namespace CleanRTOS