/*******************************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski                             *
 *                                                                             *
 * All rights reserved.                                                        *
 * Distributed under the terms of the MIT License.                             *
 ******************************************************************************/
#ifndef STATUS_BAR_H
#include "StatusBar.h"
#endif

#include <be/interface/Font.h>
#include <be/interface/Region.h>
#include <be/interface/Window.h>
#include <iostream>


#include "Constants.h"

StatusBar::StatusBar(BRect frame)
	: /*DoubleBuffered*/ BView(frame, "StatusBar", B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS),
	  m_statusText("")
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_NO_VERTICAL));
	SetExplicitSize(BSize(Bounds().Width(), Bounds().Height()));
}

StatusBar::~StatusBar() {}

void
StatusBar::Draw(BRect updateRect)
{
	BRect bounds = Bounds();

	BWindow* window = Window();
	window->Lock();
	BRect wBounds = window->Bounds();

	if (window && wBounds.Width() != bounds.Width()) {
		ResizeTo(wBounds.Width(), bounds.Height());

		Invalidate();
		window->Unlock();
		return;
	}

	// set font
	BFont font(be_fixed_font);
	font.SetSize(12.0f);
	this->SetFont(&font);

	this->SetHighColor(ViewColor());
	this->FillRect(Bounds());

	this->SetHighColor(ColorConstants::K_WHITE);
	this->StrokeLine(bounds.LeftTop(), updateRect.RightTop());
	this->StrokeLine(bounds.LeftTop(), bounds.LeftBottom());

	this->SetHighColor(tint_color(ViewColor(), B_DARKEN_1_TINT));
	this->StrokeLine(bounds.RightBottom(), BPoint(bounds.right, bounds.top + 1));
	this->StrokeLine(bounds.RightBottom(), BPoint(bounds.left + 1, bounds.bottom));
	if (m_statusText.Length() != 0) {
		this->SetHighColor(ColorConstants::K_BLACK);
		this->SetLowColor(ViewColor());
		this->MovePenTo(3.0f, bounds.bottom - 3.0f);
		this->DrawString(m_statusText.String());
	}
	window->Unlock();
}

void
StatusBar::SetText(BString statusText)
{
	m_statusText = statusText;
	Invalidate();
}

void
StatusBar::SetText(const char* statusText)
{
	m_statusText = statusText;
	Invalidate();
}

BString
StatusBar::Text()
{
	return m_statusText;
}
