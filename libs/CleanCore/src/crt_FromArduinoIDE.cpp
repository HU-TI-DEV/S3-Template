#include "crt_FreeRTOS.h" // for uint32_t etc.
#include "crt_ILogger.h"
#include "crt_FromArduinoIDE.h"
#include <esp_timer.h>
namespace crt
{
	extern ILogger& logger;

	/* Copied from gpio_types.h, for reference:
	typedef enum {
		GPIO_MODE_DISABLE = GPIO_MODE_DEF_DISABLE,                                                         < GPIO mode : disable input and output
		GPIO_MODE_INPUT = GPIO_MODE_DEF_INPUT,                                                             < GPIO mode : input only
		GPIO_MODE_OUTPUT = GPIO_MODE_DEF_OUTPUT,                                                           < GPIO mode : output only mode
		GPIO_MODE_OUTPUT_OD = ((GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)),                               < GPIO mode : output only with open-drain mode
		GPIO_MODE_INPUT_OUTPUT_OD = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)), < GPIO mode : output and input with open-drain mode
		GPIO_MODE_INPUT_OUTPUT = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT)),                         < GPIO mode : output and input mode
	} gpio_mode_t;
	*/

	// Instead of static functions as part of a class, I choose for a namespace.
	// Reason: it allows for shorthanding using namespace aliases.
	namespace FromArduinoIDE
	{
	//public:
		///*static */const int HIGH = 1;
		///*static */const int LOW = 0;

		/*static */void pinMode(int gpioPin, gpio_mode_t gpio_mode)
		{
			esp_rom_gpio_pad_select_gpio((gpio_num_t)gpioPin);
			gpio_set_direction((gpio_num_t)gpioPin, gpio_mode);
		}
		
		/*static */int digitalRead(int gpioPin)
		{
			return (gpio_get_level((gpio_num_t)gpioPin) != 0) ? 1 : 0;
		}

		/*static */void digitalWrite(int gpioPin, int level)
		{
			gpio_set_level((gpio_num_t)gpioPin, level);
		}

		// The conversion can be avoided by using next functions:
		/*static */int digitalRead(gpio_num_t gpioPin)
		{
			return (gpio_get_level(gpioPin) != 0) ? 1 : 0;
		}

		/*static */void digitalWrite(gpio_num_t gpioPin, int level)
		{
			gpio_set_level(gpioPin, level);
		}

		/*static */void delayMicroseconds(int us)
		{
			int64_t m = esp_timer_get_time();
			if (us > 0)
			{
				while ((esp_timer_get_time() - m) < (int64_t)us)
				{
					__asm__ __volatile__("nop");
				}
			}
		}

		// I choose to stick to 64 bit precision, to avoid the need of dealing with overflows.
		/*static */uint64_t micros()
		{
			return (uint64_t)esp_timer_get_time();
		}

		/*static */uint64_t millis()
		{
			return (uint64_t)(esp_timer_get_time() / 1000);
		}

		/*static */uint64_t seconds()
		{
			return (uint64_t)(esp_timer_get_time() / 1000000);
		}
	};
};