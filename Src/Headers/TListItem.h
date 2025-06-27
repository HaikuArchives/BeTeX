/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski               *
 *                                                               *
 * All rights reserved.                                          *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef _TLISTITEM_H_
#define _TLISTITEM_H_

#include <Entry.h>
#include <File.h>
#include <Font.h>
#include <ListItem.h>
#include <Path.h>
#include <ScrollView.h>
#include <String.h>
#include <iostream>
#include "Prefs.h"
#include "SplitPane.h"
#include "TexView.h"
const rgb_color highlight_color = {255, 0, 0};
class TListItem : public BListItem {
public:
	TListItem(SplitPane* sp, BScrollView* docScroll, BString label, Prefs* p);	// for new docs
	TListItem(SplitPane* sp, BScrollView* docScroll, entry_ref* r, Prefs* p,
		bool IsTemplate = false);  // for opening docs
	~TListItem();
	virtual void DrawItem(BView* owner, BRect frame, bool complete = false);
	void ShowTextView();
	void HideTextView();
	TexView* TextView();
	const char* FName();
	void SetFName(BString name);
	entry_ref GetRef();
	void SetRef(entry_ref* r);	// sets ref, updates name, sets IsSaved to true
	bool IsSaveNeeded();
	void SetSaveNeeded(bool flag);
	bool IsHomely();
	void SetHomely(bool flag);

private:
	Prefs* prefs;
	SplitPane* SP;
	BScrollView* sv;
	TexView* tv;  // each item has a tv to display the doc
	// BScrollView* sv;
	entry_ref ref;	// points to the doc open
	BString fname;	// name of the file, default is "Untitled 1"
	bool NeedToSave;
	bool HasHome;
};
#endif
