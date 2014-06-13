/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOUR_VIEW_H
#define COLOUR_VIEW_H

#include <be/interface/Rect.h>
#include <be/interface/View.h>
#include <be/interface/Bitmap.h>

/** About this class: This class is a simple subclassed BView, 
	that uses a view to draw into a Bitmap, so it's drawing is rendered flicker-free.
*/
class ColourView : public BView
{
	public:
		ColourView(BRect frame);
		virtual				~ColourView();
		
		virtual void		Draw(BRect drawRect);
		void				Render();
		void				SetColor(rgb_color colour);

	protected:
		BBitmap*			m_bitmap;
		BView*				m_bitmapView;
		
		rgb_color			m_colour;
};

#endif
