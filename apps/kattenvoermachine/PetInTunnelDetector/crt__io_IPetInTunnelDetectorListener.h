#pragma once
#include "crt__io_IPetInTunnelDetector.h"

namespace crt
{
	class IPetInTunnelDetectorListener
	{
	public:
		virtual void reportPetInTunnelDetection(uint8_t tunnelIndex, uint64_t rfid)=0;
	};
};