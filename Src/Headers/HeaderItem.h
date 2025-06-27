#ifndef HEADER_ITEM_H
#define HEADER_ITEM_H

#include <app/Message.h>
#include <interface/Point.h>
#include <interface/Rect.h>
#include <interface/View.h>
#include "Prefs.h"

class HeaderItem : public BView {
public:
	HeaderItem(BRect f, const char* n, uint32 rm, uint32 fl);
	virtual void Draw(BRect r);
	virtual void AttachedToWindow();
	virtual void MessageReceived(BMessage* msg);
	void ScrollBy(float h, float v);
	virtual void ScrollTo(BPoint where);
	virtual void MouseDown(BPoint point);
	virtual void MouseUp(BPoint point);
	virtual void MouseMoved(BPoint point, uint32 transit, const BMessage* msg);
	void Invoke();
	virtual void FrameResized(float w, float h);
	void SetHidden(bool tohideornottohide);

private:
	const char* label;
	bool OkToInvoke;
	bool IsDown;
	bool IsOver;
	bool IsHidden;
};

#endif
