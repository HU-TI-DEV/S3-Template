#pragma once
#include "crt_typesAndConstants.h"

namespace crt
{
	enum class WidgetType :int
	{
		None = 0, Widget = 1, PageRoot = 2, Panel = 3, Text = 4,
		TouchscreenButton = 5, TouchscreenButtonGroup = 6
	};

	class IWidget
	{
	public:
		virtual bool addChildWidget(IWidget& widget) = 0;        // build a tree from back to forth: root is drawn first.
		virtual WidgetType getWidgetType() = 0;
		virtual void setName(const char* str) = 0;
		virtual const char* getName() = 0;
		virtual IDisplay* getDisplay() = 0;
		virtual void setDisplay (IDisplay& display) = 0;
		virtual Vec2 getGlobPosOfParent() = 0;					  // always in pixels. always in abs
		virtual void setGlobPosOfParent(const Vec2& locPos) = 0;  // coordinates with origin top left.
		virtual Vec2 getSizeOfParent() = 0;					      // always in pixels. always in abs
		virtual void setSizeOfParent(const Vec2& sizeOfParent) = 0;     // coordinates with origin top left.
		virtual Vec2 getLocPos() = 0;                             
		virtual void setLocPos(const Vec2& locPos, CoordType coordTypeLocPos, Alignment alignment) = 0; // CoordType can be Pixels or Promillage
		virtual Vec2 getSizePix() = 0;
		virtual Vec2 getSize() = 0;
		virtual void setSize(const Vec2& size, CoordType coordTypeSize) = 0;
		virtual Alignment getAlignment() = 0;
		virtual void show(bool bIncludeChildren) = 0;		  // show/hide is about the visual aspect of a widget.
		virtual void hide(bool bIncludeChildren) = 0;        // show immediately draws it. hide immediately undraws it.
		virtual bool isShown() = 0;
		virtual void enable(bool bIncludeChildren) = 0;	  // enable/disable is about the functional aspect of 
		virtual void disable(bool bIncludeChildren) = 0;   // a widget: wether it continues to responde to input,
		virtual bool isEnabled() = 0;   // for instance.
	};
};
