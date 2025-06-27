/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef DOC_VIEW_H
#include "DocView.h"
#endif

#include "BeTeXLogo.h"

/*
Default display : BeTeX logo
*/

DocView::DocView(BRect frame)
	: BView(frame, "DocView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BRect logoRect(0, 0, 246, 107);
	m_logo = new BBitmap(logoRect, B_RGB32);

	int numBits = (logoRect.IntegerWidth() + 1) * (logoRect.IntegerHeight() + 1) * 3;
	m_logo->ImportBits(K_BETEX_LOGO, numBits, 247 * 3, 0, B_RGB24_BIG);
}

DocView::~DocView()
{
	delete m_logo;
}

void
DocView::Draw(BRect r)
{
	MovePenTo(5, 5);
	DrawBitmap(m_logo);
}
