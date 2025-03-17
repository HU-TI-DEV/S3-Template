#pragma once
#include <assert.h>
#include <crt_FreeRTOS.h>
#include "crt_typesAndConstants.h"
#include "crt_IDisplay.h"
#include "crt_Widget.h"
#include "crt_ITouchListener.h"
#include <crt_IButton.h>

namespace crt
{
	template <size_t MaxNofChildren, size_t MaxNofKeyboardListeners>
	class TouchscreenKeyboardLowerCase : public TouchscreenButtonGroup<MaxNofChildren+40>,
		public IKeyboard, public IButtonListener
	{
	private: 
		const int nofKeysTopRow = 10;
		const int keyWidth = 1000 * 1000 / (1160 * nofKeysTopRow); // reserve 1/6 buttonwidth per button for padding
		const int paddingX  = (1000 - keyWidth * nofKeysTopRow)/(nofKeysTopRow+1);
		const int stepSizeX = keyWidth + paddingX;
		
		const int nofRows = 4;
		const int keyHeight = 1000 * 1000 / (1250 * nofRows); // reserve 1/4 buttonheight per button for padding
		const int paddingY = (1000 - keyHeight * nofRows)/(nofRows+1);
		const int stepSizeY = keyHeight + paddingY;

		const uint32_t colBg   = 0x00000000;
		const uint32_t colFont = 0x00FFFFFF;

		IKeyboardListener* arKeyboardListener[MaxNofKeyboardListeners] = {};
		uint16_t nofKeyboardListeners;

		bool bChildWidgetsAdded;

		//TouchscreenButton<0 /*MaxNofChildren*/, 1/*MaxButtonListenerCount*/> tsButtonA;
		TouchscreenButton<10, 1> tsButtonQ, tsButtonW, tsButtonA, tsButtonZ, tsButtonShiftUp;

	public:
		using Base = TouchscreenButtonGroup<MaxNofChildren + 40>;
		using WBase = Widget<MaxNofChildren + 40>;

			/*tsButtonB, tsButtonC, tsButtonD, tsButtonD, tsButtonE,
			tsButtonF, tsButtonG, tsButtonI, tsButtonJ, tsButtonK, tsButtonL, tsButtonM,
			tsButtonN, tsButtonO, tsButtonP, tsButtonQ, tsButtonR, tsButtonS, tsButtonT,
			tsButtonU, tsButtonV, tsButtonW, tsButtonX, tsButtonY, tsButtonZ, tsButtonShiftToUpper,
			tsButtonToggleToNumeric, tsButtonSlashForward, tsButtonBackSpace, tsButtonDot, tsButtonEnter*/

		TouchscreenKeyboardLowerCase(const char* strName) :
			Base::TouchscreenButtonGroup(strName),
			nofKeyboardListeners(0),
			bChildWidgetsAdded(false),
			// fontScale=0 triggers automatical matching of fontscale with the button height.
			tsButtonQ(/*name*/"bn_q",
				/*locPos*/Vec2(paddingX / 2, paddingY / 2),
				CoordType::Promillage, /*size*/Vec2(keyWidth, keyHeight), /*cornerRadius*/0, CoordType::Promillage,
				Alignment::TopLeft,	/*colPanel*/0x00FF0000, /*colBg*/0x00000000,
				"q", /*buttonFont*/ 2, /*colFont*/ 0x00FFFFFF, /*fontScale*/ 0, Alignment::MidMid),

			tsButtonW(/*name*/"bn_w",
				/*locPos*/Vec2(paddingX / 2 + stepSizeX, paddingY / 2),
				CoordType::Promillage, /*size*/Vec2(keyWidth, keyHeight), /*cornerRadius*/0, CoordType::Promillage,
				Alignment::TopLeft,	/*colPanel*/0x00FF0000, /*colBg*/0x00000000,
				"w", /*buttonFont*/ 2, /*colFont*/ 0x00FFFFFF, /*fontScale*/ 0, Alignment::MidMid),

			tsButtonA(/*name*/"bn_a",
				/*locPos*/Vec2(paddingX / 2 + keyWidth / 2, paddingY / 2 + stepSizeY),
				CoordType::Promillage, /*size*/Vec2(keyWidth, keyHeight), /*cornerRadius*/0, CoordType::Promillage,
				Alignment::TopLeft,	/*colPanel*/0x00FF0000, /*colBg*/0x00000000,
				"a", /*buttonFont*/ 2, /*colFont*/ 0x00FFFFFF, /*fontScale*/ 0, Alignment::MidMid),

			tsButtonZ(/*name*/"bn_z",
				/*locPos*/Vec2(paddingX / 2 + stepSizeX, paddingY / 2 + 2*stepSizeY),
				CoordType::Promillage, /*size*/Vec2(keyWidth, keyHeight), /*cornerRadius*/0, CoordType::Promillage,
				Alignment::TopLeft,	/*colPanel*/0x00FF0000, /*colBg*/0x00000000,
				"z", /*buttonFont*/ 2, /*colFont*/ 0x00FFFFFF, /*fontScale*/ 0, Alignment::MidMid),

			tsButtonShiftUp(/*name*/"bn_ShiftUp",
				/*locPos*/Vec2(paddingX / 2, paddingY / 2 + 3*stepSizeY),
				CoordType::Promillage, /*size*/Vec2(keyWidth, keyHeight), /*cornerRadius*/0, CoordType::Promillage,
				Alignment::TopLeft,	/*colPanel*/0x00FF0000, /*colBg*/0x00000000,
				"SHIFT", /*buttonFont*/ 2, /*colFont*/ 0x00FFFFFF, /*fontScale*/ 0, Alignment::MidMid)

		{
			tsButtonQ.addButtonListener(this);
			tsButtonW.addButtonListener(this);
			tsButtonA.addButtonListener(this);
			tsButtonZ.addButtonListener(this);
			tsButtonShiftUp.addButtonListener(this);
		}

	private:
		void addChildWidgets()
		{
			ESP_LOGI("keyb", "will Add ChildWidgets");
			Base::addTouchListener(tsButtonQ, tsButtonQ);
			Base::addTouchListener(tsButtonW, tsButtonW);
			Base::addTouchListener(tsButtonA, tsButtonA);
			Base::addTouchListener(tsButtonZ, tsButtonZ);
			Base::addTouchListener(tsButtonShiftUp, tsButtonShiftUp);
			bChildWidgetsAdded = true;
		}

	public:
		// next function will be called by the parent, when this touchscreenKeyBoardLowerCase
		// is added to it as a child widget.
		/*override*/ virtual void setDisplay(IDisplay& display)
		{
			Base::setDisplay(display);
			if (!bChildWidgetsAdded)
			{
				ESP_LOGI("setDisplay", "%s started adding ChildWidgets", this->getName());

				addChildWidgets();

				ESP_LOGI("setDisplay", "%s Added ChildWidgets", this->getName());
			}
		}

		/* override */ void buttonPressed(IButton* pButton)
		{
			ESP_LOGI("Pressed", "button: %s", pButton->getButtonName());
			/*int i;
			for (i = 0; i < nofKeyboardListeners; i++)
			{
				arKeyboardListener[i]->keyPressed(pButton->getButtonName());
			}*/
		}

		/* override */ void buttonReleased(IButton* pButton)
		{
			ESP_LOGI("Released", "button: %s", pButton->getButtonName());
			/*int i;
			for (i = 0; i < nofKeyboardListeners; i++)
			{
				arKeyboardListener[i]->keyReleased(pButton->getButtonName());
			}*/
		}
		
		/* override */
		void addKeyboardListener(IKeyboardListener* pKeyboardListener)
		{
			if (!isAlreadyPresent(pKeyboardListener))
			{
				assert(nofKeyboardListeners < MaxNofKeyboardListeners);
				arKeyboardListener[nofKeyboardListeners++] = pKeyboardListener;
			}
		}

	private:
		bool isAlreadyPresent(IKeyboardListener* pKeyboardListener)
		{
			for (int i = 0; i < nofKeyboardListeners; i++)
			{
				//ESP_LOGI("adasdffasfsadf", "%s", i);
				if (arKeyboardListener[i] == pKeyboardListener)
				{
					return true;
				}
			}
			return false;
		}
	};
} // end namespace crt
