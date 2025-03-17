// by Marius Versteegen, 2023

#pragma once
#include "crt_CleanRTOS.h"
#include "crt_IButton.h"
#include "crt_ILogger.h"
#include <crt_ITouchListener.h>

// for testing only (requires presence of a global logger object):
// #define TEST_TOUCHDETECTOR

namespace crt
{
	//extern ILogger& logger;

	// Don't select the MaxTouchListenerCount too high, or you may suffer from stack overflow troubles.
	template<size_t MaxTouchListenerCount> class TFT_eSPI_TouchDetector
	{
	private:
		const uint64_t infiniteBatchSizeUs = 1000000000000; // 1e6 s means: infinite: no limitation in batchsize.

		TFT_eSPI& _tft;
		bool bPrevPressed;
		ITouchListener* arTouchListeners[MaxTouchListenerCount] = {}; // This intializes all to 0.
		uint16_t nofTouchListeners;
		uint16_t t_x, t_y; // To store the touch coordinates
		Vec2 posLastPress;
		uint64_t batchSizeUs;

	public:
		// Make sure that the update period of the handler that calls update() is about 70ms:
		// Slow enough for debounce, fast enough for response.
		// By default, allow an infinite batch size.
		TFT_eSPI_TouchDetector(TFT_eSPI& tft) :
			_tft(tft), bPrevPressed(false), nofTouchListeners(0), t_x(9999), t_y(9999), batchSizeUs(1000000000000)
		{
		}

		// Typically, if many buttons are directly assigned to the TouchDetector,
		// it may be a good idea to limit batchSizeUs, such that tasks of equal priority
		// also get their turns without waiting very long.
		TFT_eSPI_TouchDetector(TFT_eSPI& tft, uint64_t batchSizeUs) :
			_tft(tft), bPrevPressed(false), nofTouchListeners(0), t_x(9999), t_y(9999), batchSizeUs(batchSizeUs)
		{
		}

		/*override*/
		void addTouchListener(ITouchListener* pTouchListener)
		{
			if (!isAlreadyPresent(pTouchListener))
			{
				assert(nofTouchListeners < MaxTouchListenerCount);
				arTouchListeners[nofTouchListeners++] = pTouchListener;
			}
		}

		/*override*/
		bool isTouched()
		{
			return bPrevPressed;
		}

		// It is assumed that this update function is called by a handler once in about 70ms: slow enough for debounce, fast enough for response.
		/*override keyword not supported in current compiler*/
		void update(void* pCallingTask)
		{
			bool bPressed = _tft.getTouch(&t_x, &t_y);
			if (bPressed)
			{
				posLastPress.set(t_x, t_y);
				if (!bPrevPressed)
				{
					doUpdatePressedOrReleased(true, pCallingTask); // true means (just-) pressed.
				    /*for (int i = 0; i < nofTouchListeners; i++)
					{
						arTouchListeners[i]->touchPressed(posLastPress);
					}*/
				}
			}
			else if (!bPressed && bPrevPressed)
			{
				doUpdatePressedOrReleased(false, pCallingTask); // false means (just-) released.
				/*for (int i = 0; i < nofTouchListeners; i++)
				{
					arTouchListeners[i]->touchReleased(posLastPress);
				}*/
			}
			bPrevPressed = bPressed;
		}

	private:
		void doUpdatePressedOrReleased(bool bPressed, void* pCallingTask_in)
		{
#ifdef CleanRTOS_INCLUDED
			Task* pCallingTask = (Task*)pCallingTask_in;
#else
			void* pCallingTask = pCallingTask_in;
#endif

#ifdef TEST_TOUCHDETECTOR
			uint64_t before2 = esp_timer_get_time();	// For measuring of actual complete period.
			uint64_t after2 = 0;
			uint64_t diff2 = 0;
			int32_t  nofYieldsDone = 0;
			uint64_t yieldTimeSpent = 0;
			uint64_t beforeBatches = 0;
			uint64_t spentOnBatches = 0;
			uint64_t beforeAll = esp_timer_get_time();
			uint64_t afterAll = 0;	// gross Time spent by Handler, including yields to other threads.
#endif
			uint64_t beforeBatch = 0;
			uint64_t batchTimeSpent = 0;

			beforeBatch = esp_timer_get_time();

#ifdef TEST_TOUCHDETECTOR
			beforeAll = esp_timer_get_time();
			uint32_t diffAll = 0;
			yieldTimeSpent = 0;
			nofYieldsDone = 0;
			beforeBatches = beforeBatch;
#endif				

			for (int i = 0; i < nofTouchListeners; i++)
			{
				// Do the prober update:
				if (bPressed)
				{
					arTouchListeners[i]->touchPressed(posLastPress);
				}
				else
				{
					arTouchListeners[i]->touchReleased(posLastPress);
				}

				if (batchSizeUs != infiniteBatchSizeUs)
				{
					uint64_t nowUs = esp_timer_get_time();
					batchTimeSpent = (nowUs - beforeBatch);

					if (batchTimeSpent > batchSizeUs)
					{
						taskYIELD();

#ifdef TEST_TOUCHDETECTOR
						yieldTimeSpent += (esp_timer_get_time() - nowUs);
						nofYieldsDone++;
#endif
						beforeBatch = esp_timer_get_time();
					}
				}
			}
			
			// This is the time spent on batches,
			// yields and optionally the test code
			// within the #defines below.
			//if (diffAll >= periodUs)
			//{
			//	ESP_LOGI("Handler Delay too long:", "%d us", diffAll);
			//}

#ifdef TEST_TOUCHDETECTOR
			afterAll = esp_timer_get_time();     // time in micro seconds.
			diffAll = (afterAll - beforeAll);    // difference in milliseconds
			after2 = esp_timer_get_time();
			diff2 = (uint32_t)(after2 - before2);
			before2 = after2;

			spentOnBatches = afterAll - beforeBatches;

			logger.logText("-----------------------------------------------------");
			logger.logText(Task::taskName);
			logger.logText("Diagnostics:");
			logger.logText("nofYieldsDone:");
			logger.logUint32(nofYieldsDone);
			logger.logText("gross time spent on batches in microsecs, including yields:");
			logger.logUint32((int32_t)spentOnBatches);
			logger.logText("nett time spent on batches in microsecs:");
			logger.logUint32((int32_t)(spentOnBatches - yieldTimeSpent));
			logger.logText("handler interval microsecs");
			logger.logUint32((int32_t)diff2);
#endif

			if (pCallingTask != nullptr)
			{
				pCallingTask->dumpStackHighWaterMarkIfIncreased();
			}
		}

		bool isAlreadyPresent(ITouchListener* pTouchListener)
		{
			for (int i = 0; i < nofTouchListeners; i++)
			{
				//ESP_LOGI("adasdffasfsadf", "%s", i);
				if (arTouchListeners[i] == pTouchListener)
				{
					return true;
				}
			}
			return false;
		}
	}; // end class Button
}; // end namespace CleanRTOS
