// by Marius Versteegen, 2023

#include <ClockPin.h>

crt::ClockPin clockPin(2 /*gpio*/, 38000 /*freqHz*/, false /*Invert GPIO*/);  // Define 38kHz clock for gpio pin 2.

void setup() {
}

void loop() {
  vTaskDelay(1);
  clockPin.enable();
  vTaskDelay(1);
  clockPin.disable();
}