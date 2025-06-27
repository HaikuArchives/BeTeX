/*******************************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski                             *
 *                                                                             *
 * All rights reserved.                                                        *
 * Distributed under the terms of the MIT License.                             *
 ******************************************************************************/
#ifndef PROJECT_ITEM_H
#define PROJECT_ITEM_H

#include <be/interface/ListItem.h>
#include <be/interface/Rect.h>
#include <be/interface/ScrollView.h>
#include <be/interface/View.h>
#include <be/storage/Path.h>


#include <be/support/String.h>
#include "SplitPane.h"
#include "TexView.h"

class ProjectItem : public BListItem {
public:
	ProjectItem(SplitPane* splitPane, BString label, BView* childView);
	virtual ~ProjectItem();

	virtual void DrawItem(BView* owner, BRect frame, bool complete = false);

	void ShowTextView();
	void HideTextView();
	TexView* ChildView();

	void SetLabel(BString label);
	BString Label();

	void SetSaveNeeded(bool needsSave);
	bool IsSaveNeeded();
	void SetHomely(bool hasHome);
	bool IsHomely();

	void SetRef(entry_ref* ref);
	entry_ref GetRef() const;

private:
	SplitPane* m_splitPane;
	BView* m_childView;

	BString m_label;
	bool m_needToSave, m_hasHome;
	entry_ref m_ref;
};
#endif
