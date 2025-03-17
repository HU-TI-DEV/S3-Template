#pragma once
#include "crt_IKeyboard.h"

namespace crt
{
	// Note: Forward referencing with IButton because of mutual dependency could have 
	// been avoided if not IButton* but an enum value would be passed to identify the button. 
	// But I like to avoid the hassle of bookkeeping enums.
	// Mutual dependency of pure abstract interfaces is perfectly sound.
	class IKeyboard;

	class IKeyboardListener
	{
	public:
		virtual void keyPressed(const char* strKey) = 0;
		virtual void keyReleased(const char* strKey) = 0;
	};
};