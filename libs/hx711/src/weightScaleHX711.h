#pragma once
#include <crt_CleanRTOS.h>
#include <crt_FromArduinoIDE.h>
#include "crt_IWeightScale.h"
#include "HX711.h"

// This class wraps the Hx711 class
// It allows listeners to subscribe to this service.
// It periodically broadcasts new weight measurements.

namespace ard = crt::FromArduinoIDE;

namespace crt
{
	//extern crt::ILogger& logger;

	// Don't select the MAXWEIGHTSCALELISTENERCOUNT too high, or you may suffer from stack overflow troubles.
	template<unsigned int MAXWEIGHTSCALELISTENERCOUNT> class WeightScaleHX711 : public Task// inheritance is not needed, because the listener already breaks the loop. public IWeightScale/*, public IHandlerListener*/
	{
	public:
		enum class WeightScaleState { IDLE, ACTIVE };

		static void StaticMain(void *pParam)
		{
			WeightScaleHX711* THIS = (WeightScaleHX711*) pParam;
			THIS->main();
		}

	private:
		HX711& hx711;

		IWeightScaleListener* arWeightScaleListeners[MAXWEIGHTSCALELISTENERCOUNT] = {};
		uint16_t nofWeightScaleListeners;

		WeightScaleState state;
		WeightScaleState prevState;

		int nofMeasurementsToAverageOver;
		int prevRead;
		uint32_t timeOutMs;
		uint64_t prevTimeSuccessfulMeasurementMs;

	public:
		WeightScaleHX711(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber, HX711& hx711, int nofMeasurementsToAverageOver, uint32_t timeOutMs/*, IHandler& weightScaleHandler*/) :
			Task(taskName, taskPriority, 3500 + MAXWEIGHTSCALELISTENERCOUNT * sizeof(IWeightScaleListener*), taskCoreNumber),
			hx711(hx711), nofWeightScaleListeners(0), state(WeightScaleState::IDLE), prevState(WeightScaleState::IDLE),
			nofMeasurementsToAverageOver(nofMeasurementsToAverageOver), prevRead(0), timeOutMs(timeOutMs), prevTimeSuccessfulMeasurementMs(0)
		{
			for (int i = 0;i < MAXWEIGHTSCALELISTENERCOUNT;i++)
			{
				arWeightScaleListeners[i] = nullptr;
			}
			//memset(arWeightScaleListeners, 0, sizeof(IWeightScaleListener*) * MAXWEIGHTSCALELISTENERCOUNT);
			//weightScaleHandler.addHandlerListener(this);
		}

		/*override keyword not supported in current compiler*/
		void setup()
		{
			xTaskCreatePinnedToCore(
				StaticMain
				, taskName   // A name just for humans
				, taskStackSizeBytes  // This stack size can be checked & adjusted by reading the Stack Highwater
				, this
				, taskPriority
				, NULL
				, taskCoreNumber);
		}

		/*override keyword not supported in current compiler*/
		void addWeightScaleListener(IWeightScaleListener* pWeightScaleListener)
		{
			if (!isAlreadyPresent(pWeightScaleListener))
			{
				assert(nofWeightScaleListeners < MAXWEIGHTSCALELISTENERCOUNT);
				arWeightScaleListeners[nofWeightScaleListeners++] = pWeightScaleListener;
			}
		}

		/*override*/
		void setScale(float scale)
		{
			hx711.set_scale(scale);
		}

		/*override*/
		float getScale()
		{
			return hx711.get_scale();
		}

		/*override*/
		void setOffset(long offset)
		{
			hx711.set_offset(offset);
		}

		/*override*/
		long getOffset()
		{
			return hx711.get_offset();
		}

		// wakes up the chip after power down mode and start periodically weighing and broadcasting
		/*override*/
		void enable()
		{
			hx711.power_up();
			state = WeightScaleState::ACTIVE;
		}

		// stop periodically weighing and broadcasting
		/*override*/
		void disable()
		{
			state = WeightScaleState::IDLE;
			hx711.power_down();
		}

	private:
		// It is assumed that this update function is called by a WeightScaleHandler once in about 70ms: slow enough for debounce, fast enough for response.
/*override keyword not supported in current compiler*/
		void main()
		{
			while (true)
			{
				//logger.logText("weightScaleUpdate Called");

				// (re-) start the stopwatch for timeout when the weightscale gets activated.
				if ((state == WeightScaleState::ACTIVE) && (prevState == WeightScaleState::IDLE))
				{
					logger.logText("weightscale activated");
					prevTimeSuccessfulMeasurementMs = ard::millis();
				}

				if (state == WeightScaleState::ACTIVE)
				{
					if (hx711.is_ready())
					{
						uint64_t before = ard::millis();
						long reading = hx711.read_average(nofMeasurementsToAverageOver);
						uint64_t after = ard::millis();
						logger.logText("averaging out");
						logger.logUint32(nofMeasurementsToAverageOver);
						logger.logUint32((uint32_t)(after - before));

						// .. prevent division by zero
						logger.logText("absdif, absread+1, div:");
						logger.logFloat(float(abs(reading - prevRead)));
						logger.logFloat(float(abs(reading) + 1));
						logger.logFloat(float(abs(reading - prevRead)) / (float(abs(reading) + 1)));
						if ((float(abs(reading - prevRead)) / (float(abs(reading) + 1))) < 0.05f)
						{
							// Two subsequent measurements don't differ too much.
							for (int i = 0; i < nofWeightScaleListeners; i++)
							{
								arWeightScaleListeners[i]->ReportWeight(taskName, reading);
							}
							logger.logText("time since previous successful meausurement:");
							logger.logUint32(ard::millis() - prevTimeSuccessfulMeasurementMs);
							prevTimeSuccessfulMeasurementMs = ard::millis();
						}
						else
						{
							logger.logText("logfailed");
						}
						prevRead = reading;

						uint64_t curTimeMillis = ard::millis();
						if (curTimeMillis > (prevTimeSuccessfulMeasurementMs + timeOutMs))
						{
							logger.logText("processing timeout");
							for (int i = 0; i < nofWeightScaleListeners; i++)
							{
								arWeightScaleListeners[i]->ReportError(taskName, weightScaleError_t::TimeOut);
							}

							prevTimeSuccessfulMeasurementMs = curTimeMillis + 1000000000; // Prevent to repeat reporting the timeout (too often).
						}
					}
				}

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
				vTaskDelay(10);
				prevState = state;
			}
		}

		long maxLong(long a, long b)
		{
			return (a >= b) ? a : b;
		}

		bool isAlreadyPresent(IWeightScaleListener* pWeightScaleListener)
		{
			for (int i = 0; i < nofWeightScaleListeners; i++)
			{
				//ESP_LOGI("adasdffasfsadf", "%s", i);
				if (arWeightScaleListeners[i] == pWeightScaleListener)
				{
					return true;
				}
			}
			return false;
		}
	}; // end class WeightScale
}; // end namespace CleanRTOS
