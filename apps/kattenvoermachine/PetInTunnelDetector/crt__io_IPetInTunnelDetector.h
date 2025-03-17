#pragma once
#include "crt__io_IPetInTunnelDetectorListener.h"

namespace crt
{
	class IPetInTunnelDetectorListener;

	class IPetInTunnelDetector
	{
	public:
		virtual void addPetInTunnelDetectorListener(IPetInTunnelDetectorListener* pPetInTunnelDetectorListener) = 0;
	};
};