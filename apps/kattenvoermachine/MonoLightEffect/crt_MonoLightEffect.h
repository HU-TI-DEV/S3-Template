#pragma once
#include "../../../main/crt/crt_CleanRTOS.h"
#include "../../../main/libs/servo/src/analogWrite.h"
// crt::MonoLightEffect
// by Marius Versteegen, 2022

namespace crt
{
	extern crt::ILogger& logger;

	class MonoLightEffect : public Task
	{
	public:
		enum class Effect { Idle, Flash, SineWave, Triangle };

	private:
		static const int sizeSineWaveArray = 20;
		float arSineWave[sizeSineWaveArray]
			= { 0.0f, 0.309103287f, 0.587932054f, 0.809176962f,
				0.951168608f, 0.999999897f,	0.950888144f, 0.808643504f,
				0.587197851f, 0.308240247f, -0.000907346f, -0.309966072f,
				-0.588665774f, -0.809709754f, -0.951448289f, -0.999999074f,
				-0.950606897f, -0.80810938f, -0.586463163f, -0.307376954f };

		int nNofSubSamplesPerEntry;

		Timer timerFlash;
		Timer timerPeriodic;

		uint8_t pinPwmLed;
		
		uint64_t periodMs;	// for Triangle and SineWave
		uint32_t indexSineWave;
		uint32_t subSampleIndexSineWave;
		float fIndex;
		bool bSquaredOutput;

		uint32_t highMs;   // for Flash
		uint32_t lowMs;    // for Flash
		uint8_t highValue; // for Flash, Triangle and SineWave
		uint8_t lowValue;  // for Flash, Triangle and SineWave
		uint8_t curValue;  // for Flash, Triangle and SineWave

		uint64_t startTime;

		Effect effect;
		float cur;			// for Triangle
		float valueStep;    // for Triangle

	public:
		static void StaticMain(void *pParam)
		{
			MonoLightEffect* THIS = (MonoLightEffect*) pParam;
			THIS->main();
		}

	public:
		MonoLightEffect(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber, uint8_t pinPwmLed) :
			Task(taskName, taskPriority, 3000, taskCoreNumber), 
			timerFlash(this), timerPeriodic(this),
			pinPwmLed(pinPwmLed), effect(Effect::Idle)
		{
			stopCurEffect();
		}
		
		/*override*/
		void setup()
		{
			start();
		}

		void stopCurEffect()
		{
			crt::analogWrite(pinPwmLed, 0);
			timerFlash.stop();
			timerPeriodic.stop();
			effect = Effect::Idle;
		}

		void startFlash(uint8_t lowValue, uint8_t highValue, uint32_t highMs, uint32_t lowMs)
		{
			stopCurEffect();
			this->highMs	= highMs;
			this->highValue = highValue;
			this->lowMs		= lowMs;
			this->lowValue	= lowValue;
			curValue		= highValue; // assumption: Flashing should start immediately, so with the high value.
			crt::analogWrite(pinPwmLed, curValue);
			timerFlash.start(lowMs*1000);
			effect = Effect::Flash;
		}

		void startTriangle(uint8_t lowValue, uint8_t highValue, uint32_t periodMs, bool bSquaredOutput)
		{
			stopCurEffect();
			this->highValue = highValue;
			this->lowValue = lowValue;
			this->periodMs = periodMs;
			this->bSquaredOutput = bSquaredOutput;
			uint64_t updatePeriodUs = 20000; // That is, 20Ms, enough for 50 updates per sec.

			valueStep = float(highValue - lowValue) * float(updatePeriodUs)*2.0f / (1000.0f*periodMs);
			cur = lowValue;
			
			timerPeriodic.start_periodic(updatePeriodUs);
			effect = Effect::Triangle;
		}

		void startSineWave(uint8_t lowValue, uint8_t highValue, uint32_t periodMs, bool bSquaredOutput)
		{
			stopCurEffect();
			this->highValue = highValue;
			this->lowValue = lowValue;
			this->periodMs = periodMs;
			this->bSquaredOutput = bSquaredOutput;

			indexSineWave = 0;
			subSampleIndexSineWave = 0;

			nNofSubSamplesPerEntry = periodMs / 300; // That is, if the sinewave is to spread out beyound 300 ms,
			                                         // we will start subsampling.
			if (nNofSubSamplesPerEntry == 0) {
				nNofSubSamplesPerEntry = 1;			 // Without subsampling, the subsample is identical to the sample.
			}

			timerPeriodic.start_periodic(periodMs * 1000 / sizeSineWaveArray / nNofSubSamplesPerEntry);
			effect = Effect::SineWave;
		}

