#pragma once
#include "crt_typesAndConstants.h"
#include "crt_Vec2.h"
#include "crt_PrintContext.h"
#include "crt_IDisplay.h"
#include <CleanRTOS.h>

namespace crt
{
	enum class DisplayCommandType:int 
	{None=0, fillScreen=1, drawRect=2, fillRect=3, drawRoundRect=4, 
	 fillRoundRect=5, fillSmoothRoundRect = 6, drawString=7, setPrintColor=8, setPrintFont=9,
		setPrintScale=10, setPrintCursor=11, setPrintWrap=12, print=13, println=14
	};

	class DisplayCommand : public IDisplayDraw, public IDisplayPrint
	{
		//static constexpr char nullChar = '\0';

	private:
		DisplayCommandType command;
		Vec2 vecA;
		Vec2 vecB;
		uint32_t uintA;
		uint32_t uintB;
		int32_t intA;
		int32_t intB;
		const char nullChar = '\0';  // Dummy, could have been static, but then 
		                          // cpp file needed for its implementation.
		const char* str;	// NOTE: the string itself is NOT cached in this
							// DisplayCommand object (for economy).
							// Consequence: you should guarantee the lifetime
		                    // of the associated allocated memory until the 
							// command has finished!
							// For instance, by using fixed-size, statically
							// allocated strings, you can guarantee that by 
							// definition.
		const Task* pTask;

	private:
		void RestorePrintContext(IDisplay& display, const PrintContext& printContext)
		{
			display.setPrintColor(nullptr,printContext.color); // As these are synchronous calls, no need to bother to pass along the pTask involved.
			display.setPrintFont(nullptr, printContext.font);
			display.setPrintScale(nullptr, printContext.fontScale);
			display.setPrintCursor(nullptr, printContext.cursorPos);
			display.setPrintWrap(nullptr, printContext.bWrapX);
		}

		void UpdatePrintContext(IDisplay& display, PrintContext& printContext)
		{
			printContext.cursorPos = display.getPrintCursor();
		}

	public:
		DisplayCommand():command(DisplayCommandType::None),str(&nullChar), pTask(nullptr)
		{
		}

		DisplayCommand(const DisplayCommand& other) :
			command(other.command),
			vecA(other.vecA),
			vecB(other.vecB),
			uintA(other.uintA),
			uintB(other.uintB),
			intA(other.intA),
			str(other.str),
			pTask(other.pTask)
		{
		}

		DisplayCommand& operator= (const DisplayCommand& other)
		{
			command = other.command;
			vecA = other.vecA;
			vecB = other.vecB;
			uintA = other.uintA;
			uintB = other.uintB;
			intA = other.intA;
			this->str = other.str;
			this->pTask = other.pTask;

			return *this;
		}

		const Task* getTask()
		{
			return pTask;
		}

		void execute(IDisplay& display, PrintContext& printContext)
		{
			switch (command)
			{
			case DisplayCommandType::None:
				break;
			case DisplayCommandType::fillScreen:
				display.fillScreen(uintA);
				break;
			case DisplayCommandType::drawRect:
				display.drawRect(vecA, vecB, uintA);
				break;
			case DisplayCommandType::fillRect:
				display.fillRect(vecA, vecB, uintA);
				break;
			case DisplayCommandType::drawRoundRect:
				display.drawRoundRect(vecA, vecB, intA, uintA);
				break;
			case DisplayCommandType::fillRoundRect:
				display.fillRoundRect(vecA, vecB, intA, uintA);
				break;
			case DisplayCommandType::fillSmoothRoundRect:
				display.fillSmoothRoundRect(vecA, vecB, intA, uintA, uintB);
				break;
			case DisplayCommandType::drawString:
				display.drawString(str, vecA, (Alignment)intB, (uint8_t)uintA, (uint8_t)intA, (uint32_t)uintB);
				break;
			case DisplayCommandType::setPrintColor:
				printContext.color = uintA;
				break;
			case DisplayCommandType::setPrintFont:
				printContext.font = uintA;
				break;
			case DisplayCommandType::setPrintScale:
				printContext.fontScale = uintA;
				break;
			case DisplayCommandType::setPrintCursor:
				printContext.cursorPos = vecA;
				break;
			case DisplayCommandType::setPrintWrap:
				printContext.bWrapX = (uintA!=0);
				break;
			case DisplayCommandType::print:
				RestorePrintContext(display, printContext);
				display.print(nullptr, str);   // can be nullptr: display.print is called asynchronously here.
				UpdatePrintContext(display, printContext);
				break;
			case DisplayCommandType::println:
				RestorePrintContext(display, printContext);
				display.println(nullptr, str);   // can be nullptr: display.print is called asynchronously here.
				UpdatePrintContext(display, printContext);
				break;
			default:
				ESP_LOGI("Unsupported DisplayCommand", "%d", int(command));
			}
		}

