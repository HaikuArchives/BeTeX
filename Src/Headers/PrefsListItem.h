/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef PREFS_LIST_ITEM_H
#define PREFS_LIST_ITEM_H

#include <be/interface/ListItem.h>
#include <be/interface/View.h>
#include <be/support/String.h>

class PrefsListItem : public BListItem {
public:
	PrefsListItem(BView* prefsView, BView* bView, BString label);
	virtual ~PrefsListItem();

	virtual void DrawItem(BView* owner, BRect frame, bool complete = false);
	void Hide();
	void Show();
	BMessenger* GetTarget();

private:
	BView *m_prefsView, *m_backgroundView;
	BString m_label;
};
#endif
