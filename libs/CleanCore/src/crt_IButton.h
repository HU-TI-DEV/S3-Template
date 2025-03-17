#pragma once
#include <crt_IButtonListener.h>

namespace crt
{
	class IButtonListener;

	class IButton
	{
	public:
		virtual void addButtonListener(IButtonListener* pButtonListener) = 0;
		virtual bool isPressed() = 0;
		virtual void setButtonName(const char* str) = 0;
		virtual const char* getButtonName() = 0;
	};
};