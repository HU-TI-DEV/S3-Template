#pragma once
#include <assert.h>
#include "crt_typesAndConstants.h"
#include "crt_IWidget.h"

namespace crt
{
	template <size_t MaxNofChildren> 
	class WidgetProps : public IWidget
	{
	private:
		// The four values below will be filled in by the 
		// parent, after adding the widget to that parent.
		IDisplay* _display;
		Vec2 _globPosOfParent;
		Vec2 _sizeOfParent;

		Vec2 _locPos;
		Vec2 _locPosPix;					// In pixels wrt origin of parent
		CoordType _coordTypeLocPos;
		Vec2 _size;
		Vec2 _sizePix;                   
		CoordType _coordTypeSize;
		Alignment _alignment;
		bool _bVisible;
		bool _bEnabled;
		int _nofChildren;
		IWidget* _arChildren[MaxNofChildren];

	public:
		WidgetProps(const Vec2& locPos, CoordType coordTypeLocPos, 
			        const Vec2& size, CoordType coordTypeSize, Alignment alignment) : 
			_display(nullptr),            // _display will be set when added to parent.
			_globPosOfParent(0,0), _sizeOfParent(0,0),
			_locPos(locPos), _coordTypeLocPos(coordTypeLocPos),
			_size(size), _coordTypeSize(coordTypeSize), _alignment(alignment),
			_bVisible(false), _bEnabled(true), _nofChildren(0)
		{
			for (int i = 0; i < MaxNofChildren; i++)
			{
				_arChildren[i] = nullptr;
			}

			// The data below will be automatically calculated
			// after the parent has initialised _sizeOfParent and _globPosOfParent.
			// 
			//_sizePix	  = recalcSizePix();
			//_locPosPix  = recalcLocPosPix();
		}

		// build a tree from back to forth: root is drawn first.
		/*override*/ bool addChildWidget(IWidget& widget)
		{
			if (_nofChildren == MaxNofChildren)
			{
				ESP_LOGI("no place for an additional child widget", "!!!!!!!!!!");
				return false;
			}
			_arChildren[_nofChildren++] = &widget; 

			assert(_display != nullptr);
			widget.setDisplay(*_display);
			widget.setSizeOfParent(_sizePix);						    // pass down own size
			widget.setGlobPosOfParent(_globPosOfParent + _locPosPix); // pass down own globpos

			return true;
		}

		/*override*/ IDisplay* getDisplay()
		{
			return _display;
		}
		
		/*override*/ void setDisplay(IDisplay& display)
		{
			_display = &display;

			// Do it recursively, allowing a page to be moved from
			// one display to another.
			for (int i = 0; i < _nofChildren; i++)
			{
				_arChildren[i]->setDisplay(display);
			}
		}

		// always in pixels. always in abs.
		/*override*/ Vec2 getGlobPosOfParent()
		{
			return _globPosOfParent;
		}

		// This function is normally called by the parent widget.
		// It thus passes down its absolute coordinates in pixels wrt the top left of the screen.
		/*override*/ void setGlobPosOfParent(const Vec2& globPosOfParent)
		{
			if (_globPosOfParent != globPosOfParent)
			{
				_globPosOfParent = globPosOfParent;
				_locPosPix = recalcLocPosPix();
				updateNewGlobPosToChildren();
			}
		}

		// always in pixels
		/*override*/ Vec2 getSizeOfParent()
		{
			return _sizeOfParent;
		}

		// This function is normally called by the parent widget.
		// It thus passes down its absolute coordinates in pixels wrt the top left of the screen.
		/*override*/ void setSizeOfParent(const Vec2& sizeOfParent)
		{
			if (_sizeOfParent != sizeOfParent)
			{
				_sizeOfParent = sizeOfParent;

				Vec2 newSizePix   = recalcSizePix();
				Vec2 newLocPosPix = recalcLocPosPix();

				if (newSizePix != _sizePix)
				{
					_sizePix = newSizePix;
					updateNewSizeToChildren();
				}

				if (newLocPosPix != _locPosPix)
				{
					_locPosPix = newLocPosPix;
					updateNewGlobPosToChildren();
				}
			}
		}

		// CoordType can be Pixels or Promillage
		/*override*/ Vec2 getLocPos()
		{
			return _locPos;
		}

