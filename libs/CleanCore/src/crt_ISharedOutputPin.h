#pragma once
#include "crt_ISharedOutputPinUser.h"

namespace crt
{
	class ISharedOutputPinUser;

	class ISharedOutputPin
	{
	public:
		virtual void addUser(ISharedOutputPinUser* sharedOutputPinUser) = 0;
		virtual void usePin(ISharedOutputPinUser* sharedOutputPinUser)  = 0;
		virtual void unUsePin(ISharedOutputPinUser* sharedOutputPinUser) = 0;
	}; // end class
}; // end namespace CleanRTOS