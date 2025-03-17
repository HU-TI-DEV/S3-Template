// by Marius Versteegen, 2023

#pragma once
#include "TFT_eSPI.h"
#include <User_Setup.h>  // for the define LOAD_GFXFF

namespace crt
{
#ifndef LOAD_GFXFF

	// Free fonts are not used.
	// Define a dummy GFXfont class though to minimize the need of #ifdefs 
	// to keep the code building.
	typedef struct {
		int dummy;
	} GFXfont;
#endif

	class TFT_eSPI_IFreeFonts
	{
	public:
		virtual void setFreeFont(uint8_t index, const GFXfont* pGfxFont) = 0;
		virtual const GFXfont* getFreeFont(uint8_t index) = 0;
	};
};