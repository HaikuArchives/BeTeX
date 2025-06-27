/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOR_VIEW_H
#define COLOR_VIEW_H

#include <be/interface/Bitmap.h>
#include <be/interface/Rect.h>
#include <be/interface/View.h>

/** About this class: This class is a simple subclassed BView,
	that uses a view to draw into a Bitmap, so it's drawing is rendered flicker-free.
*/
class ColorView : public BView {
public:
	ColorView(BRect frame);
	virtual ~ColorView();

	virtual void Draw(BRect drawRect);
	void Render();
	void SetColor(rgb_color color);

protected:
	BBitmap* m_bitmap;
	BView* m_bitmapView;

	rgb_color m_color;
};

#endif
