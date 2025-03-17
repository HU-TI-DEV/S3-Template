// by Marius Versteegen, 2023

#pragma once
#include <Arduino.h>
#include <crt_CleanRTOS.h>
#include <crt_CleanGUI.h>
#include <crt_AsyncDisplay.h>
#include <crt_TouchscreenButton.h>
#include <crt_TFT_eSPI_DisplayAdapter.h>

#ifdef LOAD_GFXFF
#include <Free_Fonts.h>
#endif

#include <crt_TFT_eSPI_FreeFonts.h>

namespace crt
{
	class TestTouchscreenButton : public Task, public IButtonListener
	{
		static const int CommandBufferSize = 300;
		static const int MaxNofClientTasks = 10;
		static const int MaxNofTouchListeners = 20;

	private:
		TFT_eSPI_FreeFonts<20/*MaxNofFreeFonts*/> freeFonts;		// not used when LOAD_GFXFF is not defined.
		TFT_eSPI_DisplayAdapter<MaxNofTouchListeners> tft_eSPI_DisplayAdapter;
		AsyncDisplay<CommandBufferSize, MaxNofClientTasks> asyncDisplay;

		TouchscreenButton<0 /*MaxNofChildren*/, 10/*MaxButtonListenerCount*/> tsButtonA;
		TouchscreenButton<0 /*MaxNofChildren*/, 10/*MaxButtonListenerCount*/> tsButtonB;

		Flag flagBnAPressed, flagBnAReleased;
		Flag flagBnBPressed, flagBnBReleased;

	public:
		TestTouchscreenButton(const char *taskName, unsigned int taskPriority,unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, 
				10000+sizeof(freeFonts)+sizeof(tft_eSPI_DisplayAdapter)+sizeof(asyncDisplay)+
				sizeof(tsButtonA)+sizeof(tsButtonB), taskCoreNumber),

			tft_eSPI_DisplayAdapter(freeFonts, "/TouchCalData1"/*filenameCalibration*/, false/*bRepeatCalibration*/),

			asyncDisplay("AsyncDisplay", 2 /*priority*/, 20000 + CommandBufferSize*sizeof(DisplayCommand)+
				            MaxNofClientTasks*sizeof(PrintContext) /*stackBytes*/, ARDUINO_RUNNING_CORE, 
				            tft_eSPI_DisplayAdapter, 0/*rotation*/, 2/*fontTouchCalibration*/),

			// fontScale=0 triggers automatical matching of fontscale with the button height.
			tsButtonA("bn A",/*locPos*/Vec2(100, 100), CoordType::Promillage,
				/*size*/Vec2(500, 100), /*cornerRadius*/100, CoordType::Promillage,
				Alignment::MidMid,	/*colPanel*/0x00FF0000, /*colBg*/0x00000000,
				"Button A", /*buttonFont*/ 2, /*colFont*/ 0x00FFFFFF, /*fontScale*/ 0, Alignment::MidMid),

			tsButtonB("bn B",/*locPos*/Vec2(100, 250), CoordType::Promillage,
				/*size*/Vec2(500, 100), /*cornerRadius*/100, CoordType::Promillage,
				Alignment::MidMid,	/*colPanel*/0x00FF0000, /*colBg*/0x00000000,
				"Button B", /*buttonFont*/ 2, /*colFont*/ 0x00FFFFFF, /*fontScale*/ 0, Alignment::MidMid),

			flagBnAPressed(this), flagBnAReleased(this),
			flagBnBPressed(this), flagBnBReleased(this)
		{
			start(); // For simplicity, the task is started right away in it's constructor.
		}

	public:
		/* override */ void buttonPressed(IButton* pButton)
		{
			if (pButton == &tsButtonA)
			{
				flagBnAPressed.set();
			}
			else if (pButton == &tsButtonB)
			{
				flagBnBPressed.set();
			}
			else
			{
				ESP_LOGI(Task::taskName, "unhandled buttonpress");
			}
		}

		/* override */ void buttonReleased(IButton* pButton)
		{
			if (pButton == &tsButtonA)
			{
				flagBnAReleased.set();
			}
			else if (pButton == &tsButtonB)
			{
				flagBnBReleased.set();
			}
			else
			{
				ESP_LOGI(Task::taskName, "unhandled buttonpress");
			}
		}

	private:
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

#ifdef LOAD_GFXFF							  // Don't select font indices 0 and 1 - they are reserved.
			freeFonts.setFreeFont(10, FSB9);  // Free serif 9 pix font.
			freeFonts.setFreeFont(12, FSB12); // Free serif 12 pix font.
#endif
			IDisplay& display = asyncDisplay;   // To make it evident that the rest of the code is
			                                      // display independent.
			PageRoot<10> page1("root",display);

			asyncDisplay.addTouchListener(&tsButtonA);
			asyncDisplay.addTouchListener(&tsButtonB);
			tsButtonA.addButtonListener(this);
			tsButtonB.addButtonListener(this);

			page1.addChildWidget(tsButtonA);
			page1.addChildWidget(tsButtonB);
			page1.show(true);

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				waitAny(flagBnAPressed + flagBnAReleased + flagBnBPressed + flagBnBReleased);
				if (hasFired(flagBnAPressed))
				{
					ESP_LOGI(taskName, "Button A pressed");
				}
				else if (hasFired(flagBnAReleased))
				{
					ESP_LOGI(taskName, "Button A released");
				}

				if (hasFired(flagBnBPressed))
				{
					ESP_LOGI(taskName, "Button B pressed");
				}
				else if (hasFired(flagBnBReleased))
				{
					ESP_LOGI(taskName, "Button B released");
				}
			}
		}
	}; // end class TestTimers
};// end namespace crt