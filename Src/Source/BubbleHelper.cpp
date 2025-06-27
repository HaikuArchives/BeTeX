/*

	Bubblehelp class Copyright (C) 1998 Marco Nelissen <marcone@xs4all.nl>
	Freely usable in non-commercial applications, as long as proper credit
	is given.

	Usage:
	- Add the file BubbleHelper.cpp to your project
	- #include "BubbleHelper.h" in your files where needed
	- Create a single instance of BubbleHelper (it will serve your entire
	  application). It is safe to create one on the stack or as a global.
	- Call SetHelp(view,text) for each view to which you wish to attach a text.
	- Use SetHelp(view,NULL) to remove text from a view.

	This could be implemented as a BMessageFilter as well, but that means using
	one bubblehelp-instance for each window to which you wish to add help-bubbles.
	Using a single looping thread for everything turned out to be the most practical
	solution.

*/
#ifndef BUBBLE_HELPER_H
#include "BubbleHelper.h"
#endif

#include <be/app/Application.h>
#include <be/interface/Font.h>
#include <be/interface/Region.h>
#include <be/interface/Screen.h>
#include <be/interface/Window.h>
#include <malloc.h>
#include <string.h>
#include <iostream>

struct helppair {
	BView* view;
	char* text;
};

BubbleHelper::BubbleHelper(rgb_color color)
	: m_bubbleColor(color),
	  m_helplist(new BList(30))
{
	m_helperthread = spawn_thread(_helper, "helper", B_NORMAL_PRIORITY, this);
	if (m_helperthread >= 0)
		resume_thread(m_helperthread);
	m_enabled = true;
}

BubbleHelper::~BubbleHelper()
{
	if (m_helperthread >= 0) {
		// force helper thread into a known state
		bool locked = m_textwin->Lock();
		// Be rude...
		kill_thread(m_helperthread);
		// dispose of window
		if (locked) {
			m_textwin->PostMessage(B_QUIT_REQUESTED);
			m_textwin->Unlock();
		}
	}

	if (m_helplist) {
		helppair* pair;
		int i = m_helplist->CountItems() - 1;
		while (i >= 0) {
			pair = (helppair*)m_helplist->RemoveItem(i);
			if (pair && pair->text)
				free(pair->text);
			delete pair;
			i--;
		}
		delete m_helplist;
	}
}

void
BubbleHelper::SetHelp(BView* view, const char* text)
{
	if (view) {
		// delete previous text for this view, if any
		for (int i = 0;; i++) {
			helppair* pair;
			pair = (helppair*)m_helplist->ItemAt(i);
			if (!pair)
				break;
			if (pair->view == view) {
				m_helplist->RemoveItem(pair);
				free(pair->text);
				delete pair;
				break;
			}
		}

		// add new text, if any
		if (text) {
			helppair* pair = new helppair;
			pair->view = view;
			pair->text = strdup(text);
			m_helplist->AddItem(pair);
		}
	}
}

char*
BubbleHelper::GetHelp(BView* view)
{
	int i = 0;
	helppair* pair;

	// This could be sped up by sorting the list and
	// doing a binary search.
	// Right now this is left as an exercise for the
	// reader, or should I say "third party opportunity"?
	while ((pair = (helppair*)m_helplist->ItemAt(i++)) != NULL) {
		if (pair->view == view)
			return pair->text;
	}
	return NULL;
}


status_t
BubbleHelper::_helper(void* arg)
{
	((BubbleHelper*)arg)->Helper();
	return 0;
}

