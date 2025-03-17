// ==========================================================================
//
// File      : hwlib-due.hpp
// Part of   : C++ hwlib library for close-to-the-hardware OO programming
// Copyright : wouter@voti.nl 2017-2019
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// ==========================================================================

// this file contains Doxygen lines
/// @file

#ifndef HWLIB_DUE_H
#define HWLIB_DUE_H

#include HWLIB_INCLUDE( ../hwlib-all.hpp )
#include <Arduino.h>
#include <CleanRTOS.h>

namespace hwlib_esp32 
{

class pin_in_out : public hwlib::pin_in_out {
private:
    uint8_t pin;

public:
    pin_in_out(uint8_t pin) : pin(pin) {}

    void direction_set_input() override {
        pinMode(pin, INPUT);
    }

    void direction_set_output() override {
        pinMode(pin, OUTPUT);
    }

    bool read() override {
        return digitalRead(pin);
    }

    void refresh() override {
        // In de context van de Arduino is er geen buffer die vernieuwd moet worden.
    }

    void write(bool x) override {
        digitalWrite(pin, x ? HIGH : LOW);
    }

    void flush() override {
        // In de context van de Arduino is er geen buffer die doorgespoeld moet worden.
    }

    void direction_flush() override {
        // De pinMode functie in de Arduino API wijzigt de richting onmiddellijk, dus hier is niets te doen.
    }
};

class pin_in : public hwlib::pin_in {
private:
    uint8_t pin;

public:
    pin_in(uint8_t pin) : pin(pin) {
        pinMode(pin, INPUT);
    }

    bool read() override {
        return digitalRead(pin);
    }

    void refresh() override {
        // In de context van de Arduino is er geen buffer die vernieuwd moet worden.
    }
};

class pin_out : public hwlib::pin_out {
private:
    uint8_t pin;

public:
    pin_out(uint8_t pin) : pin(pin) {
        pinMode(pin, OUTPUT);
    }

    void write(bool x) override {
        digitalWrite(pin, x ? HIGH : LOW);
    }

    void flush() override {
        // In de context van de Arduino is er geen buffer die doorgespoeld moet worden.
    }
};

class pin_oc : public hwlib::pin_oc {
private:
    uint8_t pin;

public:
    pin_oc(uint8_t pin) : pin(pin) {}

    bool read() override {
        pinMode(pin, INPUT);  // Zet in hoge impedantie modus
        return digitalRead(pin);
    }

    void write(bool x) override {
        if (x) {
            pinMode(pin, INPUT);  // Zet in hoge impedantie modus
        } else {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);  // Zet naar laag
        }
    }

    void refresh() override {
        // In de context van de Arduino is er geen buffer die vernieuwd moet worden.
    }

    void flush() override {
        // In de context van de Arduino is er geen buffer die doorgespoeld moet worden.
    }
};


/// the number of ticks per us
uint_fast64_t HWLIB_WEAK ticks_per_us(){
   return 240;
}

/// returns the number of ticks since some fixed starting point
uint_fast64_t HWLIB_WEAK now_ticks(){
      TickType_t ticks = xTaskGetTickCount();
      return (uint_fast64_t)ticks;
   }
   
}; // namespace hwlib_esp32

namespace hwlib {

namespace target = ::hwlib_esp32;

void wait_ns( int_fast32_t n ) {
    // Aanname: 1 luscyclus duurt ongeveer 5 ns (dit is een ruwe schatting en hangt af van de CPU-frequentie)
    volatile int loops = n / 5;
    while(loops--) {
        asm volatile ("nop");
    }
}

void wait_us( int_fast32_t n )
{
   assert(0); // don't call this function. use a cleanRTOS Timer associated with a crt::Task instead.
}

void wait_ms( int_fast32_t n )
{
   vTaskDelay(n); // Whether vTaskDelay actually delays a multiple of milliseconds depends
                  // on the ESP32 configuration.
}
} // end namespace hwlib

#endif // #ifdef HWLIB_ARDUINMO_DUE_H
