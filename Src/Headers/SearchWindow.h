#ifndef SEARCH_WINDOW_H
#define SEARCH_WINDOW_H

#include <be/app/Messenger.h>
#include <be/interface/Button.h>
#include <be/interface/CheckBox.h>
#include <be/interface/Rect.h>
#include <be/interface/ScrollView.h>
#include <be/interface/View.h>
#include <be/interface/Window.h>
#include "TexView.h"

class ParentView;

class SearchWindow : public BWindow {
public:
	SearchWindow(BRect frame, BMessenger* messenger);
	virtual ~SearchWindow();

	virtual void MessageReceived(BMessage* message);
	void Quit();

private:
	BMessenger* m_msgr;

	TexView *m_findView, *m_replaceView;

	BCheckBox *m_caseSensitiveCheck, *m_wrapAroundCheck, *m_searchBackwardsCheck, *m_allDocsCheck;
};

class ParentView : public BView {
public:
	ParentView(BRect frame);
	virtual ~ParentView();

	virtual void Draw(BRect updateRect);
};

class ScrollView : public BScrollView {
public:
	ScrollView(const char* name, BView* target, uint32 rmode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
		uint32 flags = 0, bool horizontal = false, bool vertical = false,
		border_style border = B_FANCY_BORDER);
	virtual ~ScrollView();

	virtual void MakeFocus(bool focus = true);
};

#endif
