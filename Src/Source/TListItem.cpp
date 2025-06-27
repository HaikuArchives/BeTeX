/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski               *
 *                                                               *
 * All rights reserved.                                          *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

#include "TListItem.h"
#include "TWindow.h"
TListItem::TListItem(SplitPane* sp, BScrollView* docScroll, BString label, Prefs* p)
	: BListItem()
{
	prefs = p;
	SP = sp;
	BRect r = docScroll->Bounds();
	BRect tvrect(0, 0, r.Width() - B_V_SCROLL_BAR_WIDTH, r.Height());
	BRect tr(tvrect);
	tr.InsetBy(10, 5);
	BFont font(be_fixed_font);
	font.SetSize(12);

	tv = new TexView(tvrect, tr, prefs);
	sv = new BScrollView("ScrollView2", tv, B_FOLLOW_LEFT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_FRAME_EVENTS, false, true, B_FANCY_BORDER);
	sv->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fname = label;
	// valid = false;
	SP->AddChildTwo(sv, false, false);
	NeedToSave = false;
	HasHome = false;
}
TListItem::TListItem(
	SplitPane* sp, BScrollView* docScroll, entry_ref* r, Prefs* p, bool IsTemplate = false)
{
	prefs = p;
	if (!IsTemplate)
		ref = *r;
	entry_ref temp_ref = *r;
	BEntry entry(&temp_ref, true);
	BEntry parent;
	char name[B_FILE_NAME_LENGTH];
	entry.GetParent(&parent);  // Get parent directory
	entry.GetName(name);
	fname << name;

	// BString title;
	// title << "BeTeX" << " - " << fname;
	// Window()->SetTitle(title.String());
	/*BFile file(&ref,B_READ_ONLY);
	if (file.InitCheck() == B_OK)
	{
	*/
	SP = sp;
	BRect rct = docScroll->Bounds();
	BRect tvrect(0, 0, rct.Width() - B_V_SCROLL_BAR_WIDTH, rct.Height());
	BRect tr(tvrect);
	tr.InsetBy(10, 5);
	BFont font(be_fixed_font);
	font.SetSize(12);

	tv = new TexView(tvrect, tr, prefs);
	tv->LoadFile(&temp_ref);
	sv = new BScrollView("ScrollView2", tv, B_FOLLOW_LEFT | B_FOLLOW_TOP,
		B_WILL_DRAW | B_FRAME_EVENTS, false, true, B_FANCY_BORDER);
	sv->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	/*off_t length;
	file.GetSize(&length);		// Get the file length;
	int32 len = length;
	tv->SetText(&file,0,len);//,&tra);
	*/
	// valid = true;
	SP->AddChildTwo(sv, false, false);

	//}
	NeedToSave = false;	 // IsTemplate;
	HasHome = !IsTemplate;
}
TListItem::~TListItem()
{
	sv->RemoveChild(tv);
	SP->RemoveChild(sv);
	delete tv;
	delete sv;
}
entry_ref
TListItem::GetRef()
{
	return ref;
}
void
TListItem::DrawItem(BView* owner, BRect frame, bool complete = false)
{
	rgb_color color;
	rgb_color unsaved_color = {255, 0, 0};
	rgb_color unsaved_text_color = {0, 0, 255};
	color = owner->ViewColor();
	if (IsSelected() || complete) {
		if (IsSelected()) {
			BFont font(be_bold_font);
			font.SetSize(12);
			if (NeedToSave) {
				font.SetFace(B_ITALIC_FACE | B_BOLD_FACE);
				color = ui_color(B_WINDOW_TAB_COLOR);
			} else
				color = ui_color(B_WINDOW_TAB_COLOR);  // owner->ViewColor();//highlight_color;
			owner->SetFont(&font);
		} else {
			BFont font(be_plain_font);
			font.SetSize(12);
			if (NeedToSave) {
				font.SetFace(B_ITALIC_FACE);
				color
					= owner
						  ->ViewColor();  // tint_color(unsaved_color,B_LIGHTEN_2_TINT);//ui_color(B_WINDOW_TAB_COLOR);
			} else
				color = owner->ViewColor();	 // owner->ViewColor();//highlight_color;

			owner->SetFont(&font);
		}
		owner->SetHighColor(color);
		owner->SetLowColor(color);
		// owner->SetLowColor(255,255,255);
		owner->FillRect(frame);
	} else {
		BFont font(be_plain_font);
		font.SetSize(12);
		if (NeedToSave) {
			font.SetFace(B_ITALIC_FACE);
		}
		owner->SetFont(&font);
		owner->SetHighColor(color);
		owner->SetLowColor(color);
		owner->FillRect(frame);
	}
	owner->MovePenTo(frame.left + 4, frame.bottom - 2);

	if (IsEnabled()) {
		// set highcolor for font
		owner->SetHighColor(0, 0, 0);
	} else {
		// set highcolor for font
		owner->SetHighColor(0, 0, 0);
		// owner->SetLowColor(255,255,255);
	}
	if (NeedToSave) {
		// color = unsaved_color;

		owner->SetHighColor(unsaved_text_color);
		// owner->InvertRect(frame);
	}
	owner->SetLowColor(color);
	owner->DrawString(fname.String());
	// BFont font(be_plain_font);
	// font.SetSize(12);
	// owner->SetFont(&font);
}
void
TListItem::SetRef(entry_ref* r)
{
	ref = *r;
	BPath path(&ref);
	fname = "";
	fname << path.Leaf();
	// NeedToSave = false;
	HasHome = true;

	// valid = true;
	// Invalidate();
}
const char*
TListItem::FName()
{
	return fname.String();
}
void
TListItem::SetFName(BString name)
{
	fname = name;
}
void
TListItem::ShowTextView()
{
	SP->AddChildTwo(sv, true, true);
	// if page is big, we want it at the top
	sv->ScrollBar(B_VERTICAL)->SetValue(0);
}
void
TListItem::HideTextView()
{
	SP->AddChildTwo(sv, true, false);
	sv->ScrollBar(B_VERTICAL)->SetValue(0);
}
TexView*
TListItem::TextView()
{
	return tv;
}

bool
TListItem::IsSaveNeeded()
{
	return NeedToSave;
}
void
TListItem::SetSaveNeeded(bool flag)
{
	NeedToSave = flag;
}
bool
TListItem::IsHomely()
{
	return HasHome;
}
void
TListItem::SetHomely(bool flag)
{
	HasHome = flag;
}
