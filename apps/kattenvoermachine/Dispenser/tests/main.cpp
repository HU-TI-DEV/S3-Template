//#include "Arduino.h" //Zet een evt Arduino.h include (als je die nodig hebt) altijd aan het begin!!
#include "crt/crt_CleanRTOS.h"

#include "crt/crt_Logger.h"
#include "crt/crt_Handler.h"

#include "../apps/kattenvoermachine/Dispenser/tests/crt_test_TestDispenser.h"

namespace crt
{
 	// Create a "global" logger object withing namespace crt.
	const unsigned int pinButtonDump = 23;
	
	Logger<100> theLogger("Logger", 2 /*priority*/, ARDUINO_RUNNING_CORE, pinButtonDump);
	ILogger& logger = theLogger;	// This is the global object. It can be accessed without knowledge of the template parameter of theLogger.

    MainInits mainInits;        // Allow creation of microsecond timers.

	TestDispenser testDispenser("TestDispenser", 2 /*priority*/, ARDUINO_RUNNING_CORE); // Don't forget to call its start() member during setup().
}
// ********  end of constant part of this .ino file  **************

void myMain()
{
	// initialize serial communication at 115200 bits per second:
	//Serial.begin(115200);

	vTaskDelay(10);// allow logger to initialize.
	ESP_LOGI("checkpoint", "start of main");vTaskDelay(1);;
	
	crt::testDispenser.setup();
	ESP_LOGI("checkpoint", "end of main");vTaskDelay(1);
	//crt::logger.dumpNow(); // Up till some time, somehow, these logs are lost.
}

// Use the define below to switch between Arduino IDE and ESP-IDF.
// In addition, switch the extension of this file between .cpp and .ino
//#define CRT_ARDUINO_IDE

#ifdef CRT_ARDUINO_IDE
void setup()
{
	myMain();
}
void loop()
{
}
#else
extern "C" {
	void app_main();
}
void app_main(void)
{
	myMain();
}
#endif
