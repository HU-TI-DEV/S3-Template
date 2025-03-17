
#include "crt/crt_CleanRTOS.h"        // This file includes crt_Config.h  You'll need to change defines there for a release build.
#include "crt/crt_Logger.h"

// *************** start of ino file *****************

#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
// Possible PWM GPIO pins on the ESP32-S2: 0(used by on-board button),1-17,18(used by on-board LED),19-21,26,33-42

int servoPin = 13;

void setup() {
	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);    // standard 50 hz servo
	myservo.attach(servoPin, 500, 2500); // attaches the servo on pin 18 to the servo object
	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings
	// for an accurate 0 to 180 sweep
}

void loop() {

	for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
	for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
}

// *************** end of ino file *****************

namespace crt
{
	portMUX_TYPE criticalSectionMutex = portMUX_INITIALIZER_UNLOCKED;	// The one and only mutex for critical sections in my software.

	MainInits mainInits;        // Allow creation of microsecond timers.

 	// Create a "global" logger object withing namespace crt.
	const unsigned int pinButtonDump = 23;
	
	Logger<100> theLogger("Logger", 2 /*priority*/, ARDUINO_RUNNING_CORE, pinButtonDump);
	ILogger& logger = theLogger;	// This is the global object. It can be accessed without knowledge of the template parameter of theLogger.
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