		// for debugging purposes
		void dumpWithESP_LOGI()
		{
			// obsolete, turns out ESP32-5.3 expects different format specifiers for int32_t and uint32_t:
			// PRId32 for int32_t and PRIu32 for uint32_t.
			// Still %d for "normal" int (which I think is 64 bits).
			// ESP_LOGI("DisplayCommand", "%d",int(command));
			// ESP_LOGI("vecA x", "%d", vecA.x);
			// ESP_LOGI("vecA y", "%d", vecA.y);
			// ESP_LOGI("vecB x", "%d", vecB.x);
			// ESP_LOGI("vecB y", "%d", vecB.y);
			// ESP_LOGI("uintA", "%d", uintA);
			// ESP_LOGI("uintB", "%d", uintB);
			// ESP_LOGI("intA", "%d", intA);
			ESP_LOGI("DisplayCommand", "%d",int(command));
			ESP_LOGI("vecA x", "%d", vecA.x);
			ESP_LOGI("vecA y", "%d", vecA.y);
			ESP_LOGI("vecB x", "%d", vecB.x);
			ESP_LOGI("vecB y", "%d", vecB.y);
			ESP_LOGI("uintA", "%" PRIu32, uintA);
			ESP_LOGI("uintB", "%" PRIu32, uintB);
			ESP_LOGI("intA", "%" PRId32, intA);
		}
		/* override */ void fillScreen(uint32_t color)
		{
			command = DisplayCommandType::fillScreen;
			uintA = color;
			this->str = &nullChar;
		}

		/* override */ void drawRect(const Vec2& topLeftPix, const Vec2& sizePix, uint32_t color)
		{
			command = DisplayCommandType::drawRect;
			vecA = topLeftPix;
			vecB = sizePix;
			uintA = color;
			this->str = &nullChar;
		}

		/* override */ void fillRect(const Vec2& topLeftPix, const Vec2& sizePix, uint32_t color)
		{
			command = DisplayCommandType::fillRect;
			vecA = topLeftPix;
			vecB = sizePix;
			uintA = color;
			this->str = &nullChar;
		}

		/* override */ void drawRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t color)
		{
			command = DisplayCommandType::drawRoundRect;
			vecA = topLeftPix;
			vecB = sizePix;
			intA = radius;
			uintA = color;
			this->str = &nullChar;
		}

		/* override */ void fillRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t color)
		{
			command = DisplayCommandType::fillRoundRect;
			vecA = topLeftPix;
			vecB = sizePix;
			intA = radius;
			uintA = color;
			this->str = &nullChar;
		}

		/* override */ void fillSmoothRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t colFg, uint32_t colBg)
		{
			command = DisplayCommandType::fillSmoothRoundRect;
			vecA = topLeftPix;
			vecB = sizePix;
			intA = radius;
			uintA = colFg;
			uintB = colBg;
			this->str = &nullChar;
		}

		/*override*/ void drawString(const char* str, const Vec2& bottomLeftPix, Alignment alignment, uint8_t font, uint8_t scale, uint32_t color)
		{
			command = DisplayCommandType::drawString;
			vecA = bottomLeftPix;
			uintA = (int32_t)font;
			intA = (int32_t)scale;
			uintB = (uint32_t)color;
			intB = (int32_t)alignment;
			this->str = str;
		}

		/*override*/ void setPrintColor(void* pTask, uint32_t color)
		{
			command = DisplayCommandType::setPrintColor;
			uintA = color;
			this->pTask = (Task*) pTask;
			this->str = &nullChar;
		}

		/*override*/ void setPrintFont(void* pTask, uint8_t font)
		{
			command = DisplayCommandType::setPrintFont;
			uintA = (uint32_t)font;
			this->pTask = (Task*)pTask;
			this->str = &nullChar;
		}

		/*override*/ void setPrintScale (void* pTask, uint8_t fontScale)
		{
			command = DisplayCommandType::setPrintScale;
			uintA = (uint32_t)fontScale;
			this->pTask = (Task*)pTask;
			this->str = &nullChar;
		}

		/*override*/ void setPrintCursor(void* pTask, const Vec2& cursorPos)
		{
			command = DisplayCommandType::setPrintCursor;
			vecA = cursorPos;
			this->pTask = (Task*)pTask;
			this->str = &nullChar;
		}

		/*override*/ void setPrintWrap(void* pTask, bool bWrapX)
		{
			command = DisplayCommandType::setPrintWrap;
			uintA = bWrapX ? 1 : 0;
			this->pTask = (Task*)pTask;
			this->str = &nullChar;
		}

		/*override*/ void print(void* pTask, const char* str)
		{
			command = DisplayCommandType::print;
			this->pTask = (Task*)pTask;
			this->str = str;
		}

		/*override*/ void println(void* pTask, const char* str)
		{
			command = DisplayCommandType::println;
			this->pTask = (Task*)pTask;
			this->str = str;
		}
	};
};
