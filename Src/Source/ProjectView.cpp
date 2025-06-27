/*******************************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski                             *
 *                                                                             *
 * All rights reserved.                                                        *
 * Distributed under the terms of the MIT License.                             *
 ******************************************************************************/
#ifndef PROJECT_VIEW_H
#include "ProjectView.h"
#endif

#include <be/app/Messenger.h>
#include <be/interface/Window.h>
#include "Constants.h"
#include "ProjectItem.h"

ProjectView::ProjectView(BRect frame, BScrollView* dView)
	: BListView(frame, "listview", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES,
		  B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE),
	  m_docView(dView),
	  m_lastSelectedIndex(-1),
	  m_isDocViewHidden(false)
{
}

ProjectView::~ProjectView() {}

void
ProjectView::SelectionChanged()
{
	if (m_lastSelectedIndex >= 0) {
		ProjectItem* selected = dynamic_cast<ProjectItem*>(ItemAt(m_lastSelectedIndex));
		selected->HideTextView();
	}

	BString title;
	title << "BeTeX";

	int32 current = CurrentSelection();
	if (current >= 0) {
		ProjectItem* selected = dynamic_cast<ProjectItem*>(ItemAt(current));

		title << " - " << selected->Label();
		Window()->SetTitle(title.String());

		// send message to owner, update the statusbar and the textview shortcuts
		BMessenger msgr(Window());
		msgr.SendMessage(new BMessage(ToolbarConstants::K_UPDATE_TEXTVIEW_SHORTCUTS));
		msgr.SendMessage(new BMessage(InterfaceConstants::K_UPDATE_STATUSBAR));

		if (!m_isDocViewHidden) {
			m_ownerSplitPane->AddChildTwo(m_docView, true, false);
			m_isDocViewHidden = true;
		}
		selected->ShowTextView();
		m_lastSelectedIndex = current;
	} else {
		m_lastSelectedIndex = -1;
		//???
		if (m_isDocViewHidden) {
			m_ownerSplitPane->AddChildTwo(m_docView, true, true);
			// wtf???
			if (CountItems() == 2) {
				Window()->SetTitle(title.String());
			}
			m_isDocViewHidden = false;
		}
		// send message to owner, update the statusbar
		BMessenger msgr(Window());
		msgr.SendMessage(new BMessage(InterfaceConstants::K_UPDATE_STATUSBAR));

		Window()->SetTitle(title.String());
	}
	ScrollToSelection();
}

void
ProjectView::MouseMoved(BPoint point, uint32 transit, const BMessage* msg)
{
	switch (transit) {
		case B_ENTERED_VIEW:
		{
			MakeFocus(true);
		} break;
		case B_EXITED_VIEW:
		{
			MakeFocus(false);
		} break;
	}
}

void
ProjectView::SetSplitPane(SplitPane* owner)
{
	m_ownerSplitPane = owner;
}

void
ProjectView::MouseDown(BPoint point)
{
	GetMouse(&point, &m_button);
	BListView::MouseDown(point);
}

void
ProjectView::Next(int32 increment)
{
	int32 items = CountItems();
	if (items > 1) {
		int32 current = CurrentSelection();
		if (current >= 0) {
			if (current + increment < items) {
				Select(current + increment);
			} else {
				Select(items - 1);
			}
		}
	}
}

void
ProjectView::Prev(int32 decrement)
{
	int32 items = CountItems();
	if (items > 1) {
		int32 current = CurrentSelection();
		if (current >= 0) {
			if (current - decrement >= 0) {
				Select(current - decrement);
			} else {
				Select(0);
			}
		}
	}
}

void
ProjectView::KeyDown(const char* bytes, int32 numBytes)
{
	switch (bytes[0]) {
		case B_HOME:
		{
			int32 items = CountItems();
			if (items > 0)
				Select(0);
		} break;
		case B_END:
		{
			int32 items = CountItems();
			if (items > 0)
				Select(items - 1);
		} break;
		case B_PAGE_UP:
		{
			int32 dec = NoVisibleItems();
			Prev(dec);
		} break;
		case B_PAGE_DOWN:
		{
			int32 inc = NoVisibleItems();
			Next(inc);
		} break;
		case B_UP_ARROW:
			Prev(1);
			break;
		case B_DOWN_ARROW:
			Next(1);
			break;
		case B_DELETE:
		case B_BACKSPACE:
			Window()->PostMessage(MenuConstants::K_MENU_FILE_CLOSE);
			break;
		default:
			BListView::KeyDown(bytes, numBytes);
			break;
	}
}

int32
ProjectView::NoVisibleItems()
{
	BRect bounds = Bounds();
	int32 noitems = 0;
	for (int32 i = 0; i < CountItems(); i++) {
		if (bounds.Contains(ItemFrame(i)))
			noitems++;
	}
	return noitems;
}


bool
updateItem(BListItem* item)
{
	ProjectItem* it = (ProjectItem*)item;
	it->ChildView()->UpdateColors();
	it->ChildView()->UpdateFontSize();
	return false;
}

void
ProjectView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_MOUSE_WHEEL_CHANGED:
		{
			// We have a mouse wheel event when the focus is on the desktop.
			float delta_y;
			if (message->FindFloat("be:wheel_delta_y", &delta_y) == B_OK) {
				int32 boost = 1;
				if (modifiers() & B_OPTION_KEY)
					boost = NoVisibleItems();
				// Wrap-Around is taken care of too.
				if (delta_y > 0) {
					// move wheel down/back
					Next(boost);
				} else if (delta_y < 0) {
					// move wheel up/forward
					Prev(boost);
				}
			}
		}
		case B_OBSERVER_NOTICE_CHANGE:
		{
			if (message->GetInt32(B_OBSERVE_ORIGINAL_WHAT, 0)
				== (int32)PrefsConstants::K_PREFS_UPDATE) {
				DoForEach(updateItem);
			}
		} break;
		default:
			BListView::MessageReceived(message);
			break;
	}
}
