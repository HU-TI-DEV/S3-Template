#pragma once
// Important: when using CleanRTOS, 
// #include<CleanRTOS> at this location or prior to including THIS
// crt_TFT_eSPI_DisplayAdapter.h file.

//#include <Arduino.h>
#include "crt_typesAndConstants.h"
#include <LITTLEFS.h>
//#include "Free_Fonts.h" // Include the header file attached to this sketch
#include <crt_TFT_eSPI_IFreeFonts.h>
#include <TFT_eSPI.h>     // Hardware-specific library
#include <crt_tft_eSPI_TouchDetector.h>
#include <crt_IDisplay.h>
#include <crt_Vec2.h>
#include <User_Setup.h>   // In that file, LOAD_GFXFF determines availability of free fonts.

//#define Z_THRESHOLD 350 // Touch pressure threshold for validating touches

#ifdef LOAD_GFXFF
// From Free_Fonts.h :
#define GFXFF 1
#endif

namespace crt
{
    //class TFT_eSPI_accessable;

    template <size_t MaxTouchListenerCount> 
    class TFT_eSPI_DisplayAdapter : public IDisplay
    {
    private:
        TFT_eSPI _tft;
        TFT_eSPI_TouchDetector<MaxTouchListenerCount> _touchDetector;
        TFT_eSPI_IFreeFonts& _freeFonts;
        const char* _filenameCalibration;
        bool _bRepeatCalibration;
        
    public:
        TFT_eSPI_DisplayAdapter(TFT_eSPI_IFreeFonts& freeFonts, const char* filenameCalibration, 
                                bool bRepeatCalibration)
            :_touchDetector(_tft),
            _freeFonts(freeFonts), _filenameCalibration(filenameCalibration),
            _bRepeatCalibration(bRepeatCalibration)
        {}

        /*override*/ void pollTouch(void* pCallingTask)
        {
            _touchDetector.update(pCallingTask);
        }

        /*override*/ void addTouchListener(ITouchListener* pTouchListener)
        {
            _touchDetector.addTouchListener(pTouchListener);
        }

        /*override*/ Vec2 getScreenSize()
        {
            return Vec2(_tft.width(),_tft.height());
        }

        /*override*/ int16_t getTextWidth(const char* str, uint8_t font)
        {
            uint8_t usedFont = selectFontOrFreeFont(font);
            return _tft.textWidth(str, usedFont);
        }

        /*override*/ int16_t getFontHeight(uint8_t font)
        {
            uint8_t usedFont = selectFontOrFreeFont(font);
            return _tft.fontHeight(usedFont);
        }

        /*override*/ void setFontScale(uint8_t fontScale)
        {
            _tft.setTextSize(fontScale);
        }

        /*override*/ Vec2 getPrintCursor()
        {
            Vec2 cursor(_tft.getCursorX(), _tft.getCursorY());
            return cursor;
        }

        /*override*/ void fillScreen(uint32_t color)
        {
            _tft.fillScreen(colTo16bit(color));
        }

		/*override*/ void drawRect(const Vec2& topLeftPix, const Vec2& sizePix, uint32_t color)
		{
			_tft.drawRect(topLeftPix.x, topLeftPix.y, sizePix.x, sizePix.y, colTo16bit(color));
		}

        /*override*/ void fillRect(const Vec2& topLeftPix, const Vec2& sizePix, uint32_t color)
        {
            _tft.fillRect(topLeftPix.x, topLeftPix.y, sizePix.x, sizePix.y, colTo16bit(color));
        }

        /*override*/ void drawRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t color)
        {
            _tft.drawRoundRect(topLeftPix.x, topLeftPix.y, sizePix.x, sizePix.y, radius, colTo16bit(color));
        }

