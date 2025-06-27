/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOR_BUTTON_H
#include "ColorButton.h"
#endif

#include <Screen.h>
#include <be/app/Application.h>
#include <iostream>
#include "Constants.h"

ColorButton::ColorButton(
	BRect frame, BString name, BMessage* message, rgb_color color, BString label)
	: BControl(frame, name, NULL, message, B_FOLLOW_NONE, B_WILL_DRAW),
	  m_colorWindow(NULL),
	  m_color(color),
	  m_label(label),
	  m_isDown(false),
	  m_isOver(false),
	  m_okToInvoke(false)

{
	m_bitmap = new BBitmap(Bounds(), B_RGB32, true);
	m_bitmap->Lock();
	m_bitmapView = new BView(Bounds(), "view", B_FOLLOW_NONE, B_WILL_DRAW);
	m_bitmap->AddChild(m_bitmapView);
	m_bitmap->Unlock();
}

ColorButton::~ColorButton() {}

void
ColorButton::AttachedToWindow()
{
	BControl::AttachedToWindow();
	SetTarget(Parent());
}

void
ColorButton::Draw(BRect updateRect)
{
	Render();
}

void
ColorButton::Render()
{
	m_bitmap->Lock();

	BRect r = m_bitmapView->Bounds();
	m_bitmapView->SetDrawingMode(B_OP_COPY);

	m_bitmapView->SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	m_bitmapView->FillRect(r);
	r = BRect(0, 0, 14, 14);
	m_bitmapView->SetHighColor(m_color);
	m_bitmapView->FillRect(r);

	if (m_isDown)
		m_bitmapView->SetHighColor(tint_color(m_color, B_DARKEN_2_TINT));
	else
		m_bitmapView->SetHighColor(tint_color(m_color, B_LIGHTEN_2_TINT));

	m_bitmapView->StrokeLine(BPoint(r.left, r.top), BPoint(r.left, r.bottom));
	m_bitmapView->StrokeLine(BPoint(r.left, r.top), BPoint(r.right, r.top));

	if (m_isDown)
		m_bitmapView->SetHighColor(tint_color(m_color, B_DARKEN_1_TINT));
	else
		m_bitmapView->SetHighColor(tint_color(m_color, B_LIGHTEN_1_TINT));


	if (m_isDown)
		m_bitmapView->SetHighColor(tint_color(m_color, B_LIGHTEN_2_TINT));
	else
		m_bitmapView->SetHighColor(tint_color(m_color, B_DARKEN_2_TINT));

	m_bitmapView->StrokeLine(BPoint(r.left, r.bottom), BPoint(r.right, r.bottom));
	m_bitmapView->StrokeLine(BPoint(r.right, r.top), BPoint(r.right, r.bottom));

	if (m_isDown)
		m_bitmapView->SetHighColor(tint_color(m_color, B_LIGHTEN_1_TINT));
	else
		m_bitmapView->SetHighColor(tint_color(m_color, B_DARKEN_1_TINT));

	BPoint point(r.RightBottom());

	font_height fh;
	BFont font(be_plain_font);
	font.SetSize(12.0f);
	font.GetHeight(&fh);

	m_bitmapView->MovePenTo(BPoint(point.x + 10.0f, point.y - fh.descent));
	m_bitmapView->SetHighColor(0, 0, 0);
	m_bitmapView->DrawString(m_label.String());
	m_bitmapView->Sync();
	DrawBitmapAsync(m_bitmap, Bounds(), Bounds());
	Flush();
	Sync();

	m_bitmap->Unlock();
}

void
ColorButton::MouseDown(BPoint point)
{
	uint32 button;
	GetMouse(&point, &button);

	if (button == B_PRIMARY_MOUSE_BUTTON && IsEnabled()) {
		m_isDown = true;
		m_okToInvoke = true;
		Render();
	}
}

void
ColorButton::MouseUp(BPoint point)
{
	m_isDown = false;
	Render();

	if (m_okToInvoke) {
		Invoke();
		m_okToInvoke = false;
	}
}

void
ColorButton::MouseMoved(BPoint point, uint32 transit, const BMessage* message)
{
	uint32 button;
	GetMouse(&point, &button);

	switch (transit) {
		case B_ENTERED_VIEW:
		{
			m_isOver = true;
		} break;
		case B_EXITED_VIEW:
		{
			m_isOver = false;
			if (m_isDown) {
				m_isDown = false;
				Render();
			}

			m_okToInvoke = false;
			m_isDown = false;
		} break;
	}

	Parent()->MouseMoved(point, transit, message);
}

void
ColorButton::MessageReceived(BMessage* message)
{
	if (message->WasDropped()) {
		rgb_color* rgb;
		ssize_t size;
		// if there is RGB color
		if ((message->FindData("RGBColor", B_RGB_COLOR_TYPE, (const void**)&rgb, &size) == B_OK)) {
			m_color = *rgb;
			Render();
			BControl::Invoke();
		}
	}

	switch (message->what) {
		case ColorPrefsConstants::K_COLOR_WINDOW_QUIT:
		{
			m_colorWindow = NULL;  // mmm?
		} break;
		case ColorPrefsConstants::K_COLOR_PREFS_UPDATE:
		{
			rgb_color* rgb;
			ssize_t size;
			// if there is RGB color
			if ((message->FindData("color", B_RGB_COLOR_TYPE, (const void**)&rgb, &size) == B_OK)) {
				m_color = *rgb;
				Render();
				BControl::Invoke();
			}
		} break;
		default:
			BControl::MessageReceived(message);
			break;
	}
}

status_t
ColorButton::Invoke(BMessage* message)
{
	// start up chooser window
	if (m_colorWindow != NULL) {
		m_colorWindow->Activate(true);
		return B_OK;
	}

	BPoint point;
	uint32 buttons;
	GetMouse(&point, &buttons);
	ConvertToScreen(&point);

	point.x -= 30.0f;
	point.y -= 30.0f;
	float width = 350.0f, height = 115.0f;

	BRect colorWindowFrame(point.x, point.y, point.x + width, point.y + height);
	m_colorWindow = new ColorWindow(colorWindowFrame, new BMessenger(this), m_color);
	m_colorWindow->Show();

	return B_OK;
}

void
ColorButton::SetValue(rgb_color color)
{
	m_color = color;
	Render();
}

rgb_color
ColorButton::Value()
{
	return m_color;
}
