// by Marius Versteegen, 2023

// MainInits initializes CleanRTOS.
// A MainInits object must be defined before the Tasks are started.

#pragma once
#include "internals/crt_FreeRTOS.h"
#include "esp_timer.h"

namespace crt
{
    class MainInits
    {
    private: static bool bInitialized; // initialized to false in crt_MainInits.cpp
    public:
        MainInits()
        {
            if(!bInitialized)
            {
                //No longer needed in ESP IDF 5.1 
                //esp_timer_init();   // Allow creation and use of microsecond timers.
            }
            bInitialized = true;
        }
    };
};
