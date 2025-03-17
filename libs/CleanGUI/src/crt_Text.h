#pragma once
#include <assert.h>
#include <crt_FreeRTOS.h>
#include "crt_typesAndConstants.h"
#include "crt_IDisplay.h"
#include "crt_Widget.h"
#include "crt_Vec2.h"

namespace crt
{
	// The String used here

	template <size_t MaxNofChildren> 
	class Text : public Widget<MaxNofChildren>
	{
	private:
		//const char* str;
		uint8_t font;
		uint8_t fontScale;  // integer pix multiplier
		uint32_t color;
		bool bSizeUpToDate;
	public:
		using WBase = Widget<MaxNofChildren>;

		Text(const Vec2& locPos, CoordType coordTypeLocPos, 
			  Alignment alignment,
			  const char* str, uint8_t font, uint32_t color, uint8_t fontScale) :
			  WBase(str, locPos, coordTypeLocPos, Vec2(0,0), CoordType::Pixels, alignment),
			  /*str(str), */font(font), fontScale(fontScale), color(color), bSizeUpToDate(false)
			  {
				  // determine dimensions of the text .. that'll need to 
				  // be postponed till we have a handle of the display to ask it.
			  }

		/* override */ WidgetType getWidgetType()
		{
			return WidgetType::Text;
		}

		uint8_t getFontScale()
		{
			return fontScale;
		}

		void setFontScale(uint8_t fontScale)
		{
			if (fontScale != this->fontScale)
			{
				this->fontScale = fontScale;
				updateSize();
			}
		}

		uint8_t getFont()
		{
			return font;
		}

		void setFont(uint8_t font)
		{
			if (font != this->font)
			{
				this->font = font;
				updateSize();
			}
		}

		/* override */  virtual void setName(const char* str)
		{
			setText(str);
		}

		/* override */  virtual const char* getName()
		{
			return getText();
		}

		const char* getText()
		{
			//return str;
			return WBase::getName();
		}

		void setText(const char* str)
		{
			if (str != WBase::getName())
			{
				WBase::setName(str);
				updateSize();
			}
		}

		// show immediately draws it. hide immediately undraws it.
		// note: _widgetProps only serves as a node in the display-tree.
		//       It conains the props and links to children.
		//       The actual (un-)display functionality is to be implemented
		//       by the specific widget class (like TouchScreenButton),
		//       which is derived from IWidget as well.
		/*override*/ void show(bool bIncludeChildren)
		{
			showText();
			WBase::show(bIncludeChildren); // set the boolean bVisible to remember.
		}

		// show immediately draws it. hide immediately undraws it.
		/*override*/ void hide(bool bIncludeChildren)
		{
			hideText();
			WBase::hide(bIncludeChildren); // sets the boolean bVisible to false to remember.
		}

	private:
		void updateSize()
		{
			IDisplay* display = WBase::getDisplay();
			if (display == nullptr)
			{
				return; // Size cannot be updated till the display is attached.
			}

			Vec2 size(display->getTextWidth(WBase::getName(), font) * fontScale, display->getFontHeight(font) * fontScale);
			WBase::setSize(size, CoordType::Pixels);
		}

		void showText()
		{
			Vec2 origin;

			switch (WBase::getAlignment())
			{
			case Alignment::BottomLeft:
				{
					int32_t parentVertSizeMinusLocVertSize = WBase::getSizeOfParent().y;// -WBase::getSize().y;
					origin = WBase::getGlobPosOfParent() + Vec2(0, parentVertSizeMinusLocVertSize);
				}
				break;

			case Alignment::TopLeft:
				origin = WBase::getGlobPosOfParent();
				break;

			case Alignment::MidLeft:
			{
				int32_t parentVertSizeMinusLocVertSizeDiv2 = WBase::getSizeOfParent().y/2;// -WBase::getSize().y;
				origin = WBase::getGlobPosOfParent() + Vec2(0, parentVertSizeMinusLocVertSizeDiv2);
			}
			break;

			case Alignment::TopRight:
			{
				origin = WBase::getGlobPosOfParent() + Vec2(WBase::getSizeOfParent().x, 0);
			}
			break;

			case Alignment::BottomRight:
			{
				origin = WBase::getGlobPosOfParent() + Vec2(WBase::getSizeOfParent().x, WBase::getSizeOfParent().y);
			}
			break;

			case Alignment::MidRight:
			{
				origin = WBase::getGlobPosOfParent() + Vec2(WBase::getSizeOfParent().x, WBase::getSizeOfParent().y/2);
			}
			break;

			case Alignment::BottomMid:
			{
				origin = WBase::getGlobPosOfParent() + Vec2(WBase::getSizeOfParent().x/2, WBase::getSizeOfParent().y);
			}
			break;

			case Alignment::TopMid:
			{
				origin = WBase::getGlobPosOfParent() + Vec2(WBase::getSizeOfParent().x/2, 0);
			}
			break;
			
			case Alignment::MidMid:
			{
				origin = WBase::getGlobPosOfParent() + Vec2(WBase::getSizeOfParent().x/2, WBase::getSizeOfParent().y / 2);
			}
			break;

			default:
				ESP_LOGI("crt_Text", "alignment %d is not implemented yet",(int)WBase::getAlignment());
				break;
			}

			WBase::getDisplay()->drawString(WBase::getName(), origin + WBase::getLocPosPix(), WBase::getAlignment(), font, fontScale, color);
		}

		void hideText()
		{
			// something with a bg color?
		}
	};
};
