// by Marius Versteegen, 2023
//
// SharedOutputPin is actually a combination of an OR operation and a pin.
// If one or more of the registered users "uses" the pin, the pin is set to (remain-) logic 1.
// Otherwise, the pin is set to logic zero. It is also possible to invert the logic by setting
// positiveLogic to false.
//
// Example of use: There are multiple servo-valves. To avoid vibration-noise and power consumption,
// the servos are normally cut off from supply using a mosfet connected to a pin.
// Thus if a thread likes to temporarily move a servo, all it needs to do is to "use" the pin
// that powers the servo and "unuse" it afterwards.

#pragma once
#include "crt_ISharedOutputPinUser.h"
#include "crt_ISharedOutputPin.h"
#include "crt_FromArduinoIDE.h"
#include "crt_ILogger.h"
namespace ard = crt::FromArduinoIDE;

namespace crt
{
	//extern crt::ILogger& logger;

	class ISharedOutputPinUser;

	template<unsigned int MAXSHAREDOUTPUTPINUSERCOUNT> class SharedOutputPin : public ISharedOutputPin
	{
	private:
		ISharedOutputPinUser* arISharedOutputPinUser[MAXSHAREDOUTPUTPINUSERCOUNT] = {};
		bool arPinUserIsUsing[MAXSHAREDOUTPUTPINUSERCOUNT] = {};
		uint16_t nofSharedOutputPinUsers;

		uint8_t pinGpio;
		bool positiveLogic;
		bool currentlyUsed;

	public:
		SharedOutputPin(uint8_t pinGpio, bool positiveLogic)
			:nofSharedOutputPinUsers(0), pinGpio(pinGpio), positiveLogic(positiveLogic), 
			currentlyUsed(false)
		{
			for (int i = 0;i < MAXSHAREDOUTPUTPINUSERCOUNT;i++)
			{
				arISharedOutputPinUser[i] = nullptr;
				arPinUserIsUsing[i] = false;
			}

			ard::pinMode((int)pinGpio, gpio_mode_t::GPIO_MODE_OUTPUT);
			syncPinOutputToCurrentUsage();
		}

		/*override*/
		void addUser(ISharedOutputPinUser* sharedOutputPinUser)
		{
			if (!isAlreadyPresent(sharedOutputPinUser))
			{
				assert(nofSharedOutputPinUsers < MAXSHAREDOUTPUTPINUSERCOUNT);
				arISharedOutputPinUser[nofSharedOutputPinUsers++] = sharedOutputPinUser;
			}
		}

		/*override*/
		void usePin(ISharedOutputPinUser* pSharedOutputPinUser)
		{
			for (int i = 0; i < nofSharedOutputPinUsers; i++)
			{
				if (arISharedOutputPinUser[i] == pSharedOutputPinUser)
				{
					arPinUserIsUsing[i] = true;
					currentlyUsed = true;
					syncPinOutputToCurrentUsage();
					break;
				}
			}
		}

		/*override*/
		void unUsePin(ISharedOutputPinUser* pSharedOutputPinUser)
		{
			for (int i = 0; i < nofSharedOutputPinUsers; i++)
			{
				if (arISharedOutputPinUser[i] == pSharedOutputPinUser)
				{
					arPinUserIsUsing[i] = false;
					RecalcCurrentUsage();
					syncPinOutputToCurrentUsage();
					break;
				}
			}
		}

	private:
		void syncPinOutputToCurrentUsage()
		{
			bool bValue = currentlyUsed; // sync pin to current state of use.

			if (!positiveLogic) {
				bValue = !bValue;
			}

			int value = bValue ? ard::high : ard::Low;
			ard::digitalWrite((int)pinGpio, value);
		}

		void RecalcCurrentUsage()
		{
			bool usage = false;
			for (int i = 0; i < nofSharedOutputPinUsers; i++)
			{
				//ESP_LOGI("adasdffasfsadf", "%s", i);
				if (arPinUserIsUsing[i])
				{
					usage = true;
					break;
				}
			}
			currentlyUsed = usage;
		}

		bool isAlreadyPresent(ISharedOutputPinUser* pSharedOutputPinUser)
		{
			for (int i = 0; i < nofSharedOutputPinUsers; i++)
			{
				//ESP_LOGI("adasdffasfsadf", "%s", i);
				if (arISharedOutputPinUser[i] == pSharedOutputPinUser)
				{
					return true;
				}
			}
			return false;
		}
	}; // end class
}; // end namespace CleanRTOS