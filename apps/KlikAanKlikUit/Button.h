#pragma once
#include <crt_CleanRTOS.h>

// crt::Button
// by Hagen Patzke, 2025

namespace crt
{
	class Button
	{
	private:
		const char *buttonName; // The name of the button (for debugging) or any other associated text.
		uint8_t pinButton;		// Pin number of the button
		bool bPositiveLogic;	// If true, 1 means pressed, else 0 means pressed.

	public:
		Button(const char *buttonName, uint8_t pinButton, bool bPositiveLogic = true)
			: buttonName(buttonName), pinButton(pinButton), bPositiveLogic(bPositiveLogic)
		{
			esp_rom_gpio_pad_select_gpio(pinButton);
			gpio_reset_pin((gpio_num_t)pinButton);
			gpio_set_direction((gpio_num_t)pinButton, GPIO_MODE_INPUT);
		}

		void setButtonName(const char *str)
		{
			buttonName = str;
		}

		const char *getButtonName()
		{
			return buttonName;
		}

		bool isPressed()
		{
			bool buttonRead = !!gpio_get_level((gpio_num_t)pinButton);
			bool bPressed = (buttonRead == bPositiveLogic);
			return bPressed;
		}
	}; // end class Button

}; // end namespace CleanRTOS
