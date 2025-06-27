/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H

#include <be/app/Message.h>
#include <be/app/Messenger.h>
#include <be/interface/Bitmap.h>
#include <be/interface/Button.h>
#include <be/interface/Rect.h>
#include <be/interface/View.h>
#include <be/interface/Window.h>
#include "GradientView.h"

const uint32 UPDATE_COLORS = 'updt';
const int32 UPDATE_TIME = 100;

class AboutView : public BView {
public:
	AboutView(BRect frame);
	virtual ~AboutView();

	virtual void Draw(BRect updateRect);

private:
	void ConvertRGB32ToGreyRGB32(BBitmap* b);

private:
	BBitmap* m_logo;
};

class AboutWindow : public BWindow {
public:
	AboutWindow(BRect frame, BMessenger* messenger);
	virtual ~AboutWindow();

	virtual void MessageReceived(BMessage* message);
	void Quit();

private:
	AboutView* m_about;
	BMessenger* m_msgr;

	BButton *m_homePage, *m_donate, *m_beBits, *m_quit;

	GradientView* m_gradient;

	rgb_color m_top, m_bottom;

	bool m_tup, m_bup;
};
#endif
