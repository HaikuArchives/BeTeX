/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef COLOR_WINDOW_H
#define COLOR_WINDOW_H

#include <TypeConstants.h>
#include <be/app/Message.h>
#include <be/app/Messenger.h>
#include <be/interface/Button.h>
#include <be/interface/ColorControl.h>
#include <be/interface/Point.h>
#include <be/interface/Rect.h>
#include <be/interface/View.h>
#include <be/interface/Window.h>
#include "ColorView.h"

class FastColor : public BColorControl {
public:
	FastColor(ColorView* theWell, BPoint leftTop, color_control_layout matrix, float cellSide,
		const char* name, BMessage* message = NULL, bool bufferedDrawing = false);

	void SetValue(int32 v);
	void SetValue(rgb_color color);

private:
	ColorView* well;
};

class ColorWindow : public BWindow {
public:
	ColorWindow(BRect frame, BMessenger* messenger, rgb_color color);
	virtual ~ColorWindow();

	virtual void MessageReceived(BMessage* message);
	void Quit();

private:
	BMessenger* m_msgr;

	BButton *m_okButton, *m_cancelButton;

	BView* m_parent;
	FastColor* m_ctrlBox;
	ColorView* m_colorView;
};

#endif
