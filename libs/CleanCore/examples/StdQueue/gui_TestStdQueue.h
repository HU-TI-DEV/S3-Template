// by Marius Versteegen, 2023

#pragma once
#include <Arduino.h>
#include <crt_CleanRTOS.h>
#include <TFT_eSPI.h>              // Hardware-specific library
#include <crt_TFT_eSPI_DisplayAdapter.h>
#include <crt_Panel.h>
#include <crt_PageRoot.h>

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
		void main()
		{
			TFT_eSPI tft = TFT_eSPI();
			tft.begin();
			tft.setRotation(0);
			tft.fillScreen(TFT_BLACK);
			tft.setFreeFont(FF18);

			const char* filenameCalibration = "/TouchCalData1";
			bool bRepeatCalibration = false;
			TFT_eSPI_DisplayAdapter tft_eSPI_DisplayAdapter(tft, filenameCalibration, bRepeatCalibration);
			tft_eSPI_DisplayAdapter.touchCalibrate();
			
			IDisplay& display = tft_eSPI_DisplayAdapter; // To make it evident that the rest of the code is
			                                      // display independent.
			PageRoot<1> page1(display);
			
			Panel<0> panel(/*locPos*/Vec2(10, 10), CoordType::Pixels,
				/*size*/Vec2(50, 50), CoordType::Pixels, Alignment::TopLeft,
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