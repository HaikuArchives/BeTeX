/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski               *
 *                                                               *
 * All rights reserved.                                          *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef _GOTOLINE_WINDOW_H_
#define _GOTOLINE_WINDOW_H_

#include <Button.h>
#include <Font.h>
#include <Messenger.h>
#include <String.h>
#include <TextControl.h>
#include <View.h>
#include <Window.h>
#include <cctype>
#include <iostream>
#include <string>
#include "Constants.h"
class GoToLineWindow : public BWindow {
public:
	GoToLineWindow(BRect r, BMessenger* messenger);
	~GoToLineWindow();
	virtual void MessageReceived(BMessage* msg);
	void Quit();

private:
	BView* parent;
	BMessenger* msgr;
	BButton* go;
	BTextControl* num;
};
#endif
