#pragma once
#include <crt_CleanRTOS.h>
#include "Button.h"

// crt::KlikAanKlikUit
// by Hagen Patzke, 2025

namespace crt
{
	class KlikAanKlikUit : public Task{
		enum class State {BlueLedOn, BlueLedOff};
		
		private:
			uint8_t pinLED;
			State state = State::BlueLedOn;
			crt::Button *btnKlik;

		public:
			KlikAanKlikUit(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber, const uint8_t pinLED, crt::Button *btnKlik)
				: Task(taskName, taskPriority, 3000, taskCoreNumber), pinLED(pinLED), btnKlik(btnKlik)
			{
				// set up the LED pin
				esp_rom_gpio_pad_select_gpio(pinLED);
				gpio_reset_pin((gpio_num_t)pinLED);
				gpio_set_direction((gpio_num_t)pinLED, GPIO_MODE_OUTPUT);
				gpio_set_level((gpio_num_t)pinLED, 0);
				// our constructor also starts our task
				ESP_LOGI("KlikAanKlikUit", "start task");
				start();
			}

		private:
			void main()
			{
				// Built-In Self Test: flash LED three times
				for (int i = 1; i < 10; i++)
				{
					vTaskDelay(100);
					gpio_set_level((gpio_num_t)pinLED, (i & 1));
				}

				// main function
				while (true)
				{
					switch(state){
						case State::BlueLedOn:
							gpio_set_level((gpio_num_t)pinLED, 1);
							if (btnKlik->isPressed()) {
								ESP_LOGI("KlikAanKlikUit", "button pressed blue led off");
								state = State::BlueLedOff;
							}
							break;
						case State::BlueLedOff:
							gpio_set_level((gpio_num_t)pinLED, 0);
							if (btnKlik->isPressed()) {
								ESP_LOGI("KlikAanKlikUit", "button pressed blue led on");
								state = State::BlueLedOn;
							}
							break;
						}
					vTaskDelay(200);
				}
		}
	}; // end class KlikAanKlikUit
}; // end namespace CleanRTOS
