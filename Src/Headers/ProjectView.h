/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef PROJECT_VIEW_H
#define PROJECT_VIEW_H

#include <be/app/Message.h>
#include <be/interface/ListView.h>
#include <be/interface/Point.h>
#include <be/interface/Rect.h>
#include <be/interface/ScrollView.h>
#include "SplitPane.h"

class ProjectView : public BListView {
public:
	ProjectView(BRect frame, BScrollView* dView);
	virtual ~ProjectView();

	virtual void SelectionChanged();
	virtual void MouseDown(BPoint point);
	virtual void MouseMoved(BPoint point, uint32 transit, const BMessage* message);
	virtual void MessageReceived(BMessage* message);
	virtual void KeyDown(const char* bytes, int32 numBytes);

	void SetSplitPane(SplitPane* owner);
	int32 NoVisibleItems();
	void Next(int32 increment);
	void Prev(int32 decrement);

private:
	SplitPane* m_ownerSplitPane;
	BScrollView* m_docView;

	int32 m_lastSelectedIndex;
	bool m_isDocViewHidden;
	// last button pressed down
	uint32 m_button;
};

#endif
