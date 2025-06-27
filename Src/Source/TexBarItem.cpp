#include "TexBarItem.h"
TexBarItemView::TexBarItemView(BRect f, const char* n, uint32 rm, uint32 fl)
	: BView(f, n, rm, fl)
{
}

void
TexBarItemView::MouseMoved(BPoint point, uint32 transit, const BMessage* msg)
{
	/*switch(transit)
	{
		case B_ENTERED_VIEW:
		{
			MakeFocus(true);
		}break;
		case B_EXITED_VIEW:
		{
			MakeFocus(false);
		}break;
	}*/
	Parent()->MouseMoved(point, transit, msg);
}

TexBarItem::TexBarItem(BRect r, const char* l, TexBarItemView* p)
	: BView(r, "TexBarItem", B_FOLLOW_TOP, B_WILL_DRAW)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	r = Bounds();
	label = l;
	ItemHeight = 20;
	if (p) {
		plateau = p;
		PlateauHeight = plateau->Bounds().Height() - 1;
		// plateau->ResizeTo(r.Width(),PlateauHeight);

		AddChild(plateau);
	} else
		PlateauHeight = 0;

	IsHidden = true;
	IsDown = false;
	IsOver = false;
	labelRect = BRect(r.left, r.top, r.right, r.top + ItemHeight);
	// We're hidden by default
	ResizeTo(r.Width(), ItemHeight);
}

float
TexBarItem::Height()
{
	return IsHidden ? ItemHeight : PlateauHeight + ItemHeight + 1;
}

void
TexBarItem::ScrollBy(float h, float v)
{
	BView::ScrollBy(h, v);
	// Invalidate();
}

void
TexBarItem::ScrollTo(BPoint where)
{
	BView::ScrollTo(where);
	// Invalidate();
}

void
TexBarItem::Draw(BRect r)
{
	r = Bounds();

	/*	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_LIGHTEN_2_TINT));
		StrokeLine(BPoint(r.left,r.top),BPoint(r.left,r.bottom));
		StrokeLine(BPoint(r.left,r.top),BPoint(r.right,r.top));

		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_2_TINT));
		StrokeLine(BPoint(r.left,r.bottom),BPoint(r.right,r.bottom));
		StrokeLine(BPoint(r.right,r.top+1),BPoint(r.right,r.bottom));
	*/
	rgb_color low;
	for (int i = 0; i <= 20; i++) {
		SetHighColor(255, i * 12, 0);
		if (IsDown) {  //{
			rgb_color invert_me = HighColor();
			invert_me.red = 255 - invert_me.red;
			invert_me.green = 255 - invert_me.green;
			invert_me.blue = 255 - invert_me.blue;
			SetHighColor(invert_me);
		}
		if (i == 10)
			low = HighColor();
		// SetDrawingMode(B_OP_INVERT);
		// SetHighColor(tint_color(HighColor(),B_DARKEN_1_TINT));
		// }
		StrokeLine(BPoint(r.left + 1, ItemHeight - i), BPoint(r.right - 1, ItemHeight - i));

		if (i == 0) {
			SetHighColor(tint_color(HighColor(), B_DARKEN_2_TINT));
			StrokeLine(BPoint(r.left + 1, ItemHeight - i), BPoint(r.right - 1, ItemHeight - i));
		} else if (i == 20) {
			SetHighColor(tint_color(HighColor(), B_LIGHTEN_2_TINT));
			StrokeLine(BPoint(r.left + 1, ItemHeight - i), BPoint(r.right - 1, ItemHeight - i));
		}

		// left highlight
		SetHighColor(tint_color(HighColor(), B_LIGHTEN_2_TINT));
		StrokeLine(BPoint(r.left, ItemHeight - i), BPoint(r.left, ItemHeight - i));
		// right shadow
		SetHighColor(tint_color(HighColor(), B_DARKEN_2_TINT));
		StrokeLine(BPoint(r.right, ItemHeight - i), BPoint(r.right, ItemHeight - i));
	}
	BFont font(be_bold_font);
	font.SetSize(12);
	SetFont(&font);

	SetHighColor(ColorConstants::K_BLACK);
	SetLowColor(low);
	MovePenTo(5, ItemHeight - 5);
	DrawString(label);
}

/*void TexBarItem::Hide()
{
	BView::Hide();
}

void TexBarItem::Show()
{
	BView::Show();
}
*/

void
TexBarItem::AttachedToWindow()
{
	// SetViewColor(0,0,255);
	BView::AttachedToWindow();
}

void
TexBarItem::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		// case MENU_FILE_OPEN:
		default:
			BView::MessageReceived(msg);
			break;
	}
}

void
TexBarItem::MouseDown(BPoint point)
{
	if (labelRect.Contains(point)) {
		uint32 button;
		GetMouse(&point, &button);
		if (button == B_PRIMARY_MOUSE_BUTTON) {
			IsDown = true;
			OkToInvoke = true;
			Invalidate();
		}
	}
}

void
TexBarItem::MouseUp(BPoint point)
{
	// uint32 button;
	// GetMouse(&point,&button);
	// if(button == B_PRIMARY_MOUSE_BUTTON)
	//{
	if (labelRect.Contains(point)) {
		IsDown = false;
		Invalidate();

		if (OkToInvoke) {
			Invoke();
			OkToInvoke = false;
		}
	}
	//}
}

void
TexBarItem::MouseMoved(BPoint point, uint32 transit, const BMessage* msg)
{
	uint32 button;
	GetMouse(&point, &button);
	switch (transit) {
		case B_ENTERED_VIEW:
		{
			if (labelRect.Contains(point)) {
				// always have !IsDown - default state
				if (button == B_PRIMARY_MOUSE_BUTTON) {
					OkToInvoke = true;
					IsDown = true;
					Invalidate();
				} else {
					IsOver = true;
					// Invalidate();
				}
			}
			// Parent()->MakeFocus(true);
		} break;
		case B_EXITED_VIEW:
		{
			// if(labelRect.Contains(point))
			//{
			IsOver = false;
			if (IsDown) {
				IsDown = false;
				Invalidate();
			}
			// Invalidate();
			OkToInvoke = false;
			// Parent()->MakeFocus(false);
		} break;
	}
	// if(transit == B_EXITED_VIEW
	/*else
	{
		IsDown = false;
		Invalidate();
	}
	*/
	Parent()->MouseMoved(point, transit, msg);
}

void
TexBarItem::Invoke()
{
	// hide if shown
	if (IsHidden) {
		ResizeBy(0, PlateauHeight);
		BMessenger msgr(this);
		msgr.SendMessage(new BMessage(InterfaceConstants::K_HANDLE_HIERARCHY));
		IsHidden = false;
	}
	// show if hidden
	else {
		ResizeBy(0, -PlateauHeight);
		BMessenger msgr(this);
		msgr.SendMessage(new BMessage(InterfaceConstants::K_HANDLE_HIERARCHY));
		IsHidden = true;
	}
}
