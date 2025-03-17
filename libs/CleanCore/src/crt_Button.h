// by Marius Versteegen, 2023

#pragma once
#include "crt_CleanRTOS.h"
#include "crt_IHandlerListener.h"
#include "crt_IHandler.h"
#include "crt_IButton.h"
#include "crt_ILogger.h"
// crt::Button

namespace crt
{
	//extern ILogger& logger;

	// Don't select the MAXBUTTONLISTENERCOUNT too high, or you may suffer from stack overflow troubles.
	template<unsigned int MAXBUTTONLISTENERCOUNT> class Button : public IButton, public IHandlerListener
	{
	private:
		const char* buttonName; // The name of the button (for debugging) or any other associated text.
		uint8_t pinButton;
		bool bPositiveLogic;	// If true, 1 means pressed, else 0 means pressed.
		bool bPrevPressed;
		const char* str;

		IButtonListener* arButtonListeners[MAXBUTTONLISTENERCOUNT] = {};
		uint16_t nofButtonListeners;

	public:
		Button(const char *buttonName, uint8_t pinButton, bool bPositiveLogic, IHandler& buttonHandler) :
			buttonName(buttonName), pinButton(pinButton), bPositiveLogic(bPositiveLogic), bPrevPressed(false),
			str(""), nofButtonListeners(0)
		{
			for (int i = 0;i < MAXBUTTONLISTENERCOUNT;i++)
			{
				arButtonListeners[i] = nullptr;
			}
			//memset(arButtonListeners, 0, sizeof(IButtonListener*) * MAXBUTTONLISTENERCOUNT);
			buttonHandler.addHandlerListener(this);

			//pinMode(pinButtonDump, INPUT);
			//PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pinButton], PIN_FUNC_GPIO);
			esp_rom_gpio_pad_select_gpio(pinButton);
			gpio_set_direction((gpio_num_t)pinButton, GPIO_MODE_INPUT);
		}

		/*override keyword not supported in current compiler*/
		void addButtonListener(IButtonListener* pButtonListener)
		{
			if (!isAlreadyPresent(pButtonListener))
			{
				assert(nofButtonListeners < MAXBUTTONLISTENERCOUNT);
				arButtonListeners[nofButtonListeners++] = pButtonListener;
			}
		}

		/*override*/
		bool isPressed()
		{
			return bPrevPressed;
		}

		/*override*/ void setButtonName(const char* str)
		{
			buttonName = str;
		}

		/*override*/ const char* getButtonName()
		{
			return buttonName;
		}

		// It is assumed that this update function is called by a ButtonHandler once in about 70ms: slow enough for debounce, fast enough for response.
		/*override keyword not supported in current compiler*/
		void update()
		{
			//logger.logText("button update called for button:");
			//logger.logText(buttonName);

			//bool buttonRead = digitalRead(pinButton);
			bool buttonRead = (gpio_get_level((gpio_num_t)pinButton) != 0);

			bool bPressed = (buttonRead == bPositiveLogic); // TODO: don't use digitalRead but use ESP specific code instead (to allow build with ESP_IDF without Arduino IDE).
			if (bPressed && !bPrevPressed)
			{
				for (int i = 0; i < nofButtonListeners; i++)
				{
					arButtonListeners[i]->buttonPressed(this);
				}
			}
			else if (!bPressed && bPrevPressed)
			{
				for (int i = 0; i < nofButtonListeners; i++)
				{
					arButtonListeners[i]->buttonReleased(this);
				}
			}
			bPrevPressed = bPressed;
		}

	private:
		bool isAlreadyPresent(IButtonListener* pButtonListener)
		{
			for (int i = 0; i < nofButtonListeners; i++)
			{
				//ESP_LOGI("adasdffasfsadf", "%s", i);
				if (arButtonListeners[i] == pButtonListener)
				{
					return true;
				}
			}
			return false;
		}
	}; // end class Button
}; // end namespace CleanRTOS
