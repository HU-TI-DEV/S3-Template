// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>
#include <crt_Button.h>

namespace crt
{
	class TestButtons : public IButtonListener
	{
	public:
		TestButtons(IButton& buttonA)
		{
			buttonA.addButtonListener(this);
		}

		void buttonPressed(IButton* pButton)
		{
			ESP_LOGI("Button Pressed","%s",pButton->getButtonName());
		}

		void buttonReleased(IButton* pButton)
		{
			ESP_LOGI("Button Released","%s",pButton->getButtonName());
		}
  
	}; // end class CounterForTestHandler
};// end namespace crt