        /*override*/ void fillRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t color)
        {
            // Note: this function draws something ugly in the current implementation of TFT_eSPI:
            // The rounded corners do not align with the sides of the button.
            // For now, it is better to use the slow and pretty fillSmoothRoundRect function below instead.
            _tft.fillRoundRect(topLeftPix.x, topLeftPix.y, sizePix.x, sizePix.y, radius, colTo16bit(color));
        }

        /*override*/ void fillSmoothRoundRect(const Vec2& topLeftPix, const Vec2& sizePix, int32_t radius, uint32_t colFg, uint32_t colBg)
        {
            _tft.fillSmoothRoundRect(topLeftPix.x, topLeftPix.y, sizePix.x, sizePix.y, radius, colTo16bit(colFg), colTo16bit(colBg));
        }

        /*override*/ void drawString(const char* str, const Vec2& bottomLeftPix, Alignment alignment, uint8_t font, uint8_t scale, uint32_t color)
        {
            uint8_t textDatum = 0;

            switch (alignment)
            {
            case Alignment::TopLeft:
                textDatum = TL_DATUM;
                break;
            case Alignment::BottomLeft:
                textDatum = BL_DATUM;
                break;
            case Alignment::MidLeft:
                textDatum = CL_DATUM;
                break;
            case Alignment::TopRight:
                textDatum = TR_DATUM;
                break;
            case Alignment::BottomRight:
                textDatum = BR_DATUM;
                break;
            case Alignment::MidRight:
                textDatum = CR_DATUM;
                break;
            case Alignment::MidMid:
                textDatum = MC_DATUM;
                break;
            case Alignment::TopMid:
                textDatum = TC_DATUM;
                break;
            case Alignment::BottomMid:
                textDatum = BC_DATUM;
                break;

            default:
                ESP_LOGI("TFT_eSPI_DisplayAdapter", "Unimplemented alignment: %d", (int)alignment);
                break;
            }
            _tft.setTextDatum(textDatum);
            _tft.setTextColor(colTo16bit(color));
            _tft.setTextSize(scale);
            uint8_t usedFont = selectFontOrFreeFont(font);
            _tft.drawString(str, bottomLeftPix.x, bottomLeftPix.y, usedFont);
            _tft.setTextSize(1);
        }

        /*override*/ void setPrintColor(void* pTask, uint32_t color)
        {
            _tft.setTextColor(colTo16bit(color));
        }

        /*override*/ void setPrintFont(void* pTask, uint8_t font)
        {
            selectFontOrFreeFont(font);
        }

        /*override*/ void setPrintScale(void* pTask, uint8_t fontScale)
        {
            _tft.setTextSize(fontScale);
        }

        /*override*/ void setPrintCursor(void* pTask, const Vec2& cursorPos)
        {
            _tft.setCursor(cursorPos.x, cursorPos.y);
        }

        /*override*/ void setPrintWrap(void* pTask, bool bWrapX)
        {
            _tft.setTextWrap(bWrapX);
        }

        /*override*/ void print(void* pTask, const char* str)
        {
            _tft.print(str);
        }

        /*override*/ void println(void* pTask, const char* str)
        {
            _tft.println(str);
        }

        uint16_t colTo16bit(uint32_t color) // ARGB
        {
            uint16_t color16 = ( (((color & 0x00FF0000) >> 19 ) << 11) |   // Take the 5 MSB bits of red.
                                 (((color & 0x0000FF00) >> 10 ) << 5 ) |   // Take the 6 MSB bits of green.
                                  ((color & 0x000000FF) >> 3  )         ); // Take the 5 MSB bits of blue.
            return color16;
        }

        bool queryDoYouWantToRecalibrate()
        {
            _tft.setCursor(20, 0);
            _tft.setTextFont(2);
            _tft.setTextSize(1);
            _tft.setTextColor(TFT_WHITE, TFT_BLACK);

            _tft.println("Touch now to recalibrate");
            
            int64_t endTime = esp_timer_get_time()+2000000;     // time in micro seconds. wait 2s.

            uint16_t x_tmp, y_tmp;

            bool bValidTouch = false;
            while (!bValidTouch && ((esp_timer_get_time()-endTime)<0))
            {
                //bValidTouch = _tft.validTouch(&x_tmp, &y_tmp, Z_THRESHOLD / 2));
                bValidTouch = _tft.getTouch(&x_tmp, &y_tmp); // Perhaps improve on this, inspired by _tft.validTouch.
            }
            
            if (bValidTouch)
            {
                ESP_LOGI("DisplayAdapter", "touchcalibration requested");
            }
            
            return bValidTouch;
        }

        // The function below was for the greater part copied from Bodmer.
        /*override*/ void touchCalibrate(uint8_t rotation, uint8_t font)
        {
            _tft.begin();
            _tft.setRotation(rotation);
            _tft.fillScreen(TFT_BLACK);
            _tft.setTextFont(font);  //tft.setFreeFont(FF18);	

            uint16_t calData[5];
            uint8_t calDataOK = 0;

            // check file system exists
            if (!LITTLEFS.begin()) {
                ESP_LOGI("DisplayAdapter","Formating file system");
                LITTLEFS.format();
                LITTLEFS.begin();
            }

            bool bAskedForRecalibration = false;
            _bRepeatCalibration = false; // can change below

            // check if calibration file exists and size is correct
            if (LITTLEFS.exists(_filenameCalibration)) {
                _bRepeatCalibration = queryDoYouWantToRecalibrate();
                bAskedForRecalibration = true;

                if (_bRepeatCalibration)
                {
                    // Delete if we want to re-calibrate
                    LITTLEFS.remove(_filenameCalibration);
                }
                else
                {
                    fs::File f = LITTLEFS.open(_filenameCalibration, "r");
                    if (f) {
                        if (f.readBytes((char*)calData, 14) == 14)
                            calDataOK = 1;
                        f.close();
                    }
                }
            }

            if (calDataOK && !bAskedForRecalibration)
            {
                _bRepeatCalibration = queryDoYouWantToRecalibrate();
            }

            if (calDataOK && !_bRepeatCalibration) {
                // calibration data valid
                _tft.setTouch(calData);
            }
            else 
            {
                // data not valid or recalibration requested so recalibrate
                _tft.fillScreen(TFT_BLACK);
                _tft.setCursor(20, 0);
                _tft.setTextFont(2);
                _tft.setTextSize(1);
                _tft.setTextColor(TFT_WHITE, TFT_BLACK);

                _tft.println("Touch corners as indicated");

                _tft.setTextFont(1);
                _tft.println();

                if (_bRepeatCalibration) {
                    _tft.setTextColor(TFT_RED, TFT_BLACK);
                }

                _tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

                _tft.setTextColor(TFT_GREEN, TFT_BLACK);
                _tft.println("Calibration complete!");

                // store data
                fs::File f = LITTLEFS.open(_filenameCalibration, "w");
                if (f) {
                    f.write((const unsigned char*)calData, 14);
                    f.close();
                }
            }

            _tft.fillScreen(TFT_BLACK);
        }

    private:
        uint8_t selectFontOrFreeFont(uint8_t font)
        {
#ifdef LOAD_GFXFF

            const GFXfont* freeFont = _freeFonts.getFreeFont(font);
            if (freeFont != nullptr)
            {
                _tft.setFreeFont(freeFont);
                return GFXFF; // Meaning: use the currently set free font.
            }
            else // no free font available
            {
                _tft.setTextFont(font);
                return font;
            }
#else
            _tft.setTextFont(font);
            return font;
#endif
        }
	};
};
