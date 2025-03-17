#pragma once
#include <assert.h>
#include <crt_FreeRTOS.h>
#include "crt_typesAndConstants.h"
#include "crt_IDisplay.h"
#include "crt_Widget.h"

namespace crt
{
	template <size_t MaxNofChildren> 
	class Panel : public Widget<MaxNofChildren>
	{
	private:
		uint32_t _colFg, _colBg;
		int32_t _cornerRadius;
		int32_t _cornerRadiusPix;

		int32_t _cornerRadiusWhenCornerRadiusPixWasCalculated;
	public:
		using WBase = Widget<MaxNofChildren>;

		Panel(const char* strName, const Vec2& locPos, CoordType coordTypeLocPos,
			  const Vec2& size, int32_t cornerRadius, CoordType coordTypeSize, 
			  Alignment alignment, uint32_t colFg, uint32_t colBg) :
			  WBase(strName, locPos, coordTypeLocPos, size, coordTypeSize, alignment),
			  _colFg(colFg), _colBg(colBg), _cornerRadius(cornerRadius), 
			  _cornerRadiusPix(-1),
			  _cornerRadiusWhenCornerRadiusPixWasCalculated(-1)
		{
		}

		/* override */ WidgetType getWidgetType()
		{
			return WidgetType::Panel;
		}

		// show immediately draws it. hide immediately undraws it.
		// note: _widgetProps only serves as a node in the display-tree.
		//       It conains the props and links to children.
		//       The actual (un-)display functionality is to be implemented
		//       by the specific widget class (like TouchScreenButton),
		//       which is derived from IWidget as well.
		/*override*/ void show(bool bIncludeChildren)
		{
			showPanel();
			Widget<MaxNofChildren>::show(bIncludeChildren); // set the boolean bVisible to remember.
		}

		// show immediately draws it. hide immediately undraws it.
		/*override*/ void hide(bool bIncludeChildren)
		{
			hidePanel();
			WBase::hide(bIncludeChildren); // sets the boolean bVisible to false to remember.
		}

	private:
		void updateCornerRadiusPixIfNeeded()
		{
			if (_cornerRadius != _cornerRadiusWhenCornerRadiusPixWasCalculated)
			{
				// apply same scaling as for size to sizePix:
				if (WBase::getSize().x != 0)
				{
					int64_t tempRadiusPix = (int64_t)_cornerRadius * (int64_t)WBase::getSizePix().x / (int64_t)WBase::getSize().x;
					_cornerRadiusPix = int32_t(tempRadiusPix);
				}
				else
				{
					ESP_LOGI("Zero size of panel?", "div by zero avoided");
				}
			}
		}

		void showPanel()
		{
			updateCornerRadiusPixIfNeeded();

			if (_cornerRadiusPix > 0)
			{
				WBase::getDisplay()->fillSmoothRoundRect(WBase::getGlobPosOfParent() + WBase::getLocPosPix(), WBase::getSizePix(), _cornerRadiusPix, _colFg, _colBg);
			}
			else
			{
				WBase::getDisplay()->fillRect(WBase::getGlobPosOfParent() + WBase::getLocPosPix(), WBase::getSizePix(), _colFg);
			}
			//ESP_LOGI("Panel shown", "globparent Y = %d", _widgetProps.getGlobPosOfParent().y);
			//ESP_LOGI("Panel shown", "locpospix Y = %d", _widgetProps.getLocPosPix().y);
			//ESP_LOGI("Panel shown", "locpos Y = %d", _widgetProps.getLocPos().y);
		}

		void hidePanel()
		{
			WBase::getDisplay()->fillRect(WBase::getGlobPosOfParent() + WBase::getLocPosPix(), WBase::getSizePix(), _colBg);
		}
	};
};
