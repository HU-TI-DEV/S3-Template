#pragma once
#include "../../../main/crt/crt_CleanRTOS.h"
#include "../../../main/crt/crt__io_IButtonListener.h"
#include "crt__io_IPetInTunnelDetector.h"
#include "../../../main/libs/RfidIso11784_85/crt__io_IRfidDetector.h"
// crt::PetInTunnelDetector .. beter: PetInTunnelDetector.
// by Marius Versteegen, 2022

// Note:
// This class uses Serial2 - rx, which is gpio-16, to receive the data
// from the rfid board.
namespace crt
{
	extern crt::ILogger& logger;

	namespace ard = crt::FromArduinoIDE;

	struct PetDetectionStruct
	{
		uint8_t tunnelIndex;
		uint64_t rfId;

		PetDetectionStruct() :tunnelIndex(0), rfId(0) {}
		PetDetectionStruct(uint8_t tunnelIndex, uint64_t rfId) 
								:tunnelIndex(tunnelIndex), rfId(rfId) {}
		PetDetectionStruct(const PetDetectionStruct& cds)
		{
			tunnelIndex = cds.tunnelIndex;
			rfId = cds.rfId;
		}
	};

	// Don't select the MAXRCATIDENTIFIERLISTENERCOUNT too high, or you may suffer from stack overflow troubles.
	template<unsigned int MAXRCATIDENTIFIERLISTENERCOUNT> class PetInTunnelDetector : public Task, public IRfidDetectorListener
	{
	private:
		enum class PetInTunnelDetectorState :uint8_t {Pause, EnablingRfid, ListeningToRfid };

	private:
		uint8_t pinRfidCoilSelector; // HIGH selects tunnel0 while LOW selects tunnel1.
		
		IPetInTunnelDetectorListener* arPetInTunnelDetectorListeners[MAXRCATIDENTIFIERLISTENERCOUNT] = {};
		uint16_t nofPetInTunnelDetectorListeners;

		Timer timer;
		Queue<uint64_t, 2> queueRfIdDetections;

		IRfidDetector& rfidDetector;
		uint8_t pinCoilSelector;
		uint8_t pinPirTunnel0;
		uint8_t pinPirTunnel1;

		PetInTunnelDetectorState state;
		uint8_t tunnel; // The last tunnel that was queried for PIR value.

	public:
		static void StaticMain(void *pParam)
		{
			PetInTunnelDetector<MAXRCATIDENTIFIERLISTENERCOUNT>* THIS = (PetInTunnelDetector<MAXRCATIDENTIFIERLISTENERCOUNT>*) pParam;
			THIS->main();
		}

	public:
		PetInTunnelDetector(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber, 
			IRfidDetector& rfidDetector, uint8_t pinCoilSelector, uint8_t pinPirTunnel0, uint8_t pinPirTunnel1) :
			Task(taskName, taskPriority, 3500 + MAXRCATIDENTIFIERLISTENERCOUNT * sizeof(IPetInTunnelDetectorListener*), taskCoreNumber),
			nofPetInTunnelDetectorListeners(0), timer(this), queueRfIdDetections(this),
			rfidDetector(rfidDetector), pinCoilSelector(pinCoilSelector),
			pinPirTunnel0(pinPirTunnel0), pinPirTunnel1(pinPirTunnel1),
			state(PetInTunnelDetectorState::Pause), tunnel(0)
		{
			for (int i = 0;i < MAXRCATIDENTIFIERLISTENERCOUNT;i++)
			{
				arPetInTunnelDetectorListeners[i] = nullptr;
			}
			rfidDetector.addRfidDetectorListener(this);

			ard::pinMode(pinCoilSelector, GPIO_MODE_OUTPUT);
			ard::pinMode(pinPirTunnel0, GPIO_MODE_INPUT);
			ard::pinMode(pinPirTunnel1, GPIO_MODE_INPUT);
		}
		
		/*override*/
		void addPetInTunnelDetectorListener(IPetInTunnelDetectorListener* pPetInTunnelDetectorListener)
		{
			if (!isAlreadyPresent(pPetInTunnelDetectorListener))
			{
				assert(nofPetInTunnelDetectorListeners < MAXRCATIDENTIFIERLISTENERCOUNT);
				arPetInTunnelDetectorListeners[nofPetInTunnelDetectorListeners++] = pPetInTunnelDetectorListener;
			}
		}

		void main()
		{
			vTaskDelay(10); // wait for others to initialise
			logger.logText("PetInTunnelDetector_before while of main");
			while (true)
			{
				vTaskDelay(1); // avoid watchdog
				switch (state)
				{
				case PetInTunnelDetectorState::Pause:
					//logger.logText("PetInTunnelDetector_Pause");
					vTaskDelay(50);
					makeCentralDecisions();
					break;

				case PetInTunnelDetectorState::EnablingRfid:
					logger.logText("PetInTunnelDetector_Enabling");
					activateCoilForTunnel(tunnel);
					vTaskDelay(150); // relais takes some time to switch.
					state = PetInTunnelDetectorState::ListeningToRfid;
					break;

				case PetInTunnelDetectorState::ListeningToRfid:
					logger.logText("PetInTunnelDetector_Listening");
					queueRfIdDetections.clear();
					rfidDetector.startDetection();

					timer.start(100000);	// detection takes some time
					
					waitAny(timer + queueRfIdDetections);
					//logger.logInt32(queueRfIdDetections.getNofMessagesWaiting());
					if (hasFired(timer))
					{
						//logger.logText("timeout for rfid");
						makeCentralDecisions();
					}
					else if (hasFired(queueRfIdDetections))
					{
						//logger.logText("rfid detected");

						uint64_t rfid = 0;
						queueRfIdDetections.read(rfid);
						broadCastPetInTunnel(tunnel, rfid);
						//state = PetInTunnelDetectorState::Pause;
						makeCentralDecisions();
					}
					else 
					{
						logger.logText("Wait passed without one of both events?!?!?!?!?!");
					}

					timer.stop();
					queueRfIdDetections.clear();

					break;

				default:
					ESP_LOGI("PetInTunnelDetector:", "Undefined state");
					break;
				} //switch

#ifdef CRT_HIGH_WATERMARK_INCREASE_LOGGING
				auto temp = uxTaskGetStackHighWaterMark(nullptr);
				if (!prev_stack_hwm || temp < prev_stack_hwm)
				{
					prev_stack_hwm = temp;
					ESP_LOGI("hwm_check", "Task %s has left: %d stack uint8_ts and %d heap uint8_ts", taskName, prev_stack_hwm, unsigned(xPortGetFreeHeapSize()));
					//ESP_LOGI(taskName,"%d",unsigned(xPortGetFreeHeapSize()));vTaskDelay(1);
					//ESP_LOGI(taskName," heap uint8_ts","");vTaskDelay(1);
				}
#endif
			}
		}

		/*override keyword not supported in current compiler*/
		void setup()
		{
			start(); // Call start function of Task baseclass.
		}

		/*override*/
		void reportRfidDetection(IRfidDetector* pRfidDetector, uint64_t rfid)
		{
			logger.logText("reportRfidDetection called");
			queueRfIdDetections.write(rfid);
		}

	private:
		void makeCentralDecisions()
		{
			tunnel = nextTunnel(tunnel);
			if (isPirActive(tunnel))
			{
				state = PetInTunnelDetectorState::EnablingRfid;
			}
			else
			{
				if (!isPirActive(1))
				{
					activateCoilForTunnel(0);
				}
				state = PetInTunnelDetectorState::Pause;
			}
		}

		void activateCoilForTunnel(uint8_t tunnelIndex)
		{
			if (tunnelIndex == 0)
			{
				ard::digitalWrite(pinCoilSelector, ard::Low);
			}
			else
			{
				assert(tunnelIndex == 1);
				ard::digitalWrite(pinCoilSelector, ard::High);
			}
		}

		bool isPirActive(uint8_t tunnel)
		{
			bool bResult = (tunnel == 0) ? ard::digitalRead(pinPirTunnel0) : ard::digitalRead(pinPirTunnel1);
			return bResult; // Assuming positive logic of pir-output pin.
		}

		uint8_t nextTunnel(uint8_t tunnel)
		{
			return (tunnel == 0) ? 1 : 0;
		}

		void broadCastPetInTunnel(uint8_t tunnel, uint64_t rfId)
		{
			for (int i = 0; i < nofPetInTunnelDetectorListeners; i++)
			{
				arPetInTunnelDetectorListeners[i]->reportPetInTunnelDetection(tunnel,rfId);
			}
		}

		bool isAlreadyPresent(IPetInTunnelDetectorListener* pPetInTunnelDetectorListener)
		{
			for (int i = 0; i < nofPetInTunnelDetectorListeners; i++)
			{
				//ESP_LOGI("adasdffasfsadf", "%s", i);
				if (arPetInTunnelDetectorListeners[i] == pPetInTunnelDetectorListener)
				{
					return true;
				}
			}
			return false;
		}
	}; // end class PetInTunnelDetector
}; // end namespace CleanRTOS
