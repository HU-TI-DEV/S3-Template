#include "Arduino.h" //Zet een evt Arduino.h include (als je die nodig hebt) altijd aan het begin!!
#include "crt/crt_CleanRTOS.h"        // This file includes crt_Config.h  You'll need to change defines there for a release build.

// All Tasks should be created in this main file.
// Handlers en Loggers are task objects, so let's include them here.
// In the rest of the code, we can reference (and link) to them via their interfaces ILogger and IHandler.
#include "crt/crt_Logger.h"

#include "../apps/kattenvoermachine/PetInTunnelDetector/tests/crt_test_PetInTunnelDetector.h"
//#include "crt_test_TestWaitables.h"
// NIET op deze plek: #include "Arduino.h" Zet een evt Arduino.h include altijd aan het BEGIN!! (anders verkeerde dependency volgorde-werkt het niet)
// Beter nog: voorkom afhankelijkheden naar Arduino libraries en gebruik alleen dedicated ESP libraries.
// Anders moet je een eigen Arduino.h schrijven met wrappers (van bijvoorbeeld digitalread en pinmode)
// om het nog te kunnen bouwen met ESP-IDF.
// Voordeel van bouwen met ESP-IDF is: het bouwt sneller en foutmeldingen zijn vaak
// nauwkeuriger.

// This Project contains the CleanRTOS library, associated ESP32 libraries and tests of both.
// CleanRTOS is a collection of wrappers around FreeRTOS.
// It lives in the namespace crt (abbreviation of CleanRTOS).
// It's purpose is to make use of FreeRTOS easier and safer.
//
// To help prevent potential headerfile name clashes, it's constituent files are prefixed with crt_
// The classnames themselves don't need the prefix because of membership of the namespace.
// A benefit of that choice is that you can use autocompletion after typing crt:: to find all CleanRTOS 
// parts.
//
// Next classes are available within crt:
//    Logger : a maximally fast logger, meant for debugging purposes.

// ********  start of constant part of this .ino file  **************
// With CleanRTOS, the main file is responsible for setting up global objects.
// A global object that is assumed in all projects is a logger:
namespace crt
{
	// Create a "global" logger object withing namespace crt.
	const unsigned int pinButtonDump = 33;

	Logger<100> theLogger("Logger", 2 /*priority*/, ARDUINO_RUNNING_CORE, pinButtonDump);
	ILogger& logger = theLogger;	// This is the global object. It can be accessed without knowledge of the template parameter of theLogger.

	MainInits mainInits;        // Allow creation of microsecond timers.

	 TestPetInTunnelDetector testPetInTunnelDetector("TestPetInTunnelDetector", 2 /*priority*/, ARDUINO_RUNNING_CORE); // Don't forget to call its start() member during setup().
}
// ********  end of constant part of this .ino file  **************

void myMain()
{
	// initialize serial communication at 115200 bits per second:
	//Serial.begin(115200);

	vTaskDelay(10);// allow logger to initialize.
	ESP_LOGI("checkpoint", "start of main");vTaskDelay(1);
	
	crt::testPetInTunnelDetector.setup();
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
