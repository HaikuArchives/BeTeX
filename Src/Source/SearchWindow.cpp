#ifndef SEARCH_WINDOW_H
#include "SearchWindow.h"
#endif

#include <be/interface/Font.h>
#include "Constants.h"
#include "MessageFields.h"
#include "Preferences.h"  //prefsLock
#include "Prefs.h"		  //preferences

SearchWindow::SearchWindow(BRect frame, BMessenger* messenger)
	: BWindow(frame, "Search Dialog", B_FLOATING_WINDOW, B_NOT_ZOOMABLE | B_NOT_RESIZABLE),
	  m_msgr(messenger)
{
	SetFeel(B_NORMAL_WINDOW_FEEL);

	ParentView* parent = new ParentView(Bounds());
	parent->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(parent);

	const float texViewWidth = 200.0f, texViewHeight = 80.0f, strViewHeight = 20.0f;

	BRect r = Bounds();
	// replace this junk later!!!
	r.top = 10.0f;
	r.bottom = r.top + strViewHeight;
	r.OffsetBy(0.0f, strViewHeight + 5.0f);

	r.left = 10.0f;
	r.right = r.left + texViewWidth - B_V_SCROLL_BAR_WIDTH;
	r.bottom = r.top + texViewHeight;

	BRect findViewRect(r);
	BRect findViewTextRect(0.0f, 0.0f, findViewRect.Width(), findViewRect.Height());
	findViewTextRect.InsetBy(5.0f, 5.0f);

	m_findView = new TexView(findViewRect, findViewTextRect);
	m_findView->SetNormalFocus();
	m_findView->SetAttachedFocus();

	ScrollView* findScroll = new ScrollView("findScroll", m_findView, B_FOLLOW_LEFT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE, false, true);
	findScroll->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	parent->AddChild(findScroll);

	// replace this too!!!
	r.OffsetBy(0.0f, texViewHeight + 5.0f);
	r.bottom = r.top + strViewHeight;
	r.OffsetBy(0.0f, strViewHeight + 5.0f);
	r.left = 10.0f;
	r.right = r.left + texViewWidth - B_V_SCROLL_BAR_WIDTH;
	r.bottom = r.top + texViewHeight;

	BRect replaceViewRect(r);
	BRect replaceViewTextRect(0.0f, 0.0f, replaceViewRect.Width(), replaceViewRect.Height());
	replaceViewTextRect.InsetBy(5.0f, 5.0f);

	m_replaceView = new TexView(replaceViewRect, replaceViewTextRect);
	m_replaceView->SetNormalFocus();

	ScrollView* replaceScroll = new ScrollView("replaceScroll", m_replaceView,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE, false, true);
	replaceScroll->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	parent->AddChild(replaceScroll);

	// construct rectangel for button location/size
	float buttonWidth = 100.0f, buttonHeight = 20.0f;

	BRect buttonRect(Bounds());
	buttonRect.left = findViewRect.right + 50.0f;
	buttonRect.top = 36.0f;
	buttonRect.right = buttonRect.left + buttonWidth;
	buttonRect.bottom = buttonRect.top + buttonHeight;

	// construct buttons
	BButton* find = new BButton(
		buttonRect, "find", "Find", new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_FIND));
	parent->AddChild(find);
	find->MakeDefault(true);
	buttonRect.OffsetBy(0.0f, buttonHeight + 10.0f);

	BButton* replace = new BButton(buttonRect, "replace", "Replace",
		new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_REPLACE));
	parent->AddChild(replace);
	buttonRect.OffsetBy(0.0f, buttonHeight + 10.0f);

	BButton* replaceAll = new BButton(buttonRect, "replaceAll", "Replace All",
		new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_REPLACE_ALL));
	parent->AddChild(replaceAll);
	buttonRect.OffsetBy(0.0f, buttonHeight + 10.0f);

	// construct checkboxes
	buttonRect.right += 30.0f;

	m_caseSensitiveCheck = new BCheckBox(buttonRect, "caseSensitiveCheck", "Case Sensitive",
		new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_SETTINGS_CHANGED));
	parent->AddChild(m_caseSensitiveCheck);

	bool isCaseSensitive = false;
	m_caseSensitiveCheck->SetValue(isCaseSensitive);
	buttonRect.OffsetBy(0.0f, buttonHeight + 10.0f);

	m_wrapAroundCheck = new BCheckBox(buttonRect, "wrapAroundCheck", "Wrap Around",
		new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_SETTINGS_CHANGED));
	parent->AddChild(m_wrapAroundCheck);

	bool isWrapAround = false;
	m_wrapAroundCheck->SetValue(isWrapAround);
	buttonRect.OffsetBy(0.0f, buttonHeight + 10.0f);

	m_searchBackwardsCheck = new BCheckBox(buttonRect, "searchBackwardsCheck", "Search Backwards",
		new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_SETTINGS_CHANGED));
	parent->AddChild(m_searchBackwardsCheck);

	bool isSearchBackwards = false;
	m_searchBackwardsCheck->SetValue(isSearchBackwards);
	buttonRect.OffsetBy(0.0f, buttonHeight + 10.0f);

	m_allDocsCheck = new BCheckBox(buttonRect, "allDocsCheck", "Replace In All",
		new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_SETTINGS_CHANGED));
	parent->AddChild(m_allDocsCheck);

	bool isAllDocs = false;
	m_allDocsCheck->SetValue(isAllDocs);
	m_findView->MakeFocus(true);
}

