// by Marius Versteegen, 2023
#pragma once
#include <crt_CleanRTOS.h>
#include "crt_typesAndConstants.h"
#include <crt_Vec2.h>
#include <crt_IDisplay.h>
#include <crt_DisplayCommand.h>
#include <crt_PrintContext.h>

namespace crt
{
	// Via AsyncDisplay, multiple threads can asynchronously send commands to a (shared) display.

	template <size_t CommandBufferSize, size_t MaxNofClientTasks>
	class AsyncDisplay : public Task, public IDisplay
	{
	private:
		IDisplay& display;
		PrintContext arPrintContext[MaxNofClientTasks];
		int nofPrintContexts;
		Queue<DisplayCommand, CommandBufferSize> queueCommands;
		Timer periodicTimer;
		Flag flagTouchCalibrate;
		DisplayCommand curCommand;
		uint8_t rotation;
		uint8_t fontTouchCalibration;

	public:
		AsyncDisplay(const char* taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, 
			IDisplay& display, uint8_t rotation, uint8_t fontTouchCalibration) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), display(display), nofPrintContexts(0), queueCommands(this), 
			periodicTimer(this), flagTouchCalibrate(this), rotation(rotation), fontTouchCalibration(fontTouchCalibration)
		{
			start(); // For simplicity, the task is started right away in it's constructor.
		}

	private:
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.
			periodicTimer.start_periodic(100000); // 100ms Polling period for touch handling.

			PrintContext* pPrintContext = nullptr;

			display.touchCalibrate(rotation, fontTouchCalibration);

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				waitAny(queueCommands+periodicTimer+flagTouchCalibrate);
				
				if (hasFired(queueCommands))
				{
					queueCommands.read(curCommand);

					// curCommand.dumpWithESP_LOGI(); vTaskDelay(1); // for debugging purposes.

					pPrintContext = queryPrintContext(curCommand.getTask());
					assert(pPrintContext != nullptr);
					curCommand.execute(display, *pPrintContext);
				}
				
