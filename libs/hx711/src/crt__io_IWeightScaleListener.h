#pragma once
//#include "crt_IWeightScale.h"

namespace crt
{
	// Note: Forward referencing with IWeightScale because of mutual dependency could have 
	// been avoided if not IWeightScale* but an enum value would be passed to identify the weightScale. 
	// But I like to avoid the hassle of bookkeeping enums.
	// Mutual dependency of pure abstract interfaces is perfectly sound.
	//class IWeightScale;
	enum class weightScaleError_t { None, TimeOut };

	class IWeightScaleListener
	{
	public:
		virtual void ReportWeight(const char* scaleName, long measurement)=0;
		virtual void ReportError(const char* scaleName, weightScaleError_t error) = 0;
	};
};