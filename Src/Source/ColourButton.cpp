/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOUR_BUTTON_H
#include "ColourButton.h"
#endif 

#include <be/app/Application.h>
#include <Screen.h>
#include <iostream>
#include "constants.h"

ColourButton::ColourButton(BRect frame, BMessage *message, rgb_color colour, BString label) 
				:	BControl(frame, "colourbutton", NULL, message, B_FOLLOW_NONE, B_WILL_DRAW),
					m_colourWindow(NULL),
					m_colour(colour),
					m_label(label),
					m_isDown(false),
					m_isOver(false),
					m_okToInvoke(false)
					
{	
	m_bitmap = new BBitmap(Bounds(),B_RGB32,true);
	m_bitmap->Lock();
	m_bitmapView = new BView(Bounds(),"view",B_FOLLOW_NONE, B_WILL_DRAW);
	m_bitmap->AddChild(m_bitmapView);
	m_bitmap->Unlock();	
}

ColourButton::~ColourButton()
{
}

void ColourButton::AttachedToWindow()
{
	BControl::AttachedToWindow();
	SetTarget(Parent());
}

void ColourButton::Draw(BRect updateRect)
{
	Render();
}

void ColourButton::Render()
{
	m_bitmap->Lock();

	BRect r = m_bitmapView->Bounds();	
	m_bitmapView->SetDrawingMode(B_OP_COPY);

	m_bitmapView->SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	m_bitmapView->FillRect(r);
	r = BRect(0,0,14,14);
	m_bitmapView->SetHighColor(m_colour);
	m_bitmapView->FillRect(r);
	
	if(m_isDown)
		m_bitmapView->SetHighColor(tint_color(m_colour,B_DARKEN_2_TINT));
	else
		m_bitmapView->SetHighColor(tint_color(m_colour,B_LIGHTEN_2_TINT));
	
	m_bitmapView->StrokeLine(BPoint(r.left,r.top),BPoint(r.left,r.bottom));
	m_bitmapView->StrokeLine(BPoint(r.left,r.top),BPoint(r.right,r.top));
	
	if(m_isDown)
		m_bitmapView->SetHighColor(tint_color(m_colour,B_DARKEN_1_TINT));
	else
		m_bitmapView->SetHighColor(tint_color(m_colour,B_LIGHTEN_1_TINT));


	if(m_isDown)
		m_bitmapView->SetHighColor(tint_color(m_colour,B_LIGHTEN_2_TINT));
	else
		m_bitmapView->SetHighColor(tint_color(m_colour,B_DARKEN_2_TINT));

	m_bitmapView->StrokeLine(BPoint(r.left,r.bottom),BPoint(r.right,r.bottom));
	m_bitmapView->StrokeLine(BPoint(r.right,r.top),BPoint(r.right,r.bottom));
	
	if(m_isDown)
		m_bitmapView->SetHighColor(tint_color(m_colour,B_LIGHTEN_1_TINT));
	else
		m_bitmapView->SetHighColor(tint_color(m_colour,B_DARKEN_1_TINT));
	
	BPoint point(r.RightBottom());
	
	font_height fh;
	BFont font(be_plain_font);
	font.SetSize(12.0f);
	font.GetHeight(&fh);
		
	m_bitmapView->MovePenTo(BPoint(point.x + 10.0f ,point.y - fh.descent));
	m_bitmapView->SetHighColor(0,0,0);
	m_bitmapView->DrawString(m_label.String());
	m_bitmapView->Sync();
	DrawBitmapAsync(m_bitmap, Bounds(), Bounds());
	Flush();
	Sync();	
	
	m_bitmap->Unlock();
}

void ColourButton::MouseDown(BPoint point)
{
	uint32 button;
	GetMouse(&point, &button);
	
	if(button == B_PRIMARY_MOUSE_BUTTON && IsEnabled())
	{
		m_isDown = true;
		m_okToInvoke = true;
		Render();
	}
}

void ColourButton::MouseUp(BPoint point)
{	
	m_isDown = false;
	Render();

	if(m_okToInvoke)
	{
		Invoke();
		m_okToInvoke = false;
	}
}

void ColourButton::MouseMoved(BPoint point, uint32 transit, const BMessage* message)
{
	uint32 button;
	GetMouse(&point, &button);
	
	switch(transit)
	{
		case B_ENTERED_VIEW:
		{	
			m_isOver = true;			
		}
		break;
		case B_EXITED_VIEW:
		{
			m_isOver = false;
			if (m_isDown)
			{
				m_isDown = false;
				Render();
			}
			
			m_okToInvoke = false;
			m_isDown = false;			
		}
		break;
	}
	
	Parent()->MouseMoved(point, transit, message);
}

void ColourButton::MessageReceived(BMessage* message)
{
	if(message->WasDropped())
	{
		rgb_color*	rgb;
		ssize_t		size;
		// if there is RGB color
		if( (message->FindData("RGBColor", B_RGB_COLOR_TYPE, (const void **)&rgb, &size) == B_OK))
		{
			m_colour = *rgb;
			Render();
			BControl::Invoke();
		}
	}
		
	switch(message->what)
	{
		case ColourPrefsConstants::K_COLOUR_WINDOW_QUIT:
		{
			m_colourWindow = NULL;//mmm?
		}
		break;
		case ColourPrefsConstants::K_COLOUR_PREFS_UPDATE:
		{
			rgb_color*	rgb;
			ssize_t		size;
			// if there is RGB color
			if( (message->FindData("color", B_RGB_COLOR_TYPE, (const void **)&rgb, &size) == B_OK))
			{
				m_colour = *rgb;
				Render();
				BControl::Invoke();
			}		
		}
		break;
		default:
			BControl::MessageReceived(message);
		break;
	}
}

status_t ColourButton::Invoke(BMessage* message = NULL)
{	
	//start up chooser window
	if(m_colourWindow != NULL)
	{
		m_colourWindow->Activate(true);
		return B_OK;
	}

	BPoint point;
	uint32 buttons;
	GetMouse(&point, &buttons);	
	ConvertToScreen(&point);
	
	point.x -= 30.0f;
	point.y -= 30.0f;
	float width = 350.0f,
		  height = 115.0f;	

	BRect colourWindowFrame(point.x, point.y, point.x + width, point.y + height);	
	m_colourWindow = new ColourWindow(colourWindowFrame, new BMessenger(this), m_colour);
	m_colourWindow->Show();
	
	return B_OK;
}

void ColourButton::SetValue(rgb_color colour)
{	
	m_colour = colour;
	Render();	
}

rgb_color ColourButton::Value()
{
	return m_colour;
}
