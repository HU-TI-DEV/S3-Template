#pragma once
#include <crt_ITouchListener.h>
#include <crt_CleanRTOS.h>

namespace crt
{
	// The interface below is part of the IDisplay interface.
	// Both the DisplayAdapter and AsyncDisplay (when including CleanGUI_async library) 
	// implement these functions in a synchronous manner.
	class IDisplaySynchrone
	{
	public:
		virtual Vec2	getScreenSize() = 0;
		virtual int16_t getTextWidth(const char* str, uint8_t font) = 0;
		virtual int16_t getFontHeight(uint8_t font) = 0;
		virtual Vec2    getPrintCursor() = 0;
	};

	// The interface below is part of the IDisplay interface.
	// DisplayAdapters typically implement its functions in a synchronous manner.
	// AsyncDisplay implements these functions in an asynchronous manner though.
	class IDisplayDraw
	{
	public:
		virtual void fillScreen(uint32_t color) = 0;
		virtual void drawRect(const Vec2& topLeftPix, const Vec2& sizePix, uint32_t color) = 0;
		virtual void fillRect(const Vec2& topLeftPix, const Vec2& sizePix, uint32_t color) = 0;
		virtual void drawRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t color) = 0;
		virtual void fillRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t color) = 0;
		virtual void fillSmoothRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t colFg, uint32_t colBg) = 0;
		virtual void drawString(const char* str, const Vec2& bottomLeftPix, Alignment alignment, uint8_t font, uint8_t fontScale, uint32_t color) = 0;
	};

	// The interface below is part of the IDisplay interface.
	// These functions define a "print context" within AsyncDisplay 
	// which is maintained for each task separately.
	// The print functions are executed in the context of that "print context".
	// The (synchronous) implementation of a DisplayAdapter of the functions below typically
	// ignores the pTask parameter (a nullptr can be passed then).
	// When using CleanRTOS, pass Task pointers instead to allow asynchronous 
	// multi - threaded access to the display.
	class IDisplayPrint
	{
	public:
		virtual void setPrintColor(void* pTask, uint32_t color) = 0;
		virtual void setPrintFont(void* pTask, uint8_t font) = 0;
		virtual void setPrintScale(void* pTask, uint8_t fontScale) = 0;
		virtual void setPrintCursor(void* pTask, const Vec2& cursorPos) = 0;
		virtual void setPrintWrap(void* pTask, bool bWrapX) = 0;
		virtual void print(void* pTask, const char* str) = 0;
		virtual void println(void* pTask, const char* str) = 0;
	};

	// The interface below is used for use with touch displays.
	// If your display does not support touch, you can just override the functions below
	// with empty stubs.
	class IDisplayTouch
	{
	public:
		virtual void pollTouch(void* pCallingTask) = 0;
		virtual void addTouchListener(ITouchListener* pTouchListener) = 0;
		virtual void touchCalibrate(uint8_t rotation, uint8_t font) = 0;
	};

	class IDisplay : public IDisplaySynchrone, public IDisplayDraw, public IDisplayPrint, public IDisplayTouch
	{
	};
};
