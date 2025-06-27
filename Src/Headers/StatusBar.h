/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <iostream>

#include <be/interface/Rect.h>
#include <be/support/String.h>
#include "DoubleBufferedView.h"

/** This class is a simple subclassed DoubleBufferedView,
	that is intended to be used as a bar at the bottom of a window
	to display text (status).

	@author Brent Miszalski
	@author Tim de Jong
	@comment cleaned up Brent's code and added the destructor,
	SetText(BString) method	and the Text() method.
*/

class StatusBar : public /*DoubleBuffered*/ BView {
public:
	StatusBar(BRect frame);
	virtual ~StatusBar();

	void Draw(BRect updateRect /*BView* backView*/);
	void SetText(BString statusText);
	void SetText(const char* statusText);
	BString Text();

private:
	BString m_statusText;
};

#endif
