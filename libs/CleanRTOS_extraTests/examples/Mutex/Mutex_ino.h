// by Marius Versteegen, 2023

// This file demonstrates locking multiple mutexes to keep the associated resources in sync.
// To avoid a potential dealock, a Mutex lock of a certain Mutex id may not be followed by a Mutex lock with a lower id.
// The Mutex checks that.
// To test it, in sdkconfig set CONFIG_LOG_DEFAULT_LEVEL_VERBOSE=y, such that assert debug information is shown correctly.
// Then, uncomment some of the task definitions in Mutex_ino.h to emulate a deadlock situation.

#include <crt_CleanRTOS.h>        // This file includes crt_Config.h  You'll need to change defines there for a release build.
#include "crt_TestMutex.h"

// All Tasks should be created in this main file.

namespace crt
{
	MainInits mainInits;            // Initialize CleanRTOS.

	// Shared resources
	int32_t sharedIntA;
	int32_t sharedIntB;
	int32_t sharedIntC;

	// The mutexes that guard them.

	// Note: this file is actually part of the main.cpp (or .ino) file, as this file is included from that.
	// It is a good idea to define all mutexes that your application needs in this file.
	// That way, you can make sure that every mutex gets a unique id, and that the
	// order of the ids is correct (when multiple mutexes are locked concurrently,
	// Within each thread (separately), MUTEXES WITH SMALLER ID MUST BE LOCKED BEFORE MUTEXES WITH LARGER ID.

	Mutex mutexSharedIntA(1);		// Mutex with id 1, which protects sharedIntA
	Mutex mutexSharedIntB(2);		// Mutex with id 2, which protects sharedIntB
	Mutex mutexSharedIntC(3);		// Mutex with id 3, ,, . 
	// NOTE: To avoid deadlock potential, it is forbidden to lock a new mutex with an id
	// that is lower than the highest id of currently locked mutexes.
	// Mutexes (and MutexSections) automatically detect such situations.

	// Next thread should NOT trigger an assert, as the mutex lock priorities are honoured within its thread.
	SyncedNumbersIncreaser syncedNumbersIncreaserABC("SyncedNumbersIncreaser ABC", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
		sharedIntA, mutexSharedIntA, sharedIntB, mutexSharedIntB, sharedIntC, mutexSharedIntC); // Don't forget to call its start() member during setup().

	// Next thread should NOT trigger an assert, as the mutex lock priorities are honoured within its thread.
	// Comment it out to test asserts for the other threads as well.
	SyncedNumbersDisplayer SyncedNumbersDisplayerSameABC("SyncedNumbersDisplayer ABC", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
		sharedIntA, mutexSharedIntA, sharedIntB, mutexSharedIntB, sharedIntC, mutexSharedIntC);

	// Next thread should NOT trigger an assert, as the mutex lock priorities are honoured within its thread.
	// Comment it out to test asserts for the other threads as well.
	SyncedNumbersDisplayer SyncedNumbersDisplayerSameABC_2("SyncedNumbersDisplayer ABC", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
		sharedIntA, mutexSharedIntA, sharedIntB, mutexSharedIntB, sharedIntC, mutexSharedIntC);

	//// Next thread should trigger an assert, as the mutex lock priorities are NOT honoured within its thread.
	//// Comment it out to test asserts for the other threads as well.
	//SyncedNumbersIncreaser syncedNumbersIncreaserCBA("SyncedNumbersIncreaser CBA", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
	//	sharedIntC, mutexSharedIntC, sharedIntB, mutexSharedIntB, sharedIntA, mutexSharedIntA); // Don't forget to call its start() member during setup().

	//// Next thread should trigger an assert, as the mutex lock priorities are NOT honoured within its thread.
	//// Comment it out to test asserts for the other threads as well.
	//SyncedNumbersIncreaser syncedNumbersIncreaserCAB("SyncedNumbersIncreaser CAB", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
	//	sharedIntC, mutexSharedIntC, sharedIntA, mutexSharedIntA, sharedIntB, mutexSharedIntB); // Don't forget to call its start() member during setup().

	//// Next thread should trigger an assert, as the mutex lock priorities are NOT honoured within its thread.
	//// Comment it out to test asserts for the other threads as well.
	//SyncedNumbersIncreaser syncedNumbersIncreaserACB("SyncedNumbersIncreaser ACB", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
	//	sharedIntA, mutexSharedIntA, sharedIntC, mutexSharedIntC, sharedIntB, mutexSharedIntB); // Don't forget to call its start() member during setup().

	//// Next thread should trigger an assert, as the mutex lock priorities are NOT honoured within its thread.
	//// Comment it out to test asserts for the other threads as well.
	//SyncedNumbersIncreaser syncedNumbersIncreaserBAC("SyncedNumbersIncreaser BAC", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
	//	sharedIntB, mutexSharedIntB, sharedIntA, mutexSharedIntA, sharedIntC, mutexSharedIntC); // Don't forget to call its start() member during setup().

	//// Next thread should trigger an assert, as the mutex lock priorities are NOT honoured within its thread.
	//// Comment it out to test asserts for the other threads as well.
	//SyncedNumbersIncreaser syncedNumbersIncreaserBCA("SyncedNumbersIncreaser BCA", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
	//	sharedIntB, mutexSharedIntB, sharedIntC, mutexSharedIntC, sharedIntA, mutexSharedIntA); // Don't forget to call its start() member during setup().
}

void setup()
{
	// initialize serial communication at 115200 bits per second:
	// Serial.begin(115200); // Only needed when using Serial.print();

	vTaskDelay(10);// allow tasks to initialize.
	ESP_LOGI("checkpoint", "start of main"); vTaskDelay(1);
	ESP_LOGI("Important for this test", "in sdkconfig set CONFIG_LOG_DEFAULT_LEVEL_VERBOSE=y, such that assert debug information is shown correctly.");
	ESP_LOGI("Important for this test 2", "uncomment some of the task definitions in Mutex_ino.h to emulate a deadlock situation.");
}

void loop()
{
	vTaskDelay(1);// Nothing to do in loop - all example code runs in the 4 threads above.
}
