/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef PREFS_LIST_VIEW_H
#include "PrefsListView.h"
#endif

#include "Constants.h"
#include "PrefsListItem.h"

PrefsListView::PrefsListView(BRect frame, BView* parent, BMessenger* messenger)
	: BListView(frame, "listview", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES,
		  B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE),
	  m_msgr(messenger),
	  m_parent(parent),
	  m_lastSelectedIndex(-1)
{
}

PrefsListView::~PrefsListView() {}
void
PrefsListView::SelectionChanged()
{
	if (m_lastSelectedIndex >= 0) {
		((PrefsListItem*)ItemAt(m_lastSelectedIndex))->Hide();
	}

	int32 current = CurrentSelection();
	if (current >= 0) {
		((PrefsListItem*)ItemAt(current))->Show();
		m_lastSelectedIndex = current;
	} else {
		m_lastSelectedIndex = -1;
	}
}

void
PrefsListView::Next(int32 increment)
{
	int32 items = CountItems();
	if (items > 1) {
		int32 current = CurrentSelection();
		if (current >= 0) {
			if (current + increment < items) {
				Select(current + increment);
			} else {
				// select last item in list
				Select(items - 1);
			}
		}
	}
}

void
PrefsListView::Prev(int32 decrement)
{
	int32 items = CountItems();
	if (items > 1) {
		int32 current = CurrentSelection();
		if (current >= 0) {
			if (current - decrement >= 0) {
				Select(current - decrement);
			} else {
				// select first item in list
				Select(0);
			}
		}
	}
}

void
PrefsListView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_MOUSE_WHEEL_CHANGED:
		{
			// We have a mouse wheel event when the focus is on the desktop.
			float delta_y;
			if (message->FindFloat("be:wheel_delta_y", &delta_y) == B_OK) {
				// Wrap-Around is taken care of too.
				if (delta_y > 0)  // move wheel down/back
				{
					Next();
				} else if (delta_y < 0)	 // move wheel up/forward
				{
					Prev();
				}
			}

		} break;
		default:
		{
			BListView::MessageReceived(message);
		} break;
	}
}

void
PrefsListView::MouseDown(BPoint point)
{
	BListView::MouseDown(point);
}
