/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOR_WINDOW_H
#include "ColorWindow.h"
#endif

#include <be/interface/Font.h>
#include <be/interface/ScrollView.h>
#include <be/interface/StringView.h>
#include "Constants.h"

FastColor::FastColor(ColorView* theWell, BPoint leftTop, color_control_layout matrix,
	float cellSide, const char* name, BMessage* message, bool bufferedDrawing)
	: BColorControl(leftTop, matrix, cellSide, name, message, bufferedDrawing)
{
	well = theWell;
}

void
FastColor::SetValue(int32 v)
{
	BColorControl::SetValue(v);
	well->SetColor(ValueAsColor());
	if (Window())
		well->Render();
}

void
FastColor::SetValue(rgb_color color)
{
	BColorControl::SetValue(color);
}
//============================================================================
ColorWindow::ColorWindow(BRect frame, BMessenger* messenger, rgb_color color)
	: BWindow(frame, "Pick a Color" B_UTF8_ELLIPSIS, B_FLOATING_WINDOW,
		  B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_NOT_CLOSABLE),
	  m_msgr(messenger)
{
	SetFeel(B_NORMAL_WINDOW_FEEL);

	m_parent = new BView(Bounds(), "parent", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	m_parent->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(m_parent);

	m_colorView = new ColorView(BRect(287, 10, 339, 62));
	m_colorView->SetColor(color);
	m_parent->AddChild(m_colorView);

	m_ctrlBox = new FastColor(m_colorView, BPoint(10, 10), B_CELLS_32x8, 4.0f, "ctrlbox",
		new BMessage(ColorPrefsConstants::K_COLOR_PREFS_UPDATE), true);
	m_parent->AddChild(m_ctrlBox);
	m_ctrlBox->SetValue(color);

	int width = 50;
	int offset = width + 20;

	BRect btnRect(326 - width, 62 + 15, 326 + 10, 62 + 35);

	m_okButton
		= new BButton(btnRect, "fOk", "Ok", new BMessage(ColorPrefsConstants::K_COLOR_PREFS_OK));
	m_parent->AddChild(m_okButton);
	m_okButton->MakeDefault(true);

	btnRect.OffsetBy(-offset, 0);
	m_cancelButton = new BButton(
		btnRect, "fCancel", "Cancel", new BMessage(ColorPrefsConstants::K_COLOR_PREFS_CANCEL));
	m_parent->AddChild(m_cancelButton);
}

ColorWindow::~ColorWindow()
{
	delete m_msgr;
}

void
ColorWindow::MessageReceived(BMessage* message)
{
	if (message->WasDropped()) {
		rgb_color* rgb;
		ssize_t size;
		// if there is RGB color
		if ((message->FindData("RGBColor", B_RGB_COLOR_TYPE, (const void**)&rgb, &size) == B_OK)) {
			m_ctrlBox->SetValue(*rgb);
			m_colorView->SetColor(*rgb);
			m_colorView->Render();
		}
		return;
	}

	switch (message->what) {
		case ColorPrefsConstants::K_COLOR_PREFS_UPDATE:
		{
			rgb_color updated_color = m_ctrlBox->ValueAsColor();
			m_colorView->SetColor(updated_color);
			m_colorView->Render();
		} break;
		case ColorPrefsConstants::K_COLOR_PREFS_OK:
		{
			BMessage mymsg(ColorPrefsConstants::K_COLOR_PREFS_UPDATE);
			rgb_color updated_color = m_ctrlBox->ValueAsColor();
			mymsg.AddData("color", B_RGB_COLOR_TYPE, &updated_color, sizeof(rgb_color));

			m_msgr->SendMessage(&mymsg);
			Quit();
		} break;
		case ColorPrefsConstants::K_COLOR_PREFS_CANCEL:
		{
			Quit();
		} break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

void
ColorWindow::Quit()
{
	m_msgr->SendMessage(new BMessage(ColorPrefsConstants::K_COLOR_WINDOW_QUIT));
	BWindow::Quit();
}
