/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef PREFS_LIST_VIEW_H
#define PREFS_LIST_VIEW_H

#include <be/app/Message.h>
#include <be/interface/ListView.h>
#include <be/interface/Point.h>
#include <be/interface/Rect.h>
#include <be/interface/View.h>

class PrefsListView : public BListView {
public:
	PrefsListView(BRect frame, BView* parent, BMessenger* messenger);
	virtual ~PrefsListView();

	virtual void SelectionChanged();
	virtual void MessageReceived(BMessage* message);
	virtual void MouseDown(BPoint point);

	void Next(int32 increment = 1);
	void Prev(int32 decrement = 1);

private:
	BMessenger* m_msgr;
	BView* m_parent;
	int32 m_lastSelectedIndex;
};

#endif
