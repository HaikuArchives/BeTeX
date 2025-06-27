/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef DIMENSION_WINDOW_H
#define DIMENSION_WINDOW_H

#include <be/app/Message.h>
#include <be/app/Messenger.h>
#include <be/interface/Rect.h>
#include <be/interface/TextControl.h>
#include <be/interface/Window.h>
#include <be/support/String.h>
#include <be/support/TypeConstants.h>

class DimensionWindow : public BWindow {
public:
	DimensionWindow(BRect frame, BMessenger* ownerMsgr, BString standardWidth,
		BString standardHeight, uint32 return_msg);
	virtual ~DimensionWindow();

	virtual void MessageReceived(BMessage* message);
	void Quit();

private:
	uint32 m_returnMessage;
	BMessenger* m_ownerMsgr;
	BTextControl *m_rowsText, *m_colsText;
};


#endif
