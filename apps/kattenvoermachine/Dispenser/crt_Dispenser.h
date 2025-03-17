#pragma once
#include "../../../main/crt/crt_CleanRTOS.h"
#include "../../../apps/kattenvoerMachine/ServoSweeper/crt_ServoSweeper.h"
// crt::ServoSweeper
// by Marius Versteegen, 2022

namespace crt
{
	extern crt::ILogger& logger;
	namespace ard = crt::FromArduinoIDE;

	class Dispenser
	{
	private: 
		ServoSweeper& servoSweeperLuik;
		ServoSweeper& servoSweeperRoeren;	// NB: dit object kan gedeeld zijn met andere dispensers (afh. van het design).

	public:
		Dispenser(ServoSweeper& servoSweeperLuik, ServoSweeper& servoSweeperRoeren) :
			servoSweeperLuik(servoSweeperLuik),
			servoSweeperRoeren(servoSweeperRoeren)
		{
			servoSweeperLuik.addSweep(160, 170, 500/*ms*/);	// make sure that the hatch is initially closed
			servoSweeperRoeren.addSweep(20, 10, 500/*ms*/);	// make sure that the servo is not vibrating.
		}

		void dispensePortion()
		{
			servoSweeperLuik.addSweep(170/*startAngle*/, 90/*endAngle*/, 300/*ms*/);
			servoSweeperLuik.addSweep(90/*startAngle*/, 170/*endAngle*/, 300/*ms*/);
			servoSweeperRoeren.addSweep(10/*startAngle*/, 170/*endAngle*/, 500/*ms*/);
			servoSweeperRoeren.addSweep(170/*startAngle*/, 10/*endAngle*/, 500/*ms*/);
		}
	}; // end class crt_Dispenser
}; // end namespace CleanRTOS
