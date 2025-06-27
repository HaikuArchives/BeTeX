/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef ABOUT_WINDOW_H
#include "AboutWindow.h"
#endif

#include <be/app/Roster.h>
#include <be/support/String.h>
#include "BeTeXLogoSmall.h"
#include "Constants.h"

AboutView::AboutView(BRect frame)
	: BView(frame, "about", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
	int w = 169;
	int h = 73;
	m_logo = new BBitmap(BRect(0, 0, w, h), B_RGB32);
	m_logo->ImportBits(betex_logo_small, (w + 1) * (h + 1) * 3, (w + 1) * 3, 0, B_RGB24_BIG);
}

AboutView::~AboutView() {}

void
AboutView::Draw(BRect updateRect)
{
	BFont font(be_bold_font);
	font.SetSize(12);
	SetFont(&font);
	updateRect = Bounds();

	BFont fixed(be_fixed_font);
	fixed.SetSize(12);
	MovePenTo((updateRect.Width() - m_logo->Bounds().Width()) / 2, 0);
	DrawBitmap(m_logo);
	MovePenTo((updateRect.Width() - m_logo->Bounds().Width()) / 4, 0);

	MovePenBy(0, 100);
	BPoint p = PenLocation();

	BString about_line;
	// about_line << "BeTeX " << BETEX_VERSION << " by Brent Miszalski";
	DrawString(about_line.String());  //.\nThanks to:\n\tEli Dayan\n\tMichael Pfeiffer\n");
	MovePenTo(p);
	MovePenBy(0, 20);
	p = PenLocation();
	DrawString("Thanks To:   Eli Dayan");
	MovePenTo(p);
	MovePenBy(font.StringWidth("Thanks To:   "), 20);
	DrawString("Michael Pfeiffer");

	updateRect = Bounds();
	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_2_TINT));
	StrokeLine(BPoint(updateRect.left, updateRect.top), BPoint(updateRect.left, updateRect.bottom));
	StrokeLine(BPoint(updateRect.left, updateRect.top), BPoint(updateRect.right, updateRect.top));

	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	StrokeLine(
		BPoint(updateRect.left, updateRect.bottom), BPoint(updateRect.right, updateRect.bottom));
	StrokeLine(
		BPoint(updateRect.right, updateRect.top + 1), BPoint(updateRect.right, updateRect.bottom));
}

void
AboutView::ConvertRGB32ToGreyRGB32(BBitmap* b)
{
	uint8* src = (uint8*)b->Bits();

	for (int i = 0; i < b->BitsLength(); i += 4) {
		uint8 avg = (uint8)(0.3 * src[i + 2] + 0.59 * src[i + 1] + 0.11 * src[i]);
		src[i] = src[i + 1] = src[i + 2] = avg;
	}
}

AboutWindow::AboutWindow(BRect frame, BMessenger* messenger)
	: BWindow(frame, "About BeTeX", B_FLOATING_WINDOW, B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	SetFeel(B_NORMAL_WINDOW_FEEL);

	frame = Bounds();
	BRect specRect(Bounds());

	rgb_color red = {255, 0, 0};
	rgb_color blue = {0, 0, 255};

	m_top = red;
	m_tup = true;
	m_bottom = blue;
	m_bup = false;

	AddChild(m_gradient = new GradientView(specRect, m_top, m_bottom));

	BRect aboutRect = Bounds();
	aboutRect.InsetBy(80, 60);
	m_about = new AboutView(aboutRect);
	m_about->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	m_gradient->AddChild(m_about);

	int bwidth = 50;
	int bheight = 30;
	int bpad = 10;
	BRect btnRect(frame.right - (2 * bpad) - bwidth, frame.bottom - (2 * bpad) - bheight,
		frame.right - (2 * bpad), frame.bottom - (2 * bpad));
	m_quit
		= new BButton(btnRect, "quitbtn", "OK", new BMessage(AboutMessages::K_ABOUT_WINDOW_QUIT));
	m_quit->MakeDefault(true);
	m_gradient->AddChild(m_quit);

	bwidth = 100;
	btnRect = BRect(frame.right - (2 * bpad) - bwidth, frame.bottom - (2 * bpad) - bheight,
		frame.right - (2 * bpad), frame.bottom - (2 * bpad));
	btnRect.OffsetBy(-(50 + (2 * bpad)), 0);

	m_homePage = new BButton(btnRect, "homepagebtn", "BeTeX Homepage",
		new BMessage(AboutMessages::K_GOTO_BETEX_HOMEPAGE));
	m_gradient->AddChild(m_homePage);

	btnRect.OffsetBy(-(80 + (2 * bpad)), 0);
	btnRect.right = btnRect.left + 80;
	m_beBits = new BButton(
		btnRect, "fBeBitsbtn", "BeBits Page", new BMessage(AboutMessages::K_GOTO_BETEX_BEBITSPAGE));
	m_gradient->AddChild(m_beBits);

	btnRect.OffsetBy(-(60 + (2 * bpad)), 0);
	btnRect.right = btnRect.left + 60;
	m_donate = new BButton(
		btnRect, "fDonatebtn", "Donate", new BMessage(AboutMessages::K_GOTO_BETEX_DONATEPAGE));
	m_gradient->AddChild(m_donate);

	BMessenger mymsnger(this);
	BMessage increment(UPDATE_COLORS);

	m_msgr = messenger;
}

AboutWindow::~AboutWindow()
{
	delete m_msgr;
}

void
AboutWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case UPDATE_COLORS:
		{
			if (m_tup) {
				if (m_top.blue == 255) {
					m_top.blue--;
					m_top.red++;
					m_tup = false;
				} else {
					m_top.blue++;
					m_top.red--;
				}
			} else {
				if (m_top.blue == 0) {
					m_top.blue++;
					m_top.red--;
					m_tup = true;
				} else {
					m_top.blue--;
					m_top.red++;
				}
			}
			if (m_bup) {
				if (m_bottom.blue == 255) {
					m_bottom.blue--;
					m_bottom.red++;
					m_bup = false;
				} else {
					m_bottom.blue++;
					m_bottom.red--;
				}
			} else {
				if (m_bottom.blue == 0) {
					m_bottom.blue++;
					m_bottom.red--;
					m_bup = true;
				} else {
					m_bottom.blue--;
					m_bottom.red++;
				}
			}

			m_gradient->SetTopColor(m_top);
			m_gradient->SetBottomColor(m_bottom);
		} break;
		case AboutMessages::K_ABOUT_WINDOW_QUIT:
		{
			Quit();
		} break;
		case AboutMessages::K_GOTO_BETEX_HOMEPAGE:
		{
			const char* url = "http://misza.beosjournal.org";
			be_roster->Launch("text/html", 1, (char**)&url);
			Quit();
		} break;
		case AboutMessages::K_GOTO_BETEX_BEBITSPAGE:
		{
			const char* url = "http://www.bebits.com/app/3919";
			be_roster->Launch("text/html", 1, (char**)&url);
			Quit();
		} break;
		case AboutMessages::K_GOTO_BETEX_DONATEPAGE:
		{
			const char* url = "http://misza.beosjournal.org/donate.html";
			be_roster->Launch("text/html", 1, (char**)&url);
			Quit();
		} break;
		default:
			BWindow::MessageReceived(msg);
			break;
	}
}

void
AboutWindow::Quit()
{
	m_msgr->SendMessage(new BMessage(AboutMessages::K_ABOUT_WINDOW_QUIT));
	BWindow::Quit();
}
