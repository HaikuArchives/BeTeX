/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef PROJECT_ITEM_H
#include "ProjectItem.h"
#endif

#include <be/interface/Font.h>
#include "constants.h"

ProjectItem::ProjectItem(SplitPane* splitPane, BString label, BView *childView) 
				:	BListItem(),
					m_splitPane(splitPane),
					m_childView(childView),
					m_label(label),
					m_needToSave(false),
					m_hasHome(false)
{
	m_splitPane->AddChildTwo(m_childView, false, false);	
}

ProjectItem::~ProjectItem()
{	
	m_splitPane->RemoveChild(m_childView);
	delete m_childView;
}

void ProjectItem::DrawItem(BView* owner, BRect frame, bool complete)
{
	rgb_color colour = owner->ViewColor();	
	float fontSize = 12.0f;
	if(IsSelected() || complete)
	{
		if(IsSelected())
		{
			BFont font(be_bold_font);
			font.SetSize(fontSize);
			if(m_needToSave)
			{
				font.SetFace(B_ITALIC_FACE|B_BOLD_FACE);
				colour = ui_color(B_WINDOW_TAB_COLOR);	
			}
			else
			{
				colour = ui_color(B_WINDOW_TAB_COLOR);
			}
			owner->SetFont(&font);
		}
		else
		{ 
			BFont font(be_plain_font);
			font.SetSize(fontSize);
			if(m_needToSave)
			{	
				font.SetFace(B_ITALIC_FACE);
				colour = owner->ViewColor();
			}
			else
			{
				colour = owner->ViewColor();
			}
			owner->SetFont(&font);			
		}
		owner->SetHighColor(colour);
		owner->SetLowColor(colour);
		owner->FillRect(frame);
	}
	else
	{
		BFont font(be_plain_font);
		font.SetSize(fontSize);
		if(m_needToSave)
		{	
			font.SetFace(B_ITALIC_FACE);
		}
		owner->SetFont(&font);	
		owner->SetHighColor(colour);
		owner->SetLowColor(colour);	
		owner->FillRect(frame);
	}
	owner->MovePenTo(frame.left + 4.0f, frame.bottom - 2.0f);
	
	if(IsEnabled())
	{
		owner->SetHighColor(0,0,0);
	}
	else
	{
		owner->SetHighColor(0,0,0);				
	}
	
	if(m_needToSave)
	{			
		owner->SetHighColor(ColourConstants::K_UNSAVED_TEXT_COLOUR);
	}
	owner->SetLowColor(colour);
	owner->DrawString(m_label.String());	
}


void ProjectItem::ShowTextView()
{
	m_splitPane->AddChildTwo(m_childView, true, true);		
}

void ProjectItem::HideTextView()
{
	m_splitPane->AddChildTwo(m_childView, true, false);
}

BView* ProjectItem::ChildView()
{
	return m_childView;
}

void ProjectItem::SetSaveNeeded(bool needsSave)
{
	m_needToSave = needsSave;
}

bool ProjectItem::IsSaveNeeded()
{
	return m_needToSave;
}

void ProjectItem::SetHomely(bool hasHome)
{
	m_hasHome = hasHome;
}

bool ProjectItem::IsHomely()
{
	return m_hasHome;
}
