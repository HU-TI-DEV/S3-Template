#pragma once
#include "crt_ITouchListener.h"

namespace crt
{
	class ITouchListener;

	class ITouchDetector
	{
	public:
		virtual void addTouchListener(ITouchListener* pTouchListener) = 0;
		virtual bool isTouched() = 0;
	};
};