
#include "crt/crt_CleanRTOS.h"        // This file includes crt_Config.h  You'll need to change defines there for a release build.
#include "crt/crt_Logger.h"
#include "crt/crt__io_SharedOutputPin.h"
#include "libs/servo/src/ESP32Servo.h"
#include "../apps/kattenvoermachine/ServoSweeper/crt_ServoSweeper.h"

Servo servo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
// Possible PWM GPIO pins on the ESP32-S2: 0(used by on-board button),1-17,18(used by on-board LED),19-21,26,33-42

int servoPin = 13;
int servoPowerPin = 25;
const unsigned int MaxNofUsersPerSharedOutputPin = 5;
crt::SharedOutputPin<5> sharedOutputPin(servoPowerPin,true /*positiveLogic*/);
crt::ServoSweeper servoSweeper("ServoSweeper", 2 /*priority*/, ARDUINO_RUNNING_CORE, servo, sharedOutputPin); // Don't forget to call its start() member during setup().

void setup() {
	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	servo.setPeriodHertz(50);    // standard 50 hz servo
	servo.attach(servoPin, 500, 2500);

	servoSweeper.start();
}

void loop() {

	int nSweepTimeMs = 5000;

	// Queue two commands
	servoSweeper.addSweep(90, 175, nSweepTimeMs); // beginAngle, startAngle, sweepTimeMs
	servoSweeper.addSweep(175, 90, nSweepTimeMs);

	vTaskDelay(nSweepTimeMs * 2.5);
}

// *************** end of ino file *****************

namespace crt
{
	portMUX_TYPE criticalSectionMutex = portMUX_INITIALIZER_UNLOCKED;	// The one and only mutex for critical sections in my software.

 	// Create a "global" logger object withing namespace crt.
	const unsigned int pinButtonDump = 23;
	
	Logger<100> theLogger("Logger", 2 /*priority*/, ARDUINO_RUNNING_CORE, pinButtonDump);
	ILogger& logger = theLogger;	// This is the global object. It can be accessed without knowledge of the template parameter of theLogger.

    MainInits mainInits;        // Allow creation of microsecond timers.
}
// ********  end of constant part of this .ino file  **************

void myMain()
{
	setup();
	while (true)
	{
		loop();
		vTaskDelay(1);
	}
}

extern "C" {
	void app_main();
}
void app_main(void)
{
	myMain();
}

