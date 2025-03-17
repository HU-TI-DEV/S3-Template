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
	template <size_t MaxNofChildren>
	class TouchscreenButtonGroup : public Widget<MaxNofChildren>, public ITouchListener
	{
	private:
		ITouchListener* arTouchListener[MaxNofChildren] = {};
		uint16_t nofTouchListeners;

	public:
		using WBase = Widget<MaxNofChildren>;
		TouchscreenButtonGroup(const char* strName) :
			  WBase(strName, Vec2(0,0)/*locPos*/, CoordType::Pixels/*coordTypeLocPos*/,
				    Vec2(1000,1000)/*size*/, CoordType::Promillage/*coordTypeSize*/, Alignment::TopLeft)
		{
		}

		// To add TouchscreenButtons or TouchscreenButtonGroups, use the function below.
		// Pass the object twice. (because a reliable sideways-cast does not exist).
		void addTouchListener(IWidget& widget, ITouchListener& touchListener)
		{
			WBase::addChildWidget(widget);

			if (!isAlreadyPresent(&touchListener))
			{
				assert(nofTouchListeners < MaxNofChildren);
				arTouchListener[nofTouchListeners++] = &touchListener;
			}
		}

		/*override*/ bool addChildWidget(IWidget& widget)
		{
			assert((widget.getWidgetType() != WidgetType::TouchscreenButton) &&
				(widget.getWidgetType() != WidgetType::TouchscreenButtonGroup));
			// If above asserts (it does so for TouchscreenButtons and TouchscreenButtonGroups),
			// you should have called addTouchListener instead.
			return WBase::addChildWidget(widget);
		}

		/* override */ WidgetType getWidgetType()
		{
			return WidgetType::TouchscreenButtonGroup;
		}

		/*override*/ void setSizeOfParent(const Vec2& sizeOfParent)
		{
			WBase::setSizeOfParent(sizeOfParent);
			// A TouchScreenButtonGroup takes the size of its parent.
			// Typically, that could be a Panel.
			// This can be of importance if the buttons that are 
			// added to the TouchScreenButtonGroup use CoordType::Promillage
			// for its positions and/or sizes.
			//WBase::setSize(sizeOfParent, CoordType::Pixels);
		}

		// Forward the touchPressed events to the buttons in this buttongroup.
		/* override */ void touchPressed(const Vec2& pos)
		{
			if (!WBase::isEnabled())
			{
				return;
			}

			ESP_LOGI("tsbg", "touch pressed");
			ESP_LOGI("tsbg", "nof touch listeners: %d", nofTouchListeners);
			for (int i = 0; i < nofTouchListeners; i++)
			{
				arTouchListener[i]->touchPressed(pos);
			}
		}

		/* override */ void touchReleased(const Vec2& pos)
		{
			if (!WBase::isEnabled())
			{
				return;
			}

			ESP_LOGI("tsbg", "touch released");
			ESP_LOGI("tsbg", "nof touch listeners: %d", nofTouchListeners);
			for (int i = 0; i < nofTouchListeners; i++)
			{
				arTouchListener[i]->touchReleased(pos);
			}
		}

	private:
		bool isAlreadyPresent(ITouchListener * pTouchListener)
		{
			for (int i = 0; i < nofTouchListeners; i++)
			{
				//ESP_LOGI("adasdffasfsadf", "%s", i);
				if (arTouchListener[i] == pTouchListener)
				{
					return true;
				}
			}
			return false;
		}
	};
} // end namespace crt