		// CoordType can be Pixels or Promillage
		/*override*/ void setLocPos(const Vec2& locPos, const CoordType coordTypeLocPos, const Alignment alignment)
		{
			_alignment = alignment;
			_coordTypeLocPos = coordTypeLocPos;
			_locPos = locPos;

			Vec2 newLocPosPix = recalcLocPosPix();

			if (newLocPosPix != _locPosPix)
			{
				_locPosPix = newLocPosPix;
				updateNewGlobPosToChildren();
			}
		}

		// CoordType can be Pixels or Promillage
		/*override*/ Vec2 getSize()
		{
			return _size;
		}

		// CoordType can be Pixels or Promillage
		/*override*/ void setSize(const Vec2& size, const CoordType coordTypeSize)
		{
			_size = size;
			
			Vec2 newSizePix = recalcSizePix();
			
			if (newSizePix != _sizePix)
			{
				_sizePix = newSizePix;
				updateNewSizeToChildren();
			}
		}

		/*override*/ Vec2 getLocPosPix()
		{
			return _locPosPix;
		}

		/*override*/ Vec2 getSizePix()
		{
			return _sizePix;
		}

		/*override*/ Alignment getAlignment()
		{
			return _alignment;
		}

		// show immediately draws it. hide immediately undraws it.
		// note: WidgetProps only serves as a node in the display-tree.
		//       It conains the props and links to children.
		//       The actual (un-)display functionality is to be implemented
		//       by the specific widget class (like TouchScreenButton),
		//       which is derived from IWidget as well.
		/*override*/ void show(bool bIncludeChildren)
		{
			_bVisible = true;
			if (bIncludeChildren)
			{
				for (int i = 0; i < _nofChildren; i++)
				{
					_arChildren[i]->show(true);
				}
			}
		}

		// show immediately draws it. hide immediately undraws it.
		/*override*/ void hide(bool bIncludeChildren)
		{
			_bVisible = false;
			if (bIncludeChildren)
			{
				for (int i = 0; i < _nofChildren; i++)
				{
					_arChildren[i]->hide(true);
				}
			}
		}

		/*override*/ bool isShown()
		{
			return _bVisible;
		}

		// enable/disable is about the functional aspect of 
		// note: WidgetProps only serves as a node in the display-tree.
		//       It conains the props and links to children.
		//       The actual enable/disable functionality of the related
		//       widget class (like TouchScreenButton) must be implemented
		//       by that widget class itself (which is derived from IWidget as well).
		/*override*/ void enable(bool bIncludeChildren)
		{
			_bEnabled = true;
			if (bIncludeChildren)
			{
				for (int i = 0; i < _nofChildren; i++)
				{
					_arChildren[i]->enable(true);
				}
			}
		}

		// a widget: wether it continues to responde to input,
		/*override*/ void disable(bool bIncludeChildren)
		{
			_bEnabled = false;
			if (bIncludeChildren)
			{
				for (int i = 0; i < _nofChildren; i++)
				{
					_arChildren[i]->disable(true);
				}
			}
		}

		// for instance.
		/*override*/ bool isEnabled()
		{
			return _bEnabled;
		}

	private:
		Vec2 recalcLocPosPix()
		{
			Vec2 newLocPosPix;
			if (_coordTypeLocPos == CoordType::Pixels)
			{
				newLocPosPix = _locPos;
			}
			else
			{
				// Note: sizeOfParent is in pix.
				// size is in Promillages, in this case.
				newLocPosPix = (_locPos * _sizeOfParent) / 1000;
			}
			return newLocPosPix;
		}

		Vec2 recalcSizePix()
		{
			Vec2 newSizePix;
			if (_coordTypeSize == CoordType::Pixels)
			{
				newSizePix = _size;
			}
			else
			{
				// Note: sizeOfParent is in pix.
				// size is in Promillages, in this case.
				newSizePix = (_size * _sizeOfParent) / 1000;
			}
			return newSizePix;
		}

		void updateNewGlobPosToChildren()
		{
			Vec2 newGlobPosPix = _globPosOfParent + _locPosPix;
			for (int i = 0; i < _nofChildren; i++)
			{
				_arChildren[i]->setGlobPosOfParent(newGlobPosPix);
			}
		}

		void updateNewSizeToChildren()
		{
			for (int i = 0; i < _nofChildren; i++)
			{
				_arChildren[i]->setSizeOfParent(_sizePix);
			}
		}
	};
};
