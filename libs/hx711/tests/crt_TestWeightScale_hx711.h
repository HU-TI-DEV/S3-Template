#pragma once
#include "crt_ILogger.h"
#include "crt_CleanRTOS.h"
//#include "crt\crt_IHandler.h"
//#include "crt\crt_Handler.h"
#include <crt_IWeightScaleListener.h>
#include <HX711.h>
#include <weightScaleHX711.h>
#include <crt_Logger.h>

namespace crt
{
	extern ILogger& logger;

	class TestweightScaleHx711 : public Task, public IWeightScaleListener
	{
	private:
		// static, or we'd need to initialize in the initializer list.
		static const unsigned int MaxNofListenersPerWeightScaleHx711 = 10;
		static const unsigned int nofHx711s = 1;

		static const uint8_t pinData0 = 39;
		static const uint8_t pinSck0 = 12;
		static const uint8_t gain0 = 128;

		static const uint8_t pinData1	= 36;
		static const uint8_t pinSck1	= 27;
		static const uint8_t gain1		= 128;

		static const int approxTimePerMeasurementMs = 100;
		static const int nofMeasurementsToAverageOver = 10; // To filter out noise. Note: each measurement takes about 100ms, because the chip needs time to prepare a new measurement.
		static const int averagedMeasurePeriodMs = approxTimePerMeasurementMs * nofMeasurementsToAverageOver;
		static const uint32_t timeOutMs = averagedMeasurePeriodMs * 2; // If no stable measurement for a while, a timeout is fired.

		//Handler<10 /*MAXLISTENERCOUNT*/> weightScaleHx711Handler; // don't forget to call its start()

		HX711 hx711_0;
		HX711 hx711_1;
		
		WeightScaleHX711<MaxNofListenersPerWeightScaleHx711> weightScaleHx711_0;
		WeightScaleHX711<MaxNofListenersPerWeightScaleHx711> weightScaleHx711_1;

	public:
		TestweightScaleHx711(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber/*, IHandler& weightScaleHx711Handler*/ /*with the desired period set*/) :
			Task(taskName, taskPriority, 10000 + nofHx711s * sizeof(WeightScaleHX711<MaxNofListenersPerWeightScaleHx711>) /*taskSizeBytes*/, taskCoreNumber),
			/*weightScaleHx711Handler("WeightScaleHx711Handler", 2, ARDUINO_RUNNING_CORE, handlerPeriodMs),*/
			hx711_0(pinData0, pinSck0, gain0), hx711_1(pinData1, pinSck1, gain1),
			weightScaleHx711_0("WeightscaleHx711_0", taskPriority, taskCoreNumber, hx711_0, nofMeasurementsToAverageOver, timeOutMs),
			weightScaleHx711_1("WeightscaleHx711_1", taskPriority, taskCoreNumber, hx711_1, nofMeasurementsToAverageOver, timeOutMs)
		{
			logger.logText("Bytes for weightScaleHx711s");
			logger.logInt32(nofHx711s * sizeof(WeightScaleHX711<MaxNofListenersPerWeightScaleHx711>));

			weightScaleHx711_0.addWeightScaleListener(this);
			weightScaleHx711_1.addWeightScaleListener(this);

			weightScaleHx711_0.enable();
			weightScaleHx711_1.enable();

			start();//setup();
		}

		///*override keyword not supported in current compiler*/
		//void setup()
		//{
		//	xTaskCreatePinnedToCore(
		//		StaticMain
		//		, taskName   // A name just for humans
		//		, taskStackSizeBytes  // This stack size can be checked & adjusted by reading the Stack Highwater
		//		, this
		//		, taskPriority  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		//		, &taskHandle
		//		, taskCoreNumber);
		//}

		// no longer /*override keyword not supported in current compiler*/
		void ReportWeight(const char* scaleName, long measurement)
		{
			ESP_LOGI(scaleName, "%d", (int32_t)measurement); vTaskDelay(1);
		}
		
		// no longer /* override */
		void ReportError(const char* scaleName, weightScaleError_t error)
		{
			//const char* wsName = (pWeightScale == &weightScaleHx711_0) 
			//	? "weightScaleHx711_0 reported:" 
			//	: "weightScaleHx711_1 reported:";

			switch (error)
			{
			case weightScaleError_t::None:
				ESP_LOGI(scaleName, "Strange, a None error was reported"); vTaskDelay(1);
				break;
			case weightScaleError_t::TimeOut:
				ESP_LOGI(scaleName, "Timout Occurred!"); vTaskDelay(1);
				break;
			default:
				ESP_LOGI(scaleName, "TODO: add weightScaleError_t to this switch statement"); vTaskDelay(1);
				break;
			}
		}

	private:
		void main()
		{
			//weightScaleHx711Handler.start();
			weightScaleHx711_0.start();
			weightScaleHx711_1.start();

			vTaskDelay(10);// allow others to initialize.

			while (true)
			{
				// Actually, this task does nothing.
				// Instead of setting flags upon receiving weightScaleHx711 presses, it prints them using
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
				vTaskDelay(1111);	// not important: this test "Task" does nothing itself..
			}
		}
	}; // end class

	// Create a "global" logger object withing namespace crt.
	const unsigned int pinButtonDump = 34; // Pressing a button connected to this pin dumps the latest logs to serial monitor.

	Logger<100> theLogger("Logger", 2 /*priority*/, ARDUINO_RUNNING_CORE, pinButtonDump); // A fast logger for 100 logs.
	ILogger& logger = theLogger;	// This is the global object. It can be accessed without knowledge of the template parameter of theLogger.

	portMUX_TYPE criticalSectionMutex = portMUX_INITIALIZER_UNLOCKED;	// The one and only mutex for critical sections in my software.

	MainInits mainInits;            // Initialize CleanRTOS.

	// Create the one and only task:
	TestweightScaleHx711 testweightScaleHx711("TestweightScaleHx711", 2 /*priority*/, ARDUINO_RUNNING_CORE);

}; // end namespace CleanRTOS


void setup()
{
	// initialize serial communication at 115200 bits per second:
	// Serial.begin(115200); // Only needed when using Serial.print();

	vTaskDelay(10);// allow tasks to initialize.
	ESP_LOGI("checkpoint", "start of main"); vTaskDelay(1);
}

void loop()
{
	vTaskDelay(1);// Nothing to do in loop - all example code runs in the 4 threads above.
}