// Important: Update the CMakeLists file in the same folder as this main.cpp file
// (the folder "main", that is) to make sure that every dependency can be built.

#include "Button.h"
#include "KlikAanKlikUit.h"

const uint8_t PIN_BTN =  0; // ESP32: BOOT Button
const uint8_t PIN_LED =  2; // ESP32: Bult-in LED

extern "C"
{
	void app_main();
}

void app_main(void)
{
	crt::Button btnInput("InputButton", PIN_BTN, false);
	crt::KlikAanKlikUit("KlikAanKlikUit", 2 /*priority*/, 0 /*RUNNING_CORE*/, PIN_LED, (crt::Button*)&btnInput);

	while(true){
		// do nothing while the task is running
		vTaskDelay(100);
	}
}
