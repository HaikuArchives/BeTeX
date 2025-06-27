#ifndef _TEXBARITEM_H_
#define _TEXBARITEM_H_

#include <Bitmap.h>
#include <File.h>
#include <Font.h>
#include <Messenger.h>
#include <Mime.h>
#include <Resources.h>
#include <Roster.h>
#include <View.h>
#include "Constants.h"
#include "TButton.h"

class TexBarItemView : public BView {
public:
	TexBarItemView(BRect f, const char* n, uint32 rm, uint32 fl);
	virtual void MouseMoved(BPoint point, uint32 transit, const BMessage* msg);
};
class TexBarItem : public BView {
public:
	TexBarItem(BRect r, const char* l, TexBarItemView* p);
	virtual void Draw(BRect r);
	virtual void AttachedToWindow();
	virtual void MessageReceived(BMessage* msg);
	void ScrollBy(float h, float v);
	virtual void ScrollTo(BPoint where);
	virtual void MouseDown(BPoint point);
	virtual void MouseUp(BPoint point);
	virtual void MouseMoved(BPoint point, uint32 transit, const BMessage* msg);
	float Height();
	void Invoke();

private:
	TexBarItemView* plateau;
	const char* label;
	float ItemHeight;
	float PlateauHeight;
	bool IsHidden;
	bool OkToInvoke;
	bool IsDown;
	bool IsOver;
	BRect labelRect;
};

#endif
