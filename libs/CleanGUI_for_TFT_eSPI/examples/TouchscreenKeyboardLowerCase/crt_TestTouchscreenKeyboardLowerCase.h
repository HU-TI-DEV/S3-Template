// by Marius Versteegen, 2023

// To view ESP_LOGI output messages in Arduino IDE, 
// set Tools -> Core Debug Level to "info" 
// and Serial Monitor baud rate to 115200
// When using ESP_IDF, make sure that CONFIG_LOG_DEFAULT_LEVEL_INFO=y in sdkconfig

#pragma once
#include <Arduino.h>
#include <crt_CleanRTOS.h>
#include <crt_CleanGUI.h>
#include <crt_IKeyboardListener.h>
#include <crt_TouchscreenKeyboardLowerCase.h>
#include <crt_AsyncDisplay.h>
#include <crt_TFT_eSPI_DisplayAdapter.h>

#ifdef LOAD_GFXFF
#include <Free_Fonts.h>
#endif

#include <crt_TFT_eSPI_FreeFonts.h>

namespace crt
{
	class TestTouchscreenKeyboardLowerCase : public Task, public IKeyboardListener
	{
		static const int CommandBufferSize = 300;
		static const int MaxNofClientTasks = 10;
		static const int MaxNofTouchListeners = 20;

	private:
		TFT_eSPI_FreeFonts<20/*MaxNofFreeFonts*/> freeFonts;		// not used when LOAD_GFXFF is not defined.
		TFT_eSPI_DisplayAdapter<MaxNofTouchListeners> tft_eSPI_DisplayAdapter;
		AsyncDisplay<CommandBufferSize, MaxNofClientTasks> asyncDisplay;
		Panel<1> keyboardPanel;										// this panel parents and determines the size of the 
																    // keyboard that is assigned to it.

		TouchscreenKeyboardLowerCase<0 /*MaxNofChildren*/, 1 /*MaxNofKeyboardListeners*/> touchscreenKeyboardLowerCase;

		Queue<const char*, 10> queueKeyPressedStrings;
		Queue<const char*, 10> queueKeyReleasedStrings;
		
	public:
		TestTouchscreenKeyboardLowerCase(const char *taskName, unsigned int taskPriority,unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, 
				10000+sizeof(freeFonts)+sizeof(tft_eSPI_DisplayAdapter)+sizeof(asyncDisplay)+
				sizeof(touchscreenKeyboardLowerCase) + sizeof(queueKeyPressedStrings) + 
				sizeof(queueKeyReleasedStrings), taskCoreNumber),

			tft_eSPI_DisplayAdapter(freeFonts, "/TouchCalData1"/*filenameCalibration*/, false/*bRepeatCalibration*/),

			asyncDisplay("AsyncDisplay", 2 /*priority*/, 20000 + CommandBufferSize*sizeof(DisplayCommand)+
				            MaxNofClientTasks*sizeof(PrintContext) /*stackBytes*/, ARDUINO_RUNNING_CORE, 
				            tft_eSPI_DisplayAdapter, 0/*rotation*/, 2/*fontTouchCalibration*/),

			keyboardPanel("keyboardPanel", /*locPos*/Vec2(0, 500), CoordType::Promillage,
				/*size*/Vec2(1000, 500), /*cornerRadius*/0, CoordType::Promillage, Alignment::TopLeft,
				/*colFg*/(uint32_t)0x00888888, /*colBg*/0x00000000),

			touchscreenKeyboardLowerCase("keybd"),           // no parameters, so this line could be left away.
			queueKeyPressedStrings(this),
			queueKeyReleasedStrings(this)
		{
			start(); // For simplicity, the task is started right away in it's constructor.
		}

	public:
		/* override */ void keyPressed(const char* strKey)
		{
			queueKeyPressedStrings.write(strKey);
		}

		/* override */ void keyReleased(const char* strKey)
		{
			queueKeyReleasedStrings.write(strKey);
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
			PageRoot<10> page1("root", display);
			asyncDisplay.addTouchListener(&touchscreenKeyboardLowerCase);

			touchscreenKeyboardLowerCase.addKeyboardListener(this);

			ESP_LOGI("testkb", "88");

			page1.addChildWidget(keyboardPanel);
			keyboardPanel.addChildWidget(touchscreenKeyboardLowerCase);
			keyboardPanel.show(true);

			const char* str;

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				waitAny(queueKeyPressedStrings+queueKeyReleasedStrings);

				if (hasFired(queueKeyPressedStrings))
				{
					queueKeyPressedStrings.read(str);
					ESP_LOGI(Task::taskName, "Key Released:%s", str);
				}
				else if (hasFired(queueKeyReleasedStrings))
				{
					queueKeyReleasedStrings.read(str);
					ESP_LOGI(Task::taskName, "Key Released:%s", str);
				}
				else
				{
					ESP_LOGI(Task::taskName, "unhandled waitable");
				}
			}
		}
	};
}// end namespace crt