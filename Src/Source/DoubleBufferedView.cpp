/*******************************************************************************
 * Copyright (c) 2005 Tim de Jong                                              *
 *                                                                             *
 * All rights reserved.                                                        *
 * Distributed under the terms of the MIT License.                             *
 ******************************************************************************/
#ifndef DOUBLE_BUFFERED_VIEW
#include "DoubleBufferedView.h"
#endif

#include <Window.h>
#include <iostream>
/**	Constructor
 */
DoubleBufferedView::DoubleBufferedView(
	BRect frame, const char* name, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags | B_FRAME_EVENTS)
{
	m_backBitmap = new BBitmap(Bounds(), B_RGB32, true);
	m_backView = new BView(Bounds(), "backView", B_FOLLOW_ALL, flags);
}

/**	Destructor
 */
DoubleBufferedView::~DoubleBufferedView()
{
	delete m_backBitmap;
}

/**	Hook function: called when the view has been attached to a window.
	Overridden here to add rendering of the bitmap to its functionality.
 */
void
DoubleBufferedView::AttachedToWindow()
{
	m_backBitmap->Lock();
	m_backBitmap->AddChild(m_backView);
	RenderView();
	m_backBitmap->Unlock();
}

/** Hook function: called when the the view has been resized. Overriden here
	to also resize the backbuffer in case of a view resize.
	@param width, the new view width.
	@param height, the new view height.
*/
void
DoubleBufferedView::FrameResized(float width, float height)
{
	// First make sure our background bitmap is large enough for our image.
	if (m_backBitmap) {
		m_backBitmap->RemoveChild(m_backView);
		delete m_backBitmap;
	}
	m_backBitmap = new BBitmap(BRect(0, 0, width, height), B_RGB32, true);
	m_backView->ResizeTo(width, height);

	// RenderView();
	m_backBitmap->AddChild(m_backView);
}

/** Hook function: draws the view content.
	@param updateRect,
 */
void
DoubleBufferedView::Draw(BRect updateRect)
{
	if (!Parent()) {
		m_backView->Window()->Lock();
		BRect bounds = m_backView->Bounds();
		m_backView->Window()->Unlock();

		float newWidth = updateRect.right;
		float newHeight = updateRect.bottom;
		if (newHeight != bounds.Height() || newWidth != bounds.Width()) {
			FrameResized(newWidth, newHeight);
		}
		RenderView();
	}
}

/** Hook function: refreshes the view content.
 */
void
DoubleBufferedView::Invalidate()
{
	RenderView();
}

/** This function renders the content of the view, using a double buffer.
	This prevents the flickering of the view. The function calls DrawContent
	to paint the view contents.
 */
void
DoubleBufferedView::RenderView()
{
	// Buffered draw, lock the bitmap, draw to the bitmap, then render bitmap once
	m_backBitmap->Lock();
	// Draw content here, the DrawContent method will be implemented by the subclasses.
	DrawContent(m_backView);
	// Sync the view, its very important we do this
	m_backView->Sync();
	DrawBitmapAsync(m_backBitmap, m_backView->Bounds(), m_backView->Bounds());
	// Unlock the bitmap again
	m_backBitmap->Unlock();
}

BBitmap*
DoubleBufferedView::GetBufferBitmap()
{
	return m_backBitmap;
}
