// by Marius Versteegen, 2023

// Demonstration of how to switch between building a program from 
// ESP_IDF and Arduino_IDE

// This main.cpp file wraps an Arduino IDE .ino file, such that it can be
// built by the ESP_IDF.

// Important: Update the CMakeLists file in the same folder as this main.cpp file
// (the folder "main", that is) to make sure that every dependency can be built. 
// For convenience, I have stored the CMakeLists file in the extra folder, such that
// it can be copied (from).

#include <Arduino.h>

// Selecteer onderstaand de .ino van je applicatie
// **** CleanRTOS Core Tests ****
//#include <HelloWorld.ino>    					// For initial test.
//#include <TwoTasks.ino>
//#include <TenTasks.ino>						// Tip: Vergelijk dit voorbeeld met het Handler.ino voorbeeld!
//#include <Flag.ino>
//#include <Queue.ino>
//#include <Timer.ino>							// 5.1 test ok op c6/zigbee
//#include <MutexSection.ino>
//#include <Pool.ino>
//#include <HasFired.ino>
//#include <AllWaitables.ino>					// 5.1 test ok op c6/zigbee

// **** CleanRTOS Tools Tests ****
//#include <Logger.ino>
//#include <Handler.ino>
//#include <ClockPin.ino>						// 5.1 test ok op c6/zigbee

// **** .CleanGUI related Tests ****
//#include <Vec2.ino>
//#include <Panel.ino>
//#include <Button_demo.ino>
//#include <AsyncDisplay.ino>
//#include <Free_Font_Demo.ino>
//#include "TestButton.ino"
//#include <TouchscreenButton.ino>
//#include <TouchscreenButtonGroup.ino>
//#include <TouchscreenKeyboardLowerCase.ino>  // Not finalised yet.

// **** Test of thirdparty libs CleanGUI depends on ****
//#include <LITTLEFS_test.ino>
//void touch_calibrate();

// **** Arduino IDE Examples ****
#include <WifiScan.ino>
//#include <WiFiAccessPoint.ino>
//#include <AnalogRead.ino>

// other
//#include <TouchscreenButton.ino>
//#include <TouchscreenButtonGroup.ino>
//#include <TouchscreenKeyboardLowerCase.ino>  // Not finalised yet.
//#include <Queue2.ino>                        // Not sure if this is already finalised.
//#include <oled_example.ino> // van jan
//#include <ssd1306_128x64_i2c.ino>

#include "nvs_flash.h"

//------------------------------------
// Above, you can copy or include the contents of .ino examples from the arduino IDE.
// The only thing is: you may have to forward declare functions or change the order
// of functions, such that they're known before being called.
// (apparrently the esp IDF compiler is a bit less smart in that respect than the
// compiler used in the Arduino IDE)

extern "C" {
	void app_main();
}

void app_main(void)
{
	// Initialisatie van NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());  // Wis de NVS-partitie en probeer opnieuw
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

	setup();
	for(;;)
	{
		loop();
		yield();
		//vTaskDelay(1);  // prevent the watchdog timer to kick in for this thread.
	}
}

//# google:
//#how to add littlefs component to esp32 build ?