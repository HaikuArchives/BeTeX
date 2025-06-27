#include "HeaderItem.h"
#include <app/Messenger.h>
#include <string>
#include "Constants.h"
using std::string;

HeaderItem::HeaderItem(BRect f, const char* n, uint32 rm, uint32 fl)
	: BView(f, "TexBarItem", B_FOLLOW_TOP, B_WILL_DRAW)
{
	// B_PANEL_BACKGROUND_COLOR
	SetViewColor(ui_color(
		B_PANEL_BACKGROUND_COLOR));	 // tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_2_TINT));
	f = Bounds();
	label = n;

	string name = string("TexBarItem_") + n;
	SetName(name.c_str());

	IsHidden = false;
	IsDown = false;
	IsOver = false;
}

void
HeaderItem::ScrollBy(float h, float v)
{
	BView::ScrollBy(h, v);
	Invalidate();
}

void
HeaderItem::ScrollTo(BPoint where)
{
	BView::ScrollTo(where);
	Invalidate();
}

void
HeaderItem::Draw(BRect r)
{
	r = Bounds();
	bool IsInverted = preferences->IsTexBarInverted;

	float ItemHeight = r.Height();
	/*if(!IsDown)
		SetHighColor(tint_color(ViewColor(),B_DARKEN_3_TINT));
	else
		SetHighColor(tint_color(ViewColor(),B_DARKEN_4_TINT));

	rgb_color low = HighColor();
	FillRect(r);

	BeginLineArray(4);
	SetHighColor(tint_color(HighColor(),B_DARKEN_1_TINT));
	AddLine(BPoint(r.left+1,r.bottom),BPoint(r.right-1,r.bottom),HighColor());
	AddLine(BPoint(r.right,r.bottom),BPoint(r.right,r.top),HighColor());

	SetHighColor(tint_color(HighColor(),B_LIGHTEN_1_TINT));
	AddLine(BPoint(r.left+1,r.top),BPoint(r.right-1,r.top),HighColor());
	AddLine(BPoint(r.left,r.bottom),BPoint(r.left,r.top),HighColor());
	*/
	//
	rgb_color low = HighColor();
	BeginLineArray(70);
	for (int i = 0; i <= 20; i++) {
		if (IsInverted) {
			SetHighColor(255, 255 - i * 12, 0);
			if (!IsDown) {
				rgb_color invert_me = HighColor();
				invert_me.red = 255 - invert_me.red;
				invert_me.green = 255 - invert_me.green;
				invert_me.blue = 255 - invert_me.blue;
				SetHighColor(invert_me);
			}
		} else {
			SetHighColor(255, i * 12, 0);
			if (IsDown) {
				rgb_color invert_me = HighColor();
				invert_me.red = 255 - invert_me.red;
				invert_me.green = 255 - invert_me.green;
				invert_me.blue = 255 - invert_me.blue;
				SetHighColor(invert_me);
			}
		}

		if (i == 10)
			low = HighColor();

		AddLine(
			BPoint(r.left + 1, ItemHeight - i), BPoint(r.right - 1, ItemHeight - i), HighColor());

		if (i == 0) {
			SetHighColor(tint_color(HighColor(), B_DARKEN_2_TINT));
			AddLine(BPoint(r.left + 1, ItemHeight - i), BPoint(r.right - 1, ItemHeight - i),
				HighColor());
		} else if (i == 20) {
			SetHighColor(tint_color(HighColor(), B_LIGHTEN_2_TINT));
			AddLine(BPoint(r.left + 1, ItemHeight - i), BPoint(r.right - 1, ItemHeight - i),
				HighColor());
		}

		// left highlight
		SetHighColor(tint_color(HighColor(), B_LIGHTEN_2_TINT));
		AddLine(BPoint(r.left, ItemHeight - i), BPoint(r.left, ItemHeight - i), HighColor());
		// right shadow
		SetHighColor(tint_color(HighColor(), B_DARKEN_2_TINT));
		AddLine(BPoint(r.right, ItemHeight - i), BPoint(r.right, ItemHeight - i), HighColor());
	}
	EndLineArray();
	BFont font(be_bold_font);
	font.SetSize(12);
	SetFont(&font);

	SetHighColor(ColorConstants::K_BLACK);
	SetLowColor(low);
	MovePenTo(5, ItemHeight - 5);
	DrawString(label);
}

void
HeaderItem::AttachedToWindow()
{
	BView::AttachedToWindow();
}

void
HeaderItem::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		// case MENU_FILE_OPEN:
		default:
			BView::MessageReceived(msg);
			break;
	}
}

void
HeaderItem::MouseDown(BPoint point)
{
	uint32 button;
	GetMouse(&point, &button);
	if (button == B_PRIMARY_MOUSE_BUTTON) {
		IsDown = true;
		OkToInvoke = true;
		Invalidate();
	}
}

void
HeaderItem::MouseUp(BPoint point)
{
	IsDown = false;
	Invalidate();

	if (OkToInvoke) {
		Invoke();
		OkToInvoke = false;
	}
}

void
HeaderItem::SetHidden(bool tohideornottohide)
{
	IsHidden = tohideornottohide;
}

void
HeaderItem::MouseMoved(BPoint point, uint32 transit, const BMessage* msg)
{
	uint32 button;
	GetMouse(&point, &button);
	switch (transit) {
		case B_ENTERED_VIEW:
		{
			// always have !IsDown - default state
			/*if(button == B_PRIMARY_MOUSE_BUTTON)
			{
				OkToInvoke = true;
				IsOver = true;
				IsDown = true;
				Invalidate();
			}
			else
			{*/
			IsOver = true;
			// OkToInvoke = true;
			// Invalidate();
			//}

		} break;
		case B_EXITED_VIEW:
		{
			IsOver = false;
			if (IsDown) {
				IsDown = false;
				Invalidate();
			}
			// Invalidate();
			OkToInvoke = false;
			IsDown = false;

		} break;
	}
	Parent()->MouseMoved(point, transit, msg);
}

void
HeaderItem::FrameResized(float w, float h)
{
	Invalidate();
	BView::FrameResized(w, h);
}

void
HeaderItem::Invoke()
{
	BMessage hh(InterfaceConstants::K_HANDLE_HIERARCHY);

	// show if hidden
	if (IsHidden) {
		BMessenger msgr(this);
		if (hh.AddBool(label, false) == B_OK)
			msgr.SendMessage(&hh);
		IsHidden = false;
	}
	// hide if shown
	else {
		BMessenger msgr(this);
		if (hh.AddBool(label, true) == B_OK)
			msgr.SendMessage(&hh);
		IsHidden = true;
	}
}
