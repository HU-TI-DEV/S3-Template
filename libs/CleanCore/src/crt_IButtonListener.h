#pragma once
#include "crt_IButton.h"

namespace crt
{
	// Note: Forward referencing with IButton because of mutual dependency could have 
	// been avoided if not IButton* but an enum value would be passed to identify the button. 
	// But I like to avoid the hassle of bookkeeping enums.
	// Mutual dependency of pure abstract interfaces is perfectly sound.
	class IButton;

	class IButtonListener
	{
	public:
		virtual void buttonPressed(IButton* pButton)=0;
		virtual void buttonReleased(IButton* pButton) = 0;
	};
};