// by Marius Versteegen, 2023

#pragma once
#include "TFT_eSPI.h"
#include "crt_TFT_eSPI_IFreeFonts.h"

namespace crt
{
	template <size_t MaxNofFreeFonts>
	class TFT_eSPI_FreeFonts : public TFT_eSPI_IFreeFonts
	{
	private:
		const GFXfont* freeFontMapping[MaxNofFreeFonts] = {}; // ={} should initialize all elements to zero/nullptr.
	public:
		/* override */ 
		void setFreeFont(uint8_t index, const GFXfont* pGfxFont)
		{
			if (index < 8)
			{
				ESP_LOGI("FreeFonts", "Sorry, the first 8 font indices are reserved for regular fonts");
			}
			freeFontMapping[index] = pGfxFont;
		}

		/* override */ 
		const GFXfont* getFreeFont(uint8_t index)
		{
			return freeFontMapping[index];
		}
	};
};