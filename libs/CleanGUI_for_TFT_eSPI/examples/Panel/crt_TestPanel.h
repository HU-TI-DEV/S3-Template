// by Marius Versteegen, 2023

#pragma once
#include <Arduino.h>
#include <crt_CleanRTOS.h>
#include <TFT_eSPI.h>              // Hardware-specific library
#include <crt_TFT_eSPI_DisplayAdapter.h>
#include <crt_Panel.h>
#include <crt_PageRoot.h>

#ifdef LOAD_GFXFF
#include <Free_Fonts.h>
#endif

#include <crt_TFT_eSPI_FreeFonts.h>


namespace crt
{
	class TestPanel : public Task
	{
	public:
		TestPanel(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber)
		{
			start(); // For simplicity, the task is started right away in it's constructor.
		}

	private:
		static const int MaxNofTouchListeners = 20;

		void main()
		{
			TFT_eSPI_FreeFonts<20/*MaxNofFreeFonts*/> freeFonts;		// not used when LOAD_GFXFF is not defined.
			TFT_eSPI_DisplayAdapter<MaxNofTouchListeners> tft_eSPI_DisplayAdapter(freeFonts, "/TouchCalData1"/*filenameCalibration*/, false/*bRepeatCalibration*/);
			
			IDisplay& display = tft_eSPI_DisplayAdapter; // To make it evident that the rest of the code is
			                                      // display independent.
			PageRoot<1> page1("root",display);
			
			Panel<0> panel("panel 1", /*locPos*/Vec2(10, 10), CoordType::Pixels,
				/*size*/Vec2(50, 50), /*cornerRadius*/5, CoordType::Pixels, Alignment::TopLeft,
				/*colFg*/0x00FF0000, /*colBg*/0x00000000);
			page1.addChildWidget(panel);

			vTaskDelay(1000); // wait for other threads to have started up as well.

			while (true)
			{
				ESP_LOGI("Currently", "clearing screen");
				// page1.hide();
				display.fillScreen(0x00000000);

				vTaskDelay(1000);

				ESP_LOGI("Currently", "displaying Panel");
				page1.show(true);

				vTaskDelay(1000);
			}
		}
	}; // end class TestTimers
};// end namespace crt