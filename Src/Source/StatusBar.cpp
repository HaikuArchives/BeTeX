/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef STATUS_BAR_H
#include "StatusBar.h"
#endif

#include <be/interface/Font.h>
#include "constants.h"

StatusBar::StatusBar(BRect frame) 
			:	DoubleBufferedView(frame, "StatusBar", B_FOLLOW_BOTTOM, B_WILL_DRAW),
				m_statusText("")			
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

StatusBar::~StatusBar()
{
}

void StatusBar::DrawContent(BView* backView)
{		
	BRect bounds = backView->Bounds();
	//set font
	BFont font(be_fixed_font);
	font.SetSize(12.0f);
	backView->SetFont(&font);
	
	backView->SetHighColor(ViewColor());
	backView->FillRect(Bounds());
	
	backView->SetHighColor(ColourConstants::K_WHITE);
	backView->StrokeLine(bounds.LeftTop(),bounds.RightTop());
	backView->StrokeLine(bounds.LeftTop(),bounds.LeftBottom());
	
	backView->SetHighColor(tint_color(ViewColor(),B_DARKEN_1_TINT));
	backView->StrokeLine(bounds.RightBottom(),BPoint(bounds.right,bounds.top + 1));
	backView->StrokeLine(bounds.RightBottom(),BPoint(bounds.left + 1,bounds.bottom));
	
	if(m_statusText.Length() != 0)
	{
		backView->SetHighColor(ColourConstants::K_BLACK);
		backView->SetLowColor(ViewColor());
		backView->MovePenTo(3.0f, bounds.bottom - 3.0f);
		backView->DrawString(m_statusText.String());
	}	
}

void StatusBar::SetText(BString statusText)
{
	m_statusText = statusText;
	Invalidate();
}

void StatusBar::SetText(const char* statusText)
{
	m_statusText = statusText;
	Invalidate();
}

BString StatusBar::Text()
{
	return m_statusText;
}