	private:
		float lerp(float f1, float f2, float loper)
		{
			float result = f1 + (f2 - f1)*loper;
			return result;
		}

		void main()
		{
			vTaskDelay(10); // wait for others to initialise
			while (true)
			{
				waitAny(timerFlash + timerPeriodic);
				if(hasFired(timerFlash))
				{
					if (curValue == lowValue)
					{
						curValue = highValue;
						crt::analogWrite(pinPwmLed, curValue);
						timerFlash.start(highMs*1000);
					}
					else
					{
						curValue = lowValue;
						crt::analogWrite(pinPwmLed, curValue);
						timerFlash.start(lowMs*1000);
					}
				}
				if (hasFired(timerPeriodic))
				{
					switch (effect)
					{
					case Effect::SineWave:
					{
						subSampleIndexSineWave++;
						if (subSampleIndexSineWave >= nNofSubSamplesPerEntry)
						{
							subSampleIndexSineWave = 0;
							indexSineWave++;
						}

						if (indexSineWave >= sizeSineWaveArray)
						{
							indexSineWave = 0;
						}

						uint32_t indexNextEntry = indexSineWave + 1;
						if (indexNextEntry >= sizeSineWaveArray)
						{
							indexNextEntry = 0;
						}

						float interpolatedSineWave = lerp(arSineWave[indexSineWave], arSineWave[indexNextEntry], (float)subSampleIndexSineWave / (float)nNofSubSamplesPerEntry);
						
						cur = interpolatedSineWave * float(highValue - lowValue) * 0.5f
							+ ((float)highValue + (float)lowValue)*0.5f + (float)lowValue;

						/*logger.logText("index");
						logger.logUint32((uint32_t)indexSineWave);
						logger.logText("entry");
						logger.logFloat(arSineWave[indexSineWave]);

						logger.logText("fullsine");
						logger.logFloat(arSineWave[indexSineWave] * float(highValue - lowValue) * 0.5f);

						logger.logText("fullsine offset");
						logger.logFloat(arSineWave[indexSineWave] * float(highValue - lowValue) * 0.5f + ((float)highValue + (float)lowValue)*0.5f);

						logger.logText("lowValue");
						logger.logFloat((float)lowValue);*/

						cur -= 0.5f;
						if (cur < 0.0f) {
							cur = 0.0f;
						}
						if (cur > 255.0f) {
							cur = 255.0f;
						}

						if (bSquaredOutput)
						{
							cur = cur / 255.0f;
							cur = cur * cur;
							cur *= 255.0f;
						}
						curValue = (uint8_t)cur;

						crt::analogWrite(pinPwmLed, (uint8_t)curValue);
						/*logger.logText("analogwritten t pwmLed:");
						logger.logFloat((uint32_t)cur);
						logger.logUint32((uint32_t)curValue);*/
					}
					break;

					case Effect::Triangle:
					{
						if (cur > (float)highValue)
						{
							if (valueStep > 0.0f)
							{
								valueStep *= -1.0f;
							}
						}
						if (cur < (float)lowValue)
						{
							if (valueStep < 0.0f)
							{
								valueStep *= -1.0f;
							}
						}

						logger.logText("cur orig:");
						logger.logFloat(cur);

						logger.logText("valueStep:");
						logger.logFloat(valueStep);

						cur += valueStep;
						float output = cur;

						if (output < 0.0f) {
							output = 0.0f;
						}
						if (output > 255.0f) {
							output = 255.0f;
						}

						if (bSquaredOutput)
						{
							output = output / 255.0f;
							output = output * output;
							output *= 255.0f;
						}

						curValue = (uint8_t)output;

						crt::analogWrite(pinPwmLed, (uint8_t)curValue);
						logger.logText("analogwritten t pwmLed:");
						logger.logFloat(output);
						logger.logUint32((uint32_t)curValue);
					}
					break;
						
					default:
						ESP_LOGI("monolighteff", "unknown effect type");
						break;
					}
				}

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
			}
		}
	}; // end class crt_MonoLightEffect
}; // end namespace CleanRTOS
