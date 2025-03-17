#pragma once
#include <assert.h>
#include <crt_Vec2.h>
namespace crt
{	
	class PrintContext
	{
	public:
		PrintContext() :
			pTask(nullptr), font(0), fontScale(1), color(0), cursorPos(0, 0), bWrapX(false)
		{
		}

	public:
		const Task* pTask;
		uint8_t font;
		uint8_t fontScale;
		uint32_t color;
		Vec2 cursorPos;
		bool bWrapX;
	};
};
