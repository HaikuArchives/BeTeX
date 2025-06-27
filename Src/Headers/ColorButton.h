/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

#include <be/interface/Bitmap.h>
#include <be/interface/Control.h>
#include <be/interface/Rect.h>
#include <be/interface/View.h>
#include <be/support/String.h>
#include "ColorWindow.h"

class ColorButton : public BControl {
public:
	ColorButton(BRect frame, BString name, BMessage* message, rgb_color color, BString label);
	virtual ~ColorButton();

	virtual void MessageReceived(BMessage* message);
	virtual void AttachedToWindow();
	virtual void Draw(BRect updateRect);
	virtual void MouseDown(BPoint point);
	virtual void MouseUp(BPoint point);
	virtual void MouseMoved(BPoint point, uint32 transit, const BMessage* message);

	virtual status_t Invoke(BMessage* message = NULL);
	void Render();

	void SetValue(rgb_color color);
	rgb_color Value();

private:
	BBitmap* m_bitmap;
	BView* m_bitmapView;
	ColorWindow* m_colorWindow;
	rgb_color m_color;

	BString m_label;

	bool m_isDown, m_isOver, m_okToInvoke;
};

#endif
