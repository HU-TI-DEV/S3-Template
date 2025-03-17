// by Marius Versteegen, 2023

#include "driver/ledc.h"
#include "driver/periph_ctrl.h"

namespace crt
{
    class ClockPin
    {
    private:
        ledc_mode_t    ledcMode = LEDC_LOW_SPEED_MODE;
        ledc_channel_t ledcChannel = LEDC_CHANNEL_0;
        ledc_timer_t   ledcTimer = LEDC_TIMER_0;
        int32_t        gpio;
        uint32_t       freqHz;

    public:
        // If bInvertOutput is true, then the output signal is high when the clockpin is "disabled".
        ClockPin(int32_t gpio, uint32_t freqHz, bool bInvertGPIO) :gpio(gpio), freqHz(freqHz)
        {
            periph_module_enable(PERIPH_LEDC_MODULE);

            // Set up timer
            ledc_timer_config_t ledc_timer = {
               .speed_mode = ledcMode,
               .duty_resolution = LEDC_TIMER_1_BIT,     // We need a clock, not PWM so 1 bit is enough.
               .timer_num = ledcTimer,
               .freq_hz = freqHz,                // Clock frequency, 38kHz
               .clk_cfg = LEDC_AUTO_CLK
            };
            ledc_timer_config(&ledc_timer);

            // Set up GPIO PIN
            ledc_channel_config_t channel_config = {
              .gpio_num = gpio,        // GPIO pin
              .speed_mode = ledcMode,
              .channel = ledcChannel,
              .intr_type = LEDC_INTR_DISABLE,
              .timer_sel = ledcTimer,
              .duty = 1,
              .hpoint = 0,
              .flags = {.output_invert = (unsigned int)(bInvertGPIO ? 1 : 0)}
            };
            ledc_channel_config(&channel_config);

            ledc_timer_pause(ledcMode, ledcTimer); // pause initially
        }

        void enable()
        {
            ledc_timer_resume(ledcMode, ledcTimer);
            ledc_timer_rst(ledcMode, ledcTimer);
        }

        void disable()
        {
            ledc_timer_pause(ledcMode, ledcTimer);
        }
    };
}; // end namespace crt