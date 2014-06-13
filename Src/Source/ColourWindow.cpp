/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOUR_WINDOW_H
#include "ColourWindow.h"
#endif

#include <be/interface/StringView.h>
#include <be/interface/ScrollView.h>
#include <be/interface/Font.h>
#include "constants.h"

FastColour::FastColour(ColourView* theWell, BPoint leftTop, color_control_layout matrix, float cellSide, const char* name, BMessage* message = NULL, bool bufferedDrawing = false)
 			:	BColorControl(leftTop,matrix,cellSide,name,message,bufferedDrawing)
{
	well = theWell;
}

void FastColour::SetValue(int32 v)
{
	BColorControl::SetValue(v);
	well->SetColor(ValueAsColor());
	if(Window())
		well->Render();
}

void FastColour::SetValue(rgb_color colour)
{
	BColorControl::SetValue(colour);
}
//============================================================================
ColourWindow::ColourWindow(BRect frame, BMessenger* messenger, rgb_color colour) 
				:	BWindow(frame, "Pick a Colour" B_UTF8_ELLIPSIS, B_FLOATING_WINDOW, B_NOT_ZOOMABLE|B_NOT_RESIZABLE|B_ASYNCHRONOUS_CONTROLS|B_NOT_CLOSABLE),
					m_msgr(messenger)
{
	SetFeel(B_NORMAL_WINDOW_FEEL);
	
	m_parent = new BView(Bounds(),"parent",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	m_parent->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(m_parent);

	m_colourView = new ColourView(BRect(287,10,339,62));
	m_colourView->SetColor(colour);
	m_parent->AddChild(m_colourView);

	m_ctrlBox = new FastColour(m_colourView, BPoint(10,10), B_CELLS_32x8, 4.0f, "ctrlbox", new BMessage(ColourPrefsConstants::K_COLOUR_PREFS_UPDATE), true);
	m_parent->AddChild(m_ctrlBox);
	m_ctrlBox->SetValue(colour);

	int width=50;
	int offset = width + 20;
	
	BRect btnRect(326-width,62+15,326+10,62+35);
	
	m_okButton = new BButton(btnRect,"fOk","Ok",new BMessage(ColourPrefsConstants::K_COLOUR_PREFS_OK));
	m_parent->AddChild(m_okButton);
	m_okButton->MakeDefault(true);

	btnRect.OffsetBy(-offset,0);
	m_cancelButton = new BButton(btnRect,"fCancel","Cancel",new BMessage(ColourPrefsConstants::K_COLOUR_PREFS_CANCEL));
	m_parent->AddChild(m_cancelButton);	
}

ColourWindow::~ColourWindow()
{
	delete m_msgr;
}

void ColourWindow::MessageReceived(BMessage *message)
{
	if (message->WasDropped())
	{
		rgb_color*	rgb;
		ssize_t		size;
		// if there is RGB color
		if( (message->FindData("RGBColor", B_RGB_COLOR_TYPE, (const void **)&rgb, &size) == B_OK))
		{
			m_ctrlBox->SetValue(*rgb);
			m_colourView->SetColor(*rgb);
			m_colourView->Render();			
		}
		return;
	}
	
	switch(message->what)
	{
		case ColourPrefsConstants::K_COLOUR_PREFS_UPDATE:
		{
			rgb_color updated_colour = m_ctrlBox->ValueAsColor();
			m_colourView->SetColor(updated_colour);
			m_colourView->Render();
		}
		break;		
		case ColourPrefsConstants::K_COLOUR_PREFS_OK:
		{
			BMessage mymsg(ColourPrefsConstants::K_COLOUR_PREFS_UPDATE);
			rgb_color updated_colour = m_ctrlBox->ValueAsColor();
			mymsg.AddData("color",B_RGB_COLOR_TYPE,&updated_colour,sizeof(rgb_color));
			
			m_msgr->SendMessage(&mymsg);
			Quit();
		}
		break;
		case ColourPrefsConstants::K_COLOUR_PREFS_CANCEL:
		{
			Quit();		
		}
		break;		
		default:
			BWindow::MessageReceived(message);
		break;
	}
}

void ColourWindow::Quit()
{
	m_msgr->SendMessage(new BMessage(ColourPrefsConstants::K_COLOUR_WINDOW_QUIT));
	BWindow::Quit();
}
