// by Marius Versteegen, 2023

#pragma once
#include <Arduino.h>
#include <crt_CleanRTOS.h>
#include <crt_CleanGUI.h>
#include <crt_TFT_eSPI_DisplayAdapter.h>

#ifdef LOAD_GFXFF
#include <Free_Fonts.h>
#endif

#include <FreeFonts.h>

//#include "crt_string.h"

namespace crt
{
	class TestAsyncDisplay : public Task
	{
		static const int CommandBufferSize = 300;
		static const int MaxNofClientTasks = 10;
		static const int MaxNofTouchListeners = 20;

	private:
		FreeFonts<20/*MaxNofFreeFonts*/> freeFonts;		// not used when LOAD_GFXFF is not defined.
		TFT_eSPI_DisplayAdapter<MaxNofTouchListeners> tft_eSPI_DisplayAdapter;
		AsyncDisplay<CommandBufferSize, MaxNofClientTasks> displayControl;

	public:
		TestAsyncDisplay(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),tft(),
			tft_eSPI_DisplayAdapter(freeFonts, "/TouchCalData1"/*filenameCalibration*/, false/*bRepeatCalibration*/),
			displayControl("AsyncDisplay", 2 /*priority*/, 10000 + CommandBufferSize*sizeof(DisplayCommand)+
				            MaxNofClientTasks*sizeof(PrintContext) /*stackBytes*/, ARDUINO_RUNNING_CORE, tft_eSPI_DisplayAdapter)
		{
			start(); // For simplicity, the task is started right away in it's constructor.
		}

	private:
		void main()
		{
	
#ifdef LOAD_GFXFF							  // Don't select font indices 0 and 1 - they are reserved.
			freeFonts.setFreeFont(10, FSB9);  // Free serif 9 pix font.
			freeFonts.setFreeFont(12, FSB12); // Free serif 12 pix font.
#endif

			IDisplay& display = displayControl;   // To make it evident that the rest of the code is
			                                      // display independent.
			PageRoot<1> page1(display);
			
			Panel<12> panel(/*locPos*/Vec2(0, 0), CoordType::Pixels,
				/*size*/Vec2(100, 100), CoordType::Pixels, Alignment::TopLeft,
				/*colFg*/0x00FF0000, /*colBg*/0x00000000);
			page1.addChildWidget(panel);

			Text<0> textTL(/*locPos*/Vec2(0, 0), CoordType::Pixels, Alignment::TopLeft,
				"TL", /*font*/2, /*color*/0x00FFFFFF, /*scale*/1);
			Text<0> textML(/*locPos*/Vec2(0, 0), CoordType::Pixels, Alignment::MidLeft,
				"ML", /*font*/2, /*color*/0x00FFFFFF, /*scale*/1);
			Text<0> textBL(/*locPos*/Vec2(0, 0), CoordType::Pixels, Alignment::BottomLeft,
				"BL", /*font*/2, /*color*/0x00FFFFFF, /*scale*/1);
			Text<0> textTR(/*locPos*/Vec2(0, 0), CoordType::Pixels, Alignment::TopRight,
				"TR", /*font*/2, /*color*/0x00FFFFFF, /*scale*/1);
			Text<0> textMR(/*locPos*/Vec2(0, 0), CoordType::Pixels, Alignment::MidRight,
				"MR", /*font*/2, /*color*/0x00FFFFFF, /*scale*/1);
			Text<0> textBR(/*locPos*/Vec2(0, 0), CoordType::Pixels, Alignment::BottomRight,
				"BR", /*font*/2, /*color*/0x00FFFFFF, /*scale*/1);
			Text<0> textTM(/*locPos*/Vec2(0, 0), CoordType::Pixels, Alignment::TopMid,
				"TM", /*font*/2, /*color*/0x00FFFFFF, /*scale*/1);
			Text<0> textMM(/*locPos*/Vec2(0, 0), CoordType::Pixels, Alignment::MidMid,
				"MM", /*font*/2, /*color*/0x00FFFFFF, /*scale*/1);
			Text<0> textBM(/*locPos*/Vec2(0, 0), CoordType::Pixels, Alignment::BottomMid,
				"BM", /*font*/2, /*color*/0x00FFFFFF, /*scale*/1);

			panel.addChildWidget(textTL);
			panel.addChildWidget(textML);
			panel.addChildWidget(textBL);
			panel.addChildWidget(textTR);
			panel.addChildWidget(textMR);
			panel.addChildWidget(textBR);
			panel.addChildWidget(textTM);
			panel.addChildWidget(textMM);
			panel.addChildWidget(textBM);

			vTaskDelay(1000); // wait for other threads to have started up as well.

			display.fillScreen(0x00000000);

			displayControl.setPrintFont(this, 2);
			displayControl.setPrintScale(this, 2);
			displayControl.setPrintCursor(this, Vec2(10, 120));
			displayControl.setPrintColor(this, 0x0000ff00);
			displayControl.setPrintWrap(this, true);
			displayControl.print(this, "Hello ");
			displayControl.println(this, "Jack");
			displayControl.println(this, "This sentence is too long and won't fit on the screen without wrapping.");

			Vec2 locPos(0, 0);
			bool bMovingRight = true;

			Vec2 screenSize = displayControl.getScreenSize();

			while (true)
			{
				//ESP_LOGI("Currently", "clearing screen");
				page1.show(true);
				vTaskDelay(10);

				//ESP_LOGI("Currently", "displaying Panel");
				//display.fillRect(Vec2(10,10), Vec2(50,50), 0x0000FFFF);
				panel.hide(true);
				panel.setLocPos(locPos, CoordType::Pixels, Alignment::TopLeft);
				if (bMovingRight)
				{
					locPos.x++;
					if (locPos.x + panel.getSize().x >= screenSize.x) // y because rotated..
					{
						bMovingRight = false;
					}
				}
				else
				{
					locPos.x--;
					if (locPos.x == 0)
					{
						bMovingRight = true;
					}
				}

				//ESP_LOGI("variable locpos y", "%d",locPos.y);

				//vTaskDelay(10);
			}
		}
	}; // end class TestTimers
};// end namespace crt