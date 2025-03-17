// Demonstration of how to switch between building a program from 
// ESP_IDF and Arduino_IDE

// This main.cpp file is a conversion from a file called HelloWorld.ino:
// The conversion was done as follows:
// 1 - rename HelloWorld.ino to main.cpp
// 2 - add #include <Arduino.h> to the top
// 3 - add the app_main function below the original code
// 4 - put main.cpp in the "main" folder of your ESP32 environment
// 5 - update the CMakeLists file in the same folder to make sure that
//     everything can be built. In this case, only the file main.cpp, that is.

#include <Arduino.h>

int counter = 0;

//------------------------------------------------------------------------------------------
void setup(void)
{
	// If you use Serial for communications with the ESP32,
	// it may be a good idea to select 115200. 
	// In that case, you can monitor both the Serial prints and
	// the native ESP32 debug output both at once.
	Serial.begin(115200);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void loop()
{
	Serial.println("hallo");
	printf("[%d] Hello world!\n", counter);
    counter++;
    vTaskDelay(3000);
}
//------------------------------------------------------------------------------------------

//------------------------------------
// Above, you can copy the contents of .ino examples from the arduino IDE.
// The only thing is: you may have to forward declare functions or change the order
// of functions, such that they're known before being called.
// (apparrently the esp IDF compiler is a bit less smart in that respect than the
// compiler used in the Arduino IDE)

extern "C" {
	void app_main();
}

void app_main(void)
{
	setup();
	for(;;)
	{
		loop();
		vTaskDelay(1);
	}
}

