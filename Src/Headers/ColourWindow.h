/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOUR_WINDOW_H
#define COLOUR_WINDOW_H

#include <be/app/Messenger.h>
#include <be/app/Message.h>
#include <be/interface/Rect.h>
#include <be/interface/Point.h>
#include <be/interface/Window.h>
#include <be/interface/View.h>
#include <be/interface/Button.h>
#include <be/interface/ColorControl.h>
#include <TypeConstants.h>
#include "ColourView.h"

class FastColour : public BColorControl
{
	public:
		FastColour(ColourView *theWell, BPoint leftTop, color_control_layout matrix, float cellSide, const char* name, BMessage* message = NULL, bool bufferedDrawing = false);
		
		void				SetValue(int32 v);
		void				SetValue(rgb_color colour);
	
	private:
	 	ColourView			*well;
};

class ColourWindow : public BWindow
{
	public:
		ColourWindow(BRect frame, BMessenger* messenger, rgb_color colour);
		virtual				~ColourWindow();
		
		virtual void		MessageReceived(BMessage *message);
		void				Quit();
		
	private:
		BMessenger			*m_msgr;
		
		BButton				*m_okButton,
							*m_cancelButton;	
							
		BView				*m_parent;
		FastColour			*m_ctrlBox;
		ColourView			*m_colourView;
};

#endif