				if (hasFired(periodicTimer))
				{
					display.pollTouch(this);
				}
			}
		}

	private:
		PrintContext* queryPrintContext(const Task* pTask)
		{
			PrintContext* pPrintContext = nullptr;
			int i = 0;
			for (i = 0; i < nofPrintContexts; i++)
			{
				pPrintContext = &(arPrintContext[i]);
				if (pPrintContext->pTask == pTask)
				{
					break;
				}
			}
			if (i == nofPrintContexts)
			{
				// There was no print context yet for this task.
				if (nofPrintContexts < MaxNofClientTasks)
				{
					pPrintContext = &(arPrintContext[nofPrintContexts++]);
					pPrintContext->pTask = pTask;
				}
				else
				{
					// Graceful degradation. Just use the last printContext.
					ESP_LOGI("AsyncDisplay", "MaxNofClientTasks constant is set too low.");
				}
			}
			return  pPrintContext;
		}

		// The interface functions:
	public:
		/*override*/ void pollTouch(void* pCallingTask)
		{
			// no implementation needed for AsyncDisplay.
			// This Display Control object itself calls the function 
			// in the main-loop.
		}

		/*override*/ void touchCalibrate(uint8_t rotation, uint8_t font)
		{
			ESP_LOGI("AsyncDisplay", "Stub - don't call this function directly.");
			ESP_LOGI("AsyncDisplay", "AsyncDisplay automatically handles this at startup");
		}

		/*override*/ void addTouchListener(ITouchListener* pTouchListener)
		{
			display.addTouchListener(pTouchListener);
		}

		/* override */ Vec2 getScreenSize()
		{
			return display.getScreenSize();
		}

		/*override*/ int16_t getTextWidth(const char* str, uint8_t font)
		{
			return display.getTextWidth(str, font);
		}

		/*override*/ int16_t getFontHeight(uint8_t font)
		{
			return display.getFontHeight(font);
		}

		/*override*/ Vec2 getPrintCursor()
		{
			return display.getPrintCursor();
		}

		///*override*/ void setFontScale(uint8_t fontScale)
		//{
		//	ESP_LOGI("AsyncDisplay", "You should not directly set the font scale because it could interfere with");
		//	ESP_LOGI("AsyncDisplay", "the asynchronous command of another task.");
		//	ESP_LOGI("AsyncDisplay", "Instead, pass the font scale along with a drawText call.");
		//	display.setFontScale(fontScale);
		//}

		/* override */ void fillScreen(uint32_t color)
		{
			DisplayCommand displayCommand;
			displayCommand.fillScreen(color);
			queueCommands.write(displayCommand);
		}

		/* override */ void drawRect(const Vec2& topLeftPix, const Vec2& sizePix, uint32_t color)
		{
			DisplayCommand displayCommand;
			displayCommand.drawRect(topLeftPix, sizePix, color);
			queueCommands.write(displayCommand);
		}

		/* override */ void fillRect(const Vec2& topLeftPix, const Vec2& sizePix, uint32_t color)
		{
			DisplayCommand displayCommand;
			displayCommand.fillRect(topLeftPix, sizePix, color);
			queueCommands.write(displayCommand);
		}

		/* override */ void drawRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t color)
		{
			DisplayCommand displayCommand;
			displayCommand.drawRoundRect(topLeftPix, sizePix, radius, color);
			queueCommands.write(displayCommand);
		}

		/* override */ void fillRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t color)
		{
			DisplayCommand displayCommand;
			displayCommand.fillRoundRect(topLeftPix, sizePix, radius, color);
			queueCommands.write(displayCommand);
		}

		/* override */ void fillSmoothRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t colFg, uint32_t colBg)
		{
			DisplayCommand displayCommand;
			displayCommand.fillSmoothRoundRect(topLeftPix, sizePix, radius, colFg, colBg);
			queueCommands.write(displayCommand);
		}

		/*override*/ void drawString(const char* str, const Vec2& bottomLeftPix, Alignment alignment, uint8_t font, uint8_t scale, uint32_t color)
		{
			DisplayCommand displayCommand;
			displayCommand.drawString(str, bottomLeftPix, alignment, font, scale, color);
			queueCommands.write(displayCommand);
		}

		/*override*/ void setPrintColor(void* pTask, uint32_t color)
		{
			Task* pTask2 = (Task*)pTask;
			PrintContext* printContext = queryPrintContext(pTask2);
			printContext->color = color;
		}

		/*override*/ void setPrintFont(void* pTask, uint8_t font)
		{
			Task* pTask2 = (Task*)pTask;
			PrintContext* printContext = queryPrintContext(pTask2);
			printContext->font = font;
		}

		/*override*/ void setPrintScale(void* pTask, uint8_t fontScale)
		{
			Task* pTask2 = (Task*)pTask;
			PrintContext* printContext = queryPrintContext(pTask2);
			printContext->fontScale = fontScale;
		}

		/*override*/ void setPrintCursor(void* pTask, const Vec2& cursorPos)
		{
			Task* pTask2 = (Task*)pTask;
			PrintContext* printContext = queryPrintContext(pTask2);
			printContext->cursorPos = cursorPos;
		}

		/*override*/ void setPrintWrap(void* pTask, bool bWrapX)
		{
			Task* pTask2 = (Task*)pTask;
			PrintContext* printContext = queryPrintContext(pTask2);
			printContext->bWrapX = bWrapX;
		}

		/*override*/ void print(void* pTask, const char* str)
		{
			Task* pTask2 = (Task*)pTask;
			DisplayCommand displayCommand;
			displayCommand.print(pTask2, str);
			queueCommands.write(displayCommand);
		}

		/*override*/ void println(void* pTask, const char* str)
		{
			Task* pTask2 = (Task*)pTask;
			DisplayCommand displayCommand;
			displayCommand.println(pTask2, str);
			queueCommands.write(displayCommand);
		}

	}; // end class AsyncDisplay
};// end namespace crt
