
// This class is obsolete. (because the listener already breaks the loop). TODO: remove.

#pragma once
#include "crt_IWeightScaleListener.h"

namespace crt
{
	class IWeightScaleListener;
	enum class weightScaleError_t;

	class IWeightScale
	{
	public:
		virtual void addWeightScaleListener(IWeightScaleListener* pWeightScaleListener) = 0;

		virtual void setScale(float scale)		= 0;
		virtual float getScale()				= 0;
		virtual void setOffset(long offset)		= 0;
		virtual void enable()					= 0;
		virtual void disable()					= 0;
	}; // end class IWeightScale
}; // end namespace CleanRTOS