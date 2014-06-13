/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOUR_VIEW_H
#include "ColourView.h"
#endif

#include <be/interface/Point.h>

ColourView::ColourView(BRect frame)
			:	BView(frame,"colorView",B_FOLLOW_NONE,B_WILL_DRAW)
{
	m_bitmap = new BBitmap(Bounds(),B_RGB32,true);
	m_bitmap->Lock();
	m_bitmapView = new BView(Bounds(),"view",B_FOLLOW_NONE, B_WILL_DRAW);
	m_bitmap->AddChild(m_bitmapView);
	m_bitmap->Unlock();
}

ColourView::~ColourView()
{
//	delete bitmap;
//	delete bView;
}
void ColourView::SetColor(rgb_color colour)
{
	m_colour = colour;
}

void ColourView::Draw(BRect drawRect)
{
	Render();
}

void ColourView::Render()
{
	m_bitmap->Lock();
	BRect bitmapBounds = m_bitmap->Bounds();
	m_bitmapView->SetHighColor(m_colour);
	m_bitmapView->FillRect(bitmapBounds);
	m_bitmapView->SetHighColor(tint_color(m_colour,B_LIGHTEN_2_TINT));

	m_bitmapView->StrokeLine(BPoint(bitmapBounds.left,bitmapBounds.top), BPoint(bitmapBounds.left,bitmapBounds.bottom));
	m_bitmapView->StrokeLine(BPoint(bitmapBounds.left,bitmapBounds.top), BPoint(bitmapBounds.right,bitmapBounds.top));
	
	m_bitmapView->SetHighColor(tint_color(m_colour,B_DARKEN_2_TINT));

	m_bitmapView->StrokeLine(BPoint(bitmapBounds.left,bitmapBounds.bottom), BPoint(bitmapBounds.right,bitmapBounds.bottom));
	m_bitmapView->StrokeLine(BPoint(bitmapBounds.right,bitmapBounds.top), BPoint(bitmapBounds.right,bitmapBounds.bottom));

	m_bitmapView->Sync();
	DrawBitmapAsync(m_bitmap, Bounds(), Bounds());
	Flush();
	Sync();
	m_bitmap->Unlock();
}
