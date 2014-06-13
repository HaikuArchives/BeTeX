/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef PREFS_LIST_ITEM_H
#include "PrefsListItem.h"
#endif

#include <be/interface/Font.h>
#include <iostream>

PrefsListItem::PrefsListItem(BView* prefsView, BView* backgroundView, BString label)
				:	BListItem(),
					m_prefsView(prefsView),
					m_backgroundView(backgroundView),
					m_label(label)
{
	m_backgroundView->AddChild(prefsView);
	m_prefsView->Hide();
}

PrefsListItem::~PrefsListItem()
{	
}

void PrefsListItem::Hide()
{
	if(!m_prefsView->IsHidden())
		m_prefsView->Hide();
}

void PrefsListItem::Show()
{
	if(m_prefsView->IsHidden())
		m_prefsView->Show();
}

void PrefsListItem::DrawItem(BView* owner, BRect frame, bool complete)
{
	rgb_color colour = owner->ViewColor();
	BFont font(be_plain_font);
	font.SetSize(12);
	
	if(IsSelected() || complete)
	{
		if(IsSelected())
		{
			BFont font2(be_bold_font);
			font2.SetSize(12);
			colour = ui_color(B_WINDOW_TAB_COLOR);
		
			owner->SetFont(&font2);
		}
		else
		{ 			
			colour = owner->ViewColor();
			owner->SetFont(&font);			
		}
		owner->SetHighColor(colour);
		owner->SetLowColor(colour);		
		owner->FillRect(frame);
	}
	else
	{
		owner->SetFont(&font);	
		owner->SetHighColor(colour);
		owner->SetLowColor(colour);	
		owner->FillRect(frame);
	}
	owner->MovePenTo(frame.left+4,frame.bottom-2);
	
	owner->SetHighColor(0,0,0);

	owner->SetLowColor(colour);
	owner->DrawString(m_label.String());	
}
