// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>
#include <crt_MutexSection.h>

namespace crt
{
	class TestHCounter : public IHandlerListener
	{
	private:
		const char* testHCounterName;
		int32_t count;
	public:
		TestHCounter(const char *testHCounterName, IHandler& testHCounterHandler) : testHCounterName(testHCounterName), count(0)
		{
			testHCounterHandler.addHandlerListener(this);
		}

		/*override keyword not supported in current compiler*/
		void update()
		{
			ESP_LOGI(testHCounterName,"%" PRIi32,count);
			count++;
		}
	}; // end class CounterForTestHandler
};// end namespace crt
