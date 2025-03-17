// by Marius Versteegen, 2022
// Content copied from example/TestWaitables/TestWaitables.ino
// Test interaction of some waitables
#include "../apps/kattenvoermachine/MonoLightEffect/crt_MonoLightEffect.h"
#include "crt/crt_CleanRTOS.h"
#include "crt/crt_Logger.h"

namespace crt
{
	Logger<100> theLogger("Logger", 2 /*priority*/, ARDUINO_RUNNING_CORE, 33 /*pinButtonDump*/);
	ILogger& logger = theLogger;	// Use "logger" as global logger object. It can be accessed from anywhere without knowledge of the template parameter of theLogger.

	MainInits mainInits;            // Allow creation of microsecond timers.

	const uint8_t pinPwmLedTunnel1 = 23;
	MonoLightEffect monoLightEffect("MonoLightEffect", 2 /*priority*/, ARDUINO_RUNNING_CORE, pinPwmLedTunnel1);
};

// Use the define below to switch between Arduino IDE and ESP-IDF.
// In addition, switch the extension of this file between .cpp and .ino
//#define CRT_ARDUINO_IDE

void myMain()
{
	// initialize serial communication at 115200 bits per second:
	//Serial.begin(115200);

	vTaskDelay(10);// allow logger to initialize.
	ESP_LOGI("checkpoint", "start of main");vTaskDelay(1);

	crt::monoLightEffect.setup();
	ESP_LOGI("checkpoint", "end of main");vTaskDelay(1);

	crt::monoLightEffect.startFlash(0,50,1000,500);
	vTaskDelay(3000);
	crt::monoLightEffect.startTriangle(0, 120, 1500, true /*bSquaredOutput*/);
	vTaskDelay(5000);
	crt::monoLightEffect.startFlash(0, 50, 1000, 500);
	vTaskDelay(3000);
	crt::monoLightEffect.startSineWave(0, 120, 1500, true /*bSquaredOutput*/);
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