void
BubbleHelper::Helper()
{
	// Wait until the BApplication becomes valid, in case
	// someone creates this as a global variable.
	while (!be_app_messenger.IsValid())
		snooze(200000);
	// wait a little longer, until the BApplication is really up to speed
	while (be_app->IsLaunching())
		snooze(200000);

	m_textwin = new BWindow(BRect(-100, -100, -50, -50), "", B_BORDERED_WINDOW_LOOK,
		B_FLOATING_ALL_WINDOW_FEEL, B_NOT_MOVABLE | B_AVOID_FOCUS);

	m_textview = new BTextView(
		BRect(0, 0, 50, 50), "", BRect(2, 2, 48, 48), B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	BFont font(be_bold_font);
	font.SetSize(12);
	rgb_color text_color = {0, 0, 0};
	m_textview->SetStylable(true);
	m_textview->SetFontAndColor(&font, B_FONT_ALL, &text_color);
	m_textview->MakeEditable(false);
	m_textview->MakeSelectable(false);
	m_textview->SetWordWrap(false);
	// m_textview->SetLowColor(240,240,100);
	m_textview->SetViewColor(m_bubbleColor);
	m_textview->SetHighColor(0, 0, 0);
	m_textwin->AddChild(m_textview);
	m_textwin->Run();
	m_textwin->Lock();
	m_textwin->Activate(false);
	rename_thread(m_textwin->Thread(), "bubble");
	m_textwin->Unlock();

	ulong delaycounter = 0;
	BPoint lastwhere;

	while (be_app_messenger.IsValid()) {
		BPoint where;
		uint32 buttons;
		if (m_enabled) {
			if (m_textwin->Lock()) {
				m_textview->GetMouse(&where, &buttons);
				m_textview->ConvertToScreen(&where);
				if (lastwhere != where || buttons) {
					delaycounter = 0;
				} else {
					// mouse didn't move
					if (delaycounter++ > 5) {
						delaycounter = 0;
						// mouse didn't move for a while
						BView* view = FindView(where);
						char* text = NULL;
						while (view && (text = GetHelp(view)) == NULL)
							view = view->Parent();
						if (text) {
							DisplayHelp(text, where);
							// wait until mouse moves out of view, or wait
							// for timeout
							long displaycounter = 0;
							BPoint where2;
							long displaytime = max_c(20, strlen(text));
							do {
								m_textwin->Unlock();
								snooze(100000);
								if (!m_textwin->Lock())
									goto end;  // window is apparently gone
								m_textview->GetMouse(&where2, &buttons);
								m_textview->ConvertToScreen(&where2);
							} while (
								!buttons && where2 == where && (displaycounter++ < displaytime));

							HideBubble();
							do {
								m_textwin->Unlock();
								snooze(100000);
								if (!m_textwin->Lock())
									goto end;  // window is apparently gone
								m_textview->GetMouse(&where2, &buttons);
								m_textview->ConvertToScreen(&where2);
							} while (where2 == where);
						}
					}
				}
				lastwhere = where;
				m_textwin->Unlock();
			}
		}
	end:
		snooze(100000);
	}
	// (this thread normally gets killed by the destructor before arriving here)
}

void
BubbleHelper::HideBubble()
{
	m_textwin->MoveTo(-1000, -1000);  // hide it
	if (!m_textwin->IsHidden())
		m_textwin->Hide();
}

void
BubbleHelper::ShowBubble(BPoint dest)
{
	m_textview->SetViewColor(m_bubbleColor);
	m_textwin->MoveTo(dest);
	m_textwin->SetWorkspaces(B_CURRENT_WORKSPACE);
	if (m_textwin->IsHidden())
		m_textwin->Show();
}

BView*
BubbleHelper::FindView(BPoint where)
{
	BView* winview = NULL;
	BWindow* win;
	long windex = 0;
	while ((winview == NULL) && ((win = be_app->WindowAt(windex++)) != NULL)) {
		if (win != m_textwin) {
			// lock with timeout, in case somebody has a non-running window around
			// in their app.
			if (win->LockWithTimeout(1E6) == B_OK) {
				BRect frame = win->Frame();
				if (frame.Contains(where)) {
					BPoint winpoint;
					winpoint = where - frame.LeftTop();
					winview = win->FindView(winpoint);
					if (winview) {
						BRegion region;
						BPoint newpoint = where;
						winview->ConvertFromScreen(&newpoint);
						winview->GetClippingRegion(&region);
						if (!region.Contains(newpoint))
							winview = 0;
					}
				}
				win->Unlock();
			}
		}
	}
	return winview;
}

void
BubbleHelper::DisplayHelp(char* text, BPoint where)
{
	m_textview->SetText(text);

	float height = m_textview->TextHeight(0, 2E6) + 4;
	float width = 0;
	int numlines = m_textview->CountLines();
	int linewidth;
	for (int i = 0; i < numlines; i++)
		if ((linewidth = int(m_textview->LineWidth(i))) > width)
			width = linewidth;
	m_textwin->ResizeTo(width + 4, height);
	m_textview->SetTextRect(BRect(2, 2, width + 2, height + 2));

	BScreen screen;
	BPoint dest = where + BPoint(0, 20);
	BRect screenframe = screen.Frame();
	if ((dest.y + height) > (screenframe.bottom - 3))
		dest.y = dest.y - (16 + height + 8);

	if ((dest.x + width + 4) > (screenframe.right))
		dest.x = dest.x - ((dest.x + width + 4) - screenframe.right);

	ShowBubble(dest);
}

void
BubbleHelper::EnableHelp(bool enable)
{
	m_enabled = enable;
}


void
BubbleHelper::SetColor(rgb_color color)
{
	m_bubbleColor = color;
}