SearchWindow::~SearchWindow()
{
	delete m_msgr;
}

void
SearchWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case SearchWindowConstants::K_SEARCH_WINDOW_SETTINGS_CHANGED:
		{
			prefsLock.Lock();
			preferences->IsAllDocs = m_allDocsCheck->Value();
			preferences->IsSearchBackwards = m_searchBackwardsCheck->Value();
			preferences->IsWrapAround = m_wrapAroundCheck->Value();
			preferences->IsCaseSensitive = m_caseSensitiveCheck->Value();
			prefsLock.Unlock();
		} break;
		case SearchWindowConstants::K_SEARCH_WINDOW_FIND:
		{
			BMessage* fmsg = new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_FIND);
			if ((m_findView->TextLength() > 0)
				&& (fmsg->AddString("ftext", m_findView->Text()) == B_OK)) {
				m_msgr->SendMessage(fmsg);
			}


		} break;
		case SearchWindowConstants::K_SEARCH_WINDOW_REPLACE:
		{
			/*BMessage* rmsg = new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_REPLACE);
			if	(
					(m_findView->TextLength() > 0) &&
					(rmsg->AddString("ftext", m_findView->Text()) == B_OK) &&
					(rmsg->AddString("rtext", m_replaceView->Text()) == B_OK) &&
					(rmsg->AddBool("all", false) == B_OK)
				)
			{
				m_msgr->SendMessage(rmsg);
			}*/
		} break;
		case SearchWindowConstants::K_SEARCH_WINDOW_REPLACE_ALL:
		{
			/*BMessage* rmsg = new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_REPLACE_ALL);
			if	(
					(m_findView->TextLength() > 0) &&
					(rmsg->AddString("ftext", m_findView->Text()) == B_OK) &&
					(rmsg->AddString("rtext", m_replaceView->Text()) == B_OK) &&
					(rmsg->AddBool("all", true) == B_OK)
				)
			{
				m_msgr->SendMessage(rmsg);
			}*/
		} break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

void
SearchWindow::Quit()
{
	m_msgr->SendMessage(new BMessage(SearchWindowConstants::K_SEARCH_WINDOW_QUIT));
	BWindow::Quit();
}

//=================================ParentView======================================================
ParentView::ParentView(BRect frame)
	: BView(frame, "parent", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
}

ParentView::~ParentView() {}

void
ParentView::Draw(BRect updateRect)
{
	BFont font(be_bold_font);
	font.SetSize(12.0f);
	SetFont(&font);

	MovePenTo(10.0f, 25.0f);
	DrawString("Find:");

	MovePenTo(10.0f, 135.0f);
	DrawString("Replace:");
}

//==================================ScrollView=====================================================
ScrollView::ScrollView(const char* name, BView* target, uint32 rmode, uint32 flags, bool horizontal,
	bool vertical, border_style border)
	: BScrollView(name, target, rmode, flags, horizontal, vertical, border)
{
}

ScrollView::~ScrollView() {}

void
ScrollView::MakeFocus(bool focus)
{
	Target()->MakeFocus(focus);
}
