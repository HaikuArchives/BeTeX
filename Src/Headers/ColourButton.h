/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOUR_BUTTON_H
#define COLOUR_BUTTON_H

#include <be/interface/Rect.h>
#include <be/interface/Bitmap.h>
#include <be/interface/View.h>
#include <be/interface/Control.h>
#include <be/support/String.h>
#include "ColourWindow.h"

class ColourButton : public BControl
{
	public:
		ColourButton(BRect frame, BMessage* message, rgb_color colour, BString label);
		virtual				~ColourButton();
		
		virtual void		MessageReceived(BMessage* message);
		virtual void		AttachedToWindow();
		virtual void		Draw(BRect updateRect);
		virtual void		MouseDown(BPoint point);
		virtual void		MouseUp(BPoint point);
		virtual void		MouseMoved(BPoint point, uint32 transit, const BMessage* message);

		virtual status_t	Invoke(BMessage* message = NULL);		
		void				Render();
				
		void				SetValue(rgb_color colour);
		rgb_color			Value();
		
	private:
		BBitmap				*m_bitmap;
		BView				*m_bitmapView;
		ColourWindow		*m_colourWindow;
		rgb_color			m_colour;
		
		BString				m_label;
				
		bool				m_isDown,
							m_isOver,
							m_okToInvoke;
};

#endif
