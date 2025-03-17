#pragma once
#include "crt_typesAndConstants.h"

namespace crt
{
	class ITouchListener
	{
	public:
		virtual void touchPressed(const Vec2& pos) = 0;
		virtual void touchReleased(const Vec2& pos) = 0;
	};
};