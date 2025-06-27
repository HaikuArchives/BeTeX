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
#define BUBBLE_HELPER_H

#include <OS.h>
#include <be/interface/Point.h>
#include <be/interface/TextView.h>
#include <be/interface/View.h>

class BubbleHelper {
public:
	BubbleHelper(rgb_color color);
	virtual ~BubbleHelper();

	void SetHelp(BView* view, const char* text);
	void EnableHelp(bool enable = true);
	void SetColor(rgb_color color);

private:
	void DisplayHelp(char* text, BPoint where);
	void Helper();
	char* GetHelp(BView* view);
	static status_t _helper(void* arg);
	BView* FindView(BPoint where);

	void HideBubble();
	void ShowBubble(BPoint dest);

private:
	rgb_color m_bubbleColor;
	thread_id m_helperthread;
	BList* m_helplist;
	BWindow* m_textwin;
	BTextView* m_textview;
	bool m_enabled;
};

#endif
