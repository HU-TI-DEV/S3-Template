#pragma once

#include "crt_ILogger.h"
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
		const int high=1;
		const int low=0;

		extern void pinMode(int gpioPin, gpio_mode_t gpio_mode);
		extern int digitalRead(int gpioPin);
		extern void digitalWrite(int gpioPin, int level);
		extern int digitalRead(gpio_num_t gpioPin);
		extern void digitalWrite(gpio_num_t gpioPin, int level);
		extern void delayMicroseconds(int us);
		extern uint64_t micros();
		extern uint64_t millis();
		extern uint64_t seconds();
	};
};