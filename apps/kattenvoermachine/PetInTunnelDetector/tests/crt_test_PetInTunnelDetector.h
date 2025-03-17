#pragma once
#include "..\..\..\..\main\crt\crt_CleanRTOS.h"
#include "..\crt__io_PetInTunnelDetector.h"
#include "..\crt__io_IPetInTunnelDetector.h"
#include "..\crt__io_IPetInTunnelDetectorListener.h"
#include "..\..\..\..\main\libs\RfidIso11784_85\crt__io_RfidIso11784_85.h"

namespace crt
{
	extern crt::ILogger& logger;

	class TestPetInTunnelDetector : public Task, public IPetInTunnelDetectorListener
	{
	private:
		// static, or we'd need to initialize in the initializer list.
		static const unsigned int MaxNofListenersPerPetInTunnelDetector = 1;
		static const unsigned int MaxNofListenersPerRfidIso11784_85 = 1;

		static const uint8_t pinCoilSelector = 27;
		
		static const uint8_t pinSerial2Rx = 16;
		static const uint8_t pinRfidReset = 26;

		RfidIso11784_85<MaxNofListenersPerRfidIso11784_85> rfidIso11784_85;
		PetInTunnelDetector<MaxNofListenersPerPetInTunnelDetector> petInTunnelDetector;

		static const unsigned int pinPirTunnel0 = 34;
		static const unsigned int pinPirTunnel1 = 35;
			
	public:
		static void StaticMain(void *pParam)
		{
			TestPetInTunnelDetector* THIS = (TestPetInTunnelDetector*)pParam;
			THIS->main();
		}

	public:
		TestPetInTunnelDetector(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, 10000 + sizeof(PetInTunnelDetector<MaxNofListenersPerPetInTunnelDetector>) /*taskSizeBytes*/, taskCoreNumber),
			rfidIso11784_85("RfidIso11784_85", taskPriority, taskCoreNumber, pinSerial2Rx, pinRfidReset),
			petInTunnelDetector("PetInTunnelDetector", taskPriority, taskCoreNumber, 
								 rfidIso11784_85, pinCoilSelector, pinPirTunnel0, pinPirTunnel1)
		{
			logger.logText("Bytes for PetInTunnelDetectors");
			logger.logInt32(sizeof(PetInTunnelDetector<MaxNofListenersPerPetInTunnelDetector>));

			petInTunnelDetector.addPetInTunnelDetectorListener(this);
		}

		/*override keyword not supported in current compiler*/
		void setup()
		{
			rfidIso11784_85.setup();	
			petInTunnelDetector.setup();
			start();
		}

		/*override*/
		void reportPetInTunnelDetection(uint8_t tunnelIndex, uint64_t rfid)
		{
			ESP_LOGI("Rfid_Iso", "In tunnel: %d, Pet ID reported: %d", (uint32_t)tunnelIndex, (uint32_t)rfid);
			
			logger.logText("Test PetInTunnelDetector_Report");

			//logger.logText("Pet ID reported:");
			//logger.logUint32((uint32_t)rfid);
		}

	private:
		void main()
		{
			//PetInTunnelDetectorHandler.start();
			//petInTunnelDetector.setup();

			vTaskDelay(10);// allow others to initialize.

			while (true)
			{
				// Actually, this task does nothing.
				// Instead of setting flags upon receiving PetInTunnelDetector presses, it prints them using
				// ESP_LOGI right away. (note: set logging level to info at least to make it work)

#ifdef CRT_HIGH_WATERMARK_INCREASE_LOGGING
				auto temp = uxTaskGetStackHighWaterMark(nullptr);
				if (!prev_stack_hwm || temp < prev_stack_hwm)
				{
					prev_stack_hwm = temp;
					ESP_LOGI("hwm_check", "Task %s has left: %d stack bytes and %d heap bytes", taskName, prev_stack_hwm, unsigned(xPortGetFreeHeapSize()));
					//ESP_LOGI(taskName,"%d",unsigned(xPortGetFreeHeapSize()));vTaskDelay(1);
					//ESP_LOGI(taskName," heap bytes","");vTaskDelay(1);
				}
#endif
				vTaskDelay(1);	// not important: this test "Task" does nothing itself..
			}
		}
	}; // end class Logger
}; // end namespace CleanRTOS