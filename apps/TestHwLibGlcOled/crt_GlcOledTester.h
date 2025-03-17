// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>

#include "hwlib.hpp"

// This file contains the code of multiple tasks that run concurrently and notify eachother using flags.

namespace crt
{
	extern ILogger& logger;
  
	class GlcOledTester : public Task
	{

	public:
		GlcOledTester(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber)
		{
            logger.logText("Init OledTester");
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			namespace target = hwlib::target;

			auto scl = target::pin_oc(26); 	// arbitrary selection of gpio pins for bit banged I2c.
			auto sda = target::pin_oc(27);
			auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
			//auto i2c_bus = due::i2c_hardware();

			// use the buffered version
			auto oled = hwlib::glcd_oled(i2c_bus, 0x3c);

			logger.logText("oled initilization done");

			// this won't work because it doesn't call flush()
			// hwlib::graphics_random_circles( oled );

			auto w1 = hwlib::part(
				oled,
				hwlib::xy(0, 0),
				hwlib::xy(128, 32));
			auto w2 = hwlib::part(
				oled,
				hwlib::xy(0, 32),
				hwlib::xy(128, 32));

			auto f1 = hwlib::font_default_16x16();
			auto d1 = hwlib::terminal_from(w1, f1);

			auto f2 = hwlib::font_default_8x8();
			auto d2 = hwlib::terminal_from(w2, f2);

			logger.logText("starting shooter cout");

			d1 << "\f"
			<< "LASER \nshooter";
			d2 << "\f"
			<< "==============\n"
			<< "user: Wouter\n"
			<< "score: " << 42 << "\n"
			<< "==============\n";

			logger.logText("starting oled flush");
			auto t1 = esp_timer_get_time();
			oled.flush();
			auto t2 = esp_timer_get_time();

			logger.logText("oled.fluhs() took in us:");
			logger.logInt32((int32_t)(t2-t1));

			logger.logText("oled flush done");
			
			ESP_LOGI("OledTester", "Started");vTaskDelay(200);

			int n = 0;

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				logger.logText("starting number");
				// hwlib::wait_ms(1000);
				hwlib::wait_us(1); // Let's make this task demanding.
				d1 << "\f" << ++n;
				d2 << "\t0502"
				<< ": " << n + 42;

				logger.logText("flushing number");
				// uint64_t tus = hwlib::now_us();
				oled.flush();
				// uint32_t dif = uint32_t(hwlib::now_us() - tus);
				//  logger.logUint32(dif);
			}
		}
	}; // end class OledTester

};// end namespace crt
