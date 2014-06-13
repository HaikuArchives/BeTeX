// =============================================================================
//
// libwalter Toolbar.cpp
//
// WToolbar, a toolbar widget
//
// Page width 80, tab width 4, encoding UTF-8, line endings LF.
//
// Original author:
//     Gabriele Biffi - http://www.biffuz.it/
// Contributors:
//
// Released under the terms of the MIT license.
//
// =============================================================================

// Standard C++ headers
#include <typeinfo>

// Standard C headers
#include <string.h>

// Haiku headers
#include <Bitmap.h>
#include <Handler.h>
#include <Looper.h>
#include <Menu.h>
#include <PropertyInfo.h>
#include <Window.h>

// libwalter headers
#include "Toolbar.h"
#include "ToolbarControl.h"
#include "ToolbarSupport.h"

// =============================================================================
// WToolbar
// =============================================================================

/*static property_info kWToolbarPropList[] = {
	{ "enabled",
	  { B_GET_PROPERTY, B_SET_PROPERTY, 0 },
	  { B_DIRECT_SPECIFIER, 0 },
	  NULL
	},
	{}
};*/

// Constructors and destructor

/* Creates a new, empty toolbar. Style can be W_TOOLBAR_STYLE_FLAT (default),
 * W_TOOLBAR_STYLE_3D, or W_TOOLBAR_STYLE_MENU (for a description of the styles,
 * see SetStyle()). Alignment can be W_TOOLBAR_HORIZONTAL (default, items are
 * arranged in rows) or W_TOOLBAR_VERTICAL (items are arranged in columns).
 * Border can be B_FANCY_BORDER (default, a 3D border), B_PLAIN_BORDER (a line),
 * or B_NO_BORDER. If an invalid value is given, the default will be used
 * instead. Frame, name, and resizing_mode are passed to BView's constructor
 * unmodified; the flags B_FRAME_EVENTS, B_FULL_UPDATE_ON_RESIZE, and
 * B_WILL_DRAW may be added or removed as needed by other methods.
 * The view color will be set to B_TRANSPARENT_32_BIT to avoid flickering.
 * 
 * Auto size	false
 * Enabled		true
 * Alignment	W_TOOLBAR_HORIZONTAL
 * 
 */
WToolbar::WToolbar(BRect frame, const char *name, int style,
	WToolbarAlignment alignment, bool auto_size, border_style border,
	uint32 resizing_mode, uint32 flags) :
	BView(frame, name, resizing_mode,
		flags | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW)
{
	_init_object();

	SetAlignment(alignment);
	SetAutoSize(auto_size);
	SetBorder(border);
	SetStyle(style);

	fDisableUpdate = false;
	Update();
}

WToolbar::WToolbar(BMessage *archive) :
	BView(archive)
{
	WToolbarLabelPosition labelPosition;
	float margin, padding, picSize;
	WToolbarAlignment alignment;
	BArchivable *archivable;
	bool autoSize, enabled;
	border_style border;
	WToolbarItem *item;
	BMessage message;
	BBitmap *backPic;
	int32 index;
	int style;

	_init_object();

	// Properties

	if (archive->FindBool("WToolbar::autosize", &autoSize) == B_OK)
		SetAutoSize(autoSize);

	if (archive->FindBool("WToolbar::enabled", &enabled) == B_OK)
		SetEnabled(enabled);

	// Appearance

	fDisableStyling = true;
	if (archive->FindInt32("WToolbar::style", (int32*)(&style)) == B_OK)
		SetStyle(style);
	fDisableStyling = false;

	if (archive->FindInt32("WToolbar::alignment",
	  (int32*)(&alignment)) == B_OK)
		SetAlignment(alignment);

	if (archive->FindMessage("WToolbar::background_bitmap",
	  &message) == B_OK) {
		archivable = instantiate_object(&message);
		if (archivable != NULL) {
			backPic = dynamic_cast<BBitmap*>(archivable);
			if (backPic != NULL) {
				if (backPic->IsValid())
					SetBackgroundBitmap(backPic);
				else
					delete backPic;
			}
			else
				delete archivable;
		}
	}

	if (archive->FindInt32("WToolbar::border", (int32*)(&border)) == B_OK)
		SetBorder(border);

	if (archive->FindInt32("WToolbar::label_position",
	  (int32*)(&labelPosition)) == B_OK)
		SetLabelPosition(labelPosition);

	if (archive->FindFloat("WToolbar::margin", &margin) == B_OK)
		SetMargin(margin);

	if (archive->FindFloat("WToolbar::padding", &padding) == B_OK)
		SetPadding(padding);

	if (archive->FindFloat("WToolbar::picture_size", &picSize) == B_OK)
		SetPictureSize(picSize);

	// Items

	index = 0;
	while (archive->FindMessage("WToolbar::item", index,
	  &message) == B_OK) {
		archivable = instantiate_object(&message);
		if (archivable != NULL) {
			item = dynamic_cast<WToolbarItem*>(archivable);
			if (item != NULL) {
				if (!AddItem(item, item->fLine))
					delete item;
			}
			else
				delete archivable;
		}
		index++;
	}

	// Force the update
	fDisableUpdate = false;
	Update();
}

WToolbar::~WToolbar()
{
	fDisableUpdate = true;

	// Delete all the items
	while (fLines.size() > 0)
		DeleteLine(0);

	// Other cleanups
	if (fAppearance.fBackBitmap != NULL)
		delete fAppearance.fBackBitmap;
}

// Private =====================================================================

void WToolbar::_draw_item(unsigned line, unsigned position, BRect updateRect)
{
	// Beware, this function does not perform any sanity checking!!!
	// TODO set the clipping region and update rect
	WToolbarItem *item = (*(fLines[line]))[position];
	BPoint origin = Origin();
	SetOrigin(item->fFrame.LeftTop());
	PushState();
	item->Draw(this, item->Bounds());
	PopState();
	SetOrigin(origin);
	Sync();
}

void WToolbar::_init_object(void)
{
	// Internals
	fDisableUpdate = true;
	fDisableStyling = false;
	fMouseDownItem = NULL;
	fMouseOverItem = NULL;

	// Properties
	fAutoSize = false;
	fEnabled = true;

	// Appearance (flat style)
	fAppearance.fAlignment = W_TOOLBAR_HORIZONTAL;
	fAppearance.fBackBitmap = NULL;
	fAppearance.fBorder = B_FANCY_BORDER;
	fAppearance.fLabelPosition = W_TOOLBAR_LABEL_NONE;
	fAppearance.fMargin = 0.0;
	fAppearance.fPadding = 2.0;
	fAppearance.fPicSize = static_cast<float>(W_TOOLBAR_PICTURE_XSMALL);
	fAppearance.fStyle = W_TOOLBAR_STYLE_FLAT;

	// BView properties
	SetViewColor(B_TRANSPARENT_32_BIT);
}

// Protected ===================================================================

void WToolbar::AssertLocked(void)
{
	// Why BView::check_lock() is private and not protected???
	if (Looper() == NULL) return;
	if (!Looper()->IsLocked())
		debugger("looper must be locked");
}

/* Return the thickness, in view's units, of the border as drawn by
 * DrawBorder(), used only if the border is set to B_FANCY_BORDER. The default
 * implementation returns 2.0. You need to overload this if you overload
 * DrawBorder() and the your border's thickness is different from 2.0 units.
 * Currently, the value will be rounded down. 
 */
float WToolbar::BorderThickness(void)
{
	return 2.0;
}

/* Draw the background of the toolbar. The default implementation draws the
 * background bitmap tiled or, if there is no background bitmap, just fills the
 * area with the low color. Can be overloaded to draw a different background.
 * Border and items are drawn over the background; items can be translucent.
 */
void WToolbar::DrawBackground(BRect updateRect)
{
	if (fAppearance.fBackBitmap != NULL) {
		const float kWidth = fAppearance.fBackBitmap->Bounds().Width(),
			kHeight = fAppearance.fBackBitmap->Bounds().Height(),
			kBorder = (fAppearance.fBorder == B_FANCY_BORDER ?
				ceil(BorderThickness()) :
				(fAppearance.fBorder == B_PLAIN_BORDER ? 1.0 : 0.0));
		float x, y;
		for (y = floor((updateRect.top - kBorder) / kHeight) * kHeight +
		  kBorder; y < updateRect.bottom; y += kHeight) {
			for (x = floor((updateRect.left - kBorder) / kWidth) * kWidth +
			  kBorder; x <= updateRect.right; x += kWidth)
				DrawBitmap(fAppearance.fBackBitmap, BPoint(x, y));
		}
	}
	else
		FillRect(updateRect, B_SOLID_LOW);
}

/* Draw the border of the toolbar; it is called only if border is
 * B_FANCY_BORDER. The default implementation draws a 3D border with a thickness
 * of 2.0 units. Can be overloaded to draw a different border. If your border
 * thickness is different than 2.0 units, you'll need to overload
 * BorderThickness() as well. The border is drawn after the background, but note
 * that the default DrawBackground() may not cover the border area (it assumes
 * that the border is not translucent).
 */
void WToolbar::DrawBorder(BRect updateRect)
{
	WToolbarSupport::Draw3DBorder(this, Bounds());
}

// BArchivable hooks ===========================================================

/* Archive the toolbar to the target BMessage (must be valid). If deep is true,
 * also archive all the items. Adds the following fields if their values are
 * different from the default:
 * WToolbar::autosize			bool
 * WToolbar::enabled			bool
 * WToolbar::alignment			int32
 * WToolbar::background_bitmap	message
 * WToolbar::border				int32
 * WToolbar::label_position		int32
 * WToolbar::margin				float
 * WToolbar::padding			float
 * WToolbar::picture_size		float
 * WToolbar::style				int32
 * WToolbar::item				message		one for each item
 * Return B_OK if successful, or an error code.
 */
status_t WToolbar::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	// Archive ancestor
	status = BView::Archive(archive, false);

	// Archive properties

	if (status == B_OK && fAutoSize)
		status = archive->AddBool("WToolbar::autosize", true);

	if (status == B_OK && !fEnabled)
		status = archive->AddBool("WToolbar::enabled", false);

	// Archive appearance

	if (status == B_OK && fAppearance.fAlignment != W_TOOLBAR_HORIZONTAL)
		status = archive->AddInt32("WToolbar::alignment",
			static_cast<int32>(fAppearance.fAlignment));

	if (status == B_OK && fAppearance.fBackBitmap != NULL) {
		 if (fAppearance.fBackBitmap->IsValid()) {
			BMessage bmp;
			status = fAppearance.fBackBitmap->Archive(&bmp);
			if (status == B_OK)
				status = archive->AddMessage("WToolbar::background_bitmap",
					&bmp);
		}
	}

	if (status == B_OK && fAppearance.fBorder != B_FANCY_BORDER)
		status = archive->AddInt32("WToolbar::border",
			static_cast<int32>(fAppearance.fBorder));

	if (status == B_OK && fAppearance.fLabelPosition != W_TOOLBAR_LABEL_NONE)
		status = archive->AddInt32("WToolbar::label_position",
			static_cast<int32>(fAppearance.fLabelPosition));

	if (status == B_OK && fAppearance.fMargin != 0.0)
		status = archive->AddFloat("WToolbar::margin", fAppearance.fMargin);

	if (status == B_OK && fAppearance.fPadding != 2.0)
		status = archive->AddFloat("WToolbar::padding", fAppearance.fPadding);

	if (status == B_OK && fAppearance.fPicSize != W_TOOLBAR_PICTURE_XSMALL)
		status = archive->AddFloat("WToolbar::picture_size",
			fAppearance.fPicSize);

	if (status == B_OK && fAppearance.fStyle != W_TOOLBAR_STYLE_FLAT)
		status = archive->AddInt32("WToolbar::style", fAppearance.fStyle);

	// Archive items

	if (deep) {
		for (unsigned l = 0; l < fLines.size() && status == B_OK; l++) {
			WToolbarLine *line = fLines[l];
			const unsigned kItems = line->size();
			for (unsigned i = 0; i < kItems && status == B_OK; i++) {
				BMessage item;
				status = (*line)[i]->Archive(&item, true);
				if (status == B_OK)
					status = archive->AddMessage("WToolbar::item", &item);
			}
		}
	}

	return status;
}

BArchivable * WToolbar::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "WToolbar") ?
		new WToolbar(archive) : NULL);
}

// BHandler hooks ==============================================================

/*status_t WToolbar::GetSupportedSuites(BMessage *message)
{
	BPropertyInfo pi(kWToolbarPropList);
	message->AddString("suites", "suite/vnd.AKToolkit.Toolbar");
	message->AddFlat("messages", &pi);
	return BView::GetSupportedSuites(message);
}*/

void WToolbar::MessageReceived(BMessage *message)
{
	BMessage *reply = NULL;
	bool handled = false;

	/*switch (message->what) {
		case B_GET_PROPERTY:
		case B_SET_PROPERTY: {
			const char *property;
			int32 cur;
			if (message->GetCurrentSpecifier(&cur, NULL, NULL,
			  &property) == B_OK) {
				// TODO handle all other properties
				if (strcmp(property, "enabled")) {
					if (message->what == B_GET_PROPERTY) {
						reply = new BMessage(B_REPLY);
						reply->AddBool("result", Enabled());
						handled = true;
					}
					else {
						bool enabled;
						if (message->FindBool("data", &enabled) == B_OK) {
							SetEnabled(enabled);
							reply = new BMessage(B_REPLY);
							reply->AddInt32("error", B_OK);
							handled = true;
						}
					}
				}
			}
			} break;
	}*/

	if (handled) {
		if (reply != NULL) {
			message->SendReply(reply);
			delete reply;
		}
	}
	else
		BView::MessageReceived(message);
}

/*BHandler * WToolbar::ResolveSpecifier(BMessage *message, int32 index,
	BMessage *specifier, int32 what, const char *property)
{
	BPropertyInfo pi(kWToolbarPropList);
	if (pi.FindMatch(message, index, specifier, what, property) >= 0)
		return this;
	return BView::ResolveSpecifier(message, index, specifier, what, property);
}*/

// BView hooks =================================================================

/* Set the window as the target for all the controls which doesn't have a valid
 * target yet. Note that if you detach the toolbar from the window the controls'
 * target will _not_ be deleted, so when you attach it to another window it will
 * not become the new target (this is a wanted feature). To change to target for
 * all the controls, use SetControlsTarget().
 */
void WToolbar::AttachedToWindow(void)
{
	BView::AttachedToWindow();
	if (!fTarget.IsValid()) {
		for (unsigned l = 0; l < fLines.size(); l++) {
			WToolbarLine *line = fLines[l];
			const unsigned kItems = line->size();
			WToolbarControl *control;
			unsigned i;
			SetTarget(Window());
			for (i = 0; i < kItems; i++) {
				control = dynamic_cast<WToolbarControl*>((*line)[i]);
				if (control != NULL && !(control->Messenger().IsValid()))
					control->SetTarget(fTarget);
			}
		}
	}
}

void WToolbar::DetachedFromWindow(void)
{
	BView::DetachedFromWindow();
}

void WToolbar::Draw(BRect updateRect)
{
	// Background
	PushState();
	DrawBackground(updateRect);
	PopState();

	// Border
	switch (fAppearance.fBorder) {
		case B_FANCY_BORDER:
			PushState();
			DrawBorder(updateRect);
			PopState();
			break;
		case B_PLAIN_BORDER:
			StrokeRect(Bounds(), B_SOLID_HIGH);
		default:
			break;
	}		

	// Items
	for (unsigned l = 0; l < fLines.size(); l++) {
		WToolbarLine *line = fLines[l];
		const unsigned kItems = line->size();
		for (unsigned i = 0; i < kItems; i++) {
			WToolbarItem *item = (*line)[i];
			if (item->fFrame.Intersects(updateRect) && item->fVisible)
				_draw_item(l, i, updateRect);
		}
	}
}

/* Return the preferred size of the toolbar. Includes all the items, margin,
 * padding, and border. It works only if the toolbar is attached to a window.
 * If you don't need one of the values, it can be NULL.
 */
void WToolbar::GetPreferredSize(float *width, float *height)
{
	float w = 0.0, h = 0.0, total = 0.0, max = 0.0;
	unsigned l, i, visible_lines = 0;
	WToolbarLine *line;
	WToolbarItem *item;

	if (Window() == NULL) {
		if (width != NULL) *width = -1.0;
		if (height != NULL) *height = -1.0;
	}

	// Items
	if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL) {
		// Toolbar width is the sum of the widths of the visible lines; the
		// width of each line is the width of any of its item (all the items
		// have the same width)
		// Toolbar height is the height of the tallest line; the height of each
		// line is the sum of the heights of its visible items
		for (l = 0; l < fLines.size(); l++) {
			if (LineVisible(l)) {
				line = fLines[l];
				const unsigned kItems = line->size();
				visible_lines++;
				h = 0.0;
				for (i = 0; i < kItems; i++) {
					item = (*line)[i];
					if (item->fVisible) {
						w = item->fFrame.Width() + 1.0;
						h += item->fFrame.Height() + 1.0 + fAppearance.fMargin;
					}
				}
				total += w + fAppearance.fMargin * 2.0;	// width
				max = (h > max ? h : max);				// height
			}
		}
		if (visible_lines > 0) {
			w = total;
			h = max + fAppearance.fMargin;
		}
	}
	else { // W_TOOLBAR_HORIZONTAL
		// Toolbar width is the width of the widest line; the width of each line
		// is the sum of the widths of its visible items
		// Toolbar height is the sum of the heights of the visible lines; the
		// height of each line is the height of any of its item (all the items
		// have the same height)
		for (l = 0; l < fLines.size(); l++) {
			if (LineVisible(l)) {
				line = fLines[l];
				const unsigned kItems = line->size();
				visible_lines++;
				w = 0.0;
				for (i = 0; i < kItems; i++) {
					item = (*line)[i];
					if (item->fVisible) {
						w += item->fFrame.Width() + 1.0 + fAppearance.fMargin;
						h = item->fFrame.Height() + 1.0;
					}
				}
				max = (w > max ? w : max);				// width
				total += h + fAppearance.fMargin * 2.0;	// height
			}
		}
		if (visible_lines > 0) {
			w = max + fAppearance.fMargin;
			h = total;
		}
	}

	// Border
	switch (fAppearance.fBorder) {
		case B_FANCY_BORDER:
			h += BorderThickness() * 2.0;
			w += BorderThickness() * 2.0;
			break;
		case B_PLAIN_BORDER:
			h += 2.0;
			w += 2.0;
			break;
		default:
			break;
	}

	// Padding
	h += fAppearance.fPadding * 2.0;
	w += fAppearance.fPadding * 2.0;			

	if (height != NULL) *height = h - 1.0;
	if (width != NULL) *width = w - 1.0;
}

void WToolbar::FrameResized(float width, float height)
{
	if (!fAutoSize)
		Update();
	BView::FrameResized(width, height);
}

void WToolbar::MouseDown(BPoint point)
{
	if (Looper() == NULL) return;

	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS |
		B_SUSPEND_VIEW_FOCUS);

	int32 button = B_PRIMARY_MOUSE_BUTTON;
	BMessage *msg;

	// Find the mouse button
	msg = Looper()->CurrentMessage();
	if (msg != NULL)
		msg->FindInt32("buttons", &button);

	// Left button
	if (button == B_PRIMARY_MOUSE_BUTTON) {

		// If the mouse is over a item, tell the item about this
		if (fMouseOverItem != NULL && fEnabled) {
			fMouseOverItem->MouseDown(point - fMouseOverItem->fFrame.LeftTop());
			fMouseDownItem = fMouseOverItem;
		}
	}
}

void WToolbar::MouseMoved(BPoint point, uint32 transit,
	const BMessage *message)
{
	if (Looper() == NULL) return;

	bool disableUpdate = fDisableUpdate;
	fDisableUpdate = true;

	if (fMouseDownItem == NULL) {

		int32 buttons = 0;
		BMessage *msg;

		// Pressed mouse buttons
		msg = Looper()->CurrentMessage();
		if (msg != NULL)
			msg->FindInt32("buttons", &buttons);

		if (buttons == 0) {
			if (transit == B_ENTERED_VIEW || transit == B_INSIDE_VIEW) {
				WToolbarItem *old_over = fMouseOverItem;
				fMouseOverItem = ItemAt(point);
				if (fMouseOverItem != old_over) {
					// Remove mouse over from previous item
					if (old_over != NULL)
						old_over->MouseMoved(point - old_over->fFrame.LeftTop(),
							B_EXITED_VIEW, message);
					// Assign mouse over to new item
					if (fMouseOverItem != NULL)
						fMouseOverItem->MouseMoved(
							point - fMouseOverItem->fFrame.LeftTop(),
							B_ENTERED_VIEW, message);
				}
				else {
					// Tell the item that the mouse has moved
					if (fMouseOverItem != NULL)
						fMouseOverItem->MouseMoved(
							point - fMouseOverItem->fFrame.LeftTop(),
							B_INSIDE_VIEW, message);
				}
			}
			else {
				// B_EXITED_VIEW or B_OUTSIDE_VIEW, remove focus
				if (fMouseOverItem != NULL) {
					fMouseOverItem->MouseMoved(
						point - fMouseOverItem->fFrame.LeftTop(),
						B_EXITED_VIEW, message);
					fMouseOverItem = NULL;
				}
			}
		}
	}

	// If the mouse is down, we need to remove the mouse over from the currently
	// pressed item, but not assign it to a new one.
	else {
		if (transit == B_ENTERED_VIEW || transit == B_INSIDE_VIEW) {
			WToolbarItem *new_over = ItemAt(point);
			if (new_over != fMouseDownItem) {
				// Remove mouse over from pressed item
				fMouseDownItem->MouseMoved(point -
					fMouseDownItem->fFrame.LeftTop(), B_EXITED_VIEW, message);
			}
			else {
				// Tell the item that the mouse has moved
				fMouseDownItem->MouseMoved(point -
					fMouseDownItem->fFrame.LeftTop(), B_INSIDE_VIEW, message);
			}
		}
		else {
			// B_EXITED_VIEW or B_OUTSIDE_VIEW, remove focus
			fMouseDownItem->MouseMoved(point - fMouseDownItem->fFrame.LeftTop(),
				B_EXITED_VIEW, message);
		}
	}

	fDisableUpdate = disableUpdate;
}

void WToolbar::MouseUp(BPoint point)
{
	if (Looper() == NULL) return;

	WToolbarItem *new_over;
	BMessage *msg;

	msg = Looper()->CurrentMessage();

	// If an item is pressed, tell it what is happening
	if (fMouseDownItem != NULL) {
		fMouseDownItem->MouseUp(point - fMouseDownItem->fFrame.LeftTop());
		fMouseDownItem = NULL;
	}

	// Check which item the mouse is over
	new_over = ItemAt(point);
	if (new_over != fMouseOverItem) {
		if (fMouseOverItem != NULL)
			fMouseOverItem->MouseMoved(point - fMouseOverItem->fFrame.LeftTop(),
				B_EXITED_VIEW, msg);
		if (new_over != NULL)
			new_over->MouseMoved(point - new_over->fFrame.LeftTop(),
				B_ENTERED_VIEW, msg);
		fMouseOverItem = new_over;
	}
	else {
		if (fMouseOverItem != NULL)
			fMouseOverItem->MouseMoved(point - fMouseOverItem->fFrame.LeftTop(),
				B_INSIDE_VIEW, msg);
	}
}

// Items management ============================================================

/* Add an item to the toolbar. Return true if the operation is successful or the
 * item already belongs to the toolbar.
 * The item is inserted at the given line and position (indexes start from 0);
 * if position is 0, the item will be inserted at the beginning of the line, or
 * if it is greater or equal to the number of items in the line, or
 * W_TOOLBAR_LAST_POSITION, it will be inserted at the end; if it is negative, a
 * new line will be created and inserted at the given line index, and the item
 * will be the first of this new line. The items in the lines below will have
 * their line index increased.
 * If line index is equal or greater than the number of lines, or
 * W_TOOLBAR_LAST_LINE,the item is added at the end of the last line.
 * If line index is negative, a new line is created below the existing lines and
 * the item is added as the first item of it.
 * There cannot be more than W_TOOLBAR_MAX_LINES and each line cannot have more
 * have more than W_TOOLBAR_MAX_ITEMS_PER_LINE; the method returns false if one
 * of these conditions occurs.
 * Note: the line number stored in the item will be ignored and the correct
 * value will be assigned to it.
 */
bool WToolbar::AddItem(WToolbarItem *item, int line, int position)
{
	AssertLocked();
	if (item == NULL) return false;
	if (item->Toolbar() != NULL)
		return (item->Toolbar() == this);

	bool disableUpdate = fDisableUpdate;
	const unsigned kLines = fLines.size();
	fDisableUpdate = true;
	
	// There are no lines, or user asked for a new line after the last one;
	// create a new line, add the item at the beginning of it, and add the line
	// below all the other lines
	if (kLines == 0 || line < 0) {
		if (kLines >= W_TOOLBAR_MAX_LINES) {
			fDisableUpdate = disableUpdate;
			return false;
		}
		WToolbarLine *newLine = new WToolbarLine;
		item->fLine = kLines;
		newLine->push_back(item);
		fLines.push_back(newLine);
	}
	else 
	{
		//User asked to insert the item into an existing line, or to create a line
		//in the middle
		if (position >= 0) 
		{
			// Insert the item at the asked line and position if possible
			unsigned lineIndex = ((unsigned)line < fLines.size() ?
				(unsigned)line : fLines.size());
			WToolbarLine *_line = fLines[lineIndex];
			if (_line->size() >= W_TOOLBAR_MAX_ITEMS_PER_LINE) {
				fDisableUpdate = disableUpdate;
				return false;
			}
			item->fLine = lineIndex;
			if ((unsigned)position < _line->size())
				_line->insert(_line->begin() + (position > 0 ? position : 0),
					item);
			else
				_line->push_back(item);
		}
		else 
		{ // position < 0
			// Create a new line
			if (fLines.size() > W_TOOLBAR_MAX_LINES) 
			{
				fDisableUpdate = disableUpdate;
				return false;
			}
			WToolbarLine *_line = new WToolbarLine;
			item->fLine = line;
			_line->push_back(item);
			fLines.insert(fLines.begin() + line, _line);
			// Increase the line index of the items in the lines below
			for (unsigned l = line + 1; l < fLines.size(); l++) {
				_line = fLines[l];
				const unsigned kItems = _line->size();
				for (unsigned i = 0; i < kItems; i++)
					(*_line)[i]->fLine++;
			}
		}
	}

	item->fToolbar = this;
	item->AttachedToToolbar();

	fDisableUpdate = disableUpdate;
	Update();
	return true;
}

/* Return the total number of items in the toolbar. If visibleOnly is true,
 * return the total number of visible items.
 */
unsigned WToolbar::CountItems(bool visibleOnly)
{
	unsigned items = 0;
	for (unsigned l = 0; l < fLines.size(); l++)
		items += CountItemsInLine(l, visibleOnly);
	return items;
}

/* Return the number of items in the line. If visibleOnly is true, return the
 * number of visible items. If no items are visible, the line is not visible.
 * If there is no a line with the given index, return 0. Line index starts
 * from 0.
 */
unsigned WToolbar::CountItemsInLine(unsigned line, bool visibleOnly)
{
	if (line >= fLines.size())
		return 0;
	if (!visibleOnly)
		return fLines[line]->size();
	WToolbarLine *_line = fLines[line];
	const unsigned kItems = _line->size();
	unsigned i, items = 0;
	for (i = 0; i < kItems; i++) {
		if ((*_line)[i]->Visible())
			items++;
	}
	return items;
}

/* Return the number of lines of items. If visibleOnly is true, return the
 * number of lines with at least one visible item.
 */
unsigned WToolbar::CountLines(bool visibleOnly)
{
	if (!visibleOnly)
		return fLines.size();
	unsigned l, lines = 0;
	for (l = 0; l < fLines.size(); l++) {
		if (LineVisible(l))
			lines++;
	}
	return lines;
}

/* Remove the line, freeing all the items on it. Line index starts from 0.
 */
void WToolbar::DeleteLine(unsigned line)
{
	if (line >= fLines.size()) return;

	WToolbarLine *_line = fLines[line];
	bool disableUpdate;
	WToolbarItem *item;
	unsigned i = 0;

	// We have to buffer the number of items because the while loop below use
	// it as condition, but the line will be deleted before the last check
	const unsigned kItems = _line->size();

	// Disable updates until we finished to delete all the items on the line
	disableUpdate = fDisableUpdate;
	fDisableUpdate = true;

	// Delete all the items from the line. The line itself will be deleted by
	// RemoveItem() together with the last item
	while (i < kItems) {
		item = (*_line)[0];
		RemoveItem(item);
		delete item;
		i++;
	}

	fDisableUpdate = disableUpdate;
	Update();
}

/* Force the redraw of the item, if it belongs to this toolbar and the toolbar
 * is attached to a window.
 */
void WToolbar::DrawItem(WToolbarItem *item)
{
	AssertLocked();
	if (item == NULL) return;
	if (item->fToolbar != this) return;
	if (Window() == NULL) return;
	Invalidate(item->Frame());
}

/* Find the flexible item in the line, or NULL. If there are multiple flexible
 * items on the line, only the first is returned; there can't be more than one
 * flexible item for each line. Line index starts from 0.
 * A flexible item is an item that will be stretched to fill the line's width or
 * height (depending on the toolbar alignment), pushing the next items to the
 * right or bottom. The miminum width/height is given by the item's
 * GetPreferredSize(), there is no maximum.
 * If the toolbar has a single line and is set to auto resize, this property
 * will have no effect.
 */
WToolbarItem * WToolbar::FindFlexibleItem(unsigned line)
{
	if (line >= fLines.size()) return NULL;
	WToolbarLine *_line = fLines[line];
	const unsigned kItems = _line->size();
	unsigned i = 0;
	while (i < kItems) {
		WToolbarItem *item = (*_line)[i];
		if (item->Flexible())
			return item;
		i++;
	}
	return NULL;
}

/* Return the first item with a matching name, or NULL. Name can be NULL.
 */
WToolbarItem * WToolbar::FindItem(const char *name)
{
	WToolbarLine *line;
	unsigned l = 0, i;
	while (l < fLines.size()) {
		line = fLines[l];
		const unsigned kItems = line->size();
		i = 0;
		while (i < kItems) {
			WToolbarItem *item = (*line)[i];
			if (name == NULL) {
				if (item->Name() == NULL)
					return item;
			}
			else {
				if (strcmp(name, item->Name()) == 0)
					return item;
			}
			i++;
		}
	}
	return NULL;
}

/* Return the absolute index of the item, or -1 if it doesn't belong to this
 * toolbar.
 */
int WToolbar::IndexOf(WToolbarItem *item)
{
	if (item == NULL) return -1;
	if (item->Toolbar() != this) return -1;
	WToolbarLine *line;
	int index = 0, i;
	for (i = 0; i < item->fLine; i++)
		index += fLines[i]->size();
	i = 0;
	line = fLines[item->fLine];
	const int kItems = line->size();
	while (i < kItems) {
		if ((*line)[i] == item)
			return index + i;
	}
	return -1; // Should never happen
}

/* Return the item at the given view's coordinates, or NULL.
 */
WToolbarItem * WToolbar::ItemAt(BPoint point)
{
	if (fLines.size() == 0) return NULL;
	WToolbarLine *line;
	WToolbarItem *item;
	unsigned l = 0, i;
	while (l < fLines.size()) {
		line = fLines[l];
		const unsigned kItems = line->size();
		i = 0;
		while (i < kItems) {
			item = (*line)[i];
			if (item->fVisible && item->fFrame.Contains(point))
				return item;
			i++;
		}
		l++;
	}
	return NULL;
}

/* Return the item at the given absolute index, or NULL. Index starts from 0.
 */
WToolbarItem * WToolbar::ItemAt(int index)
{
	if (index < 0 || fLines.size() == 0) return NULL;
	unsigned base = 0, line = 0;
	while (line < fLines.size() - 1 && base < (unsigned)index) {
		base += fLines[line]->size();
		line++;
	}
	/*if (base == index) // item is the first of the line
		return (*fLines[line])[0];*/
	if (base > (unsigned)index) { // item is in previous line
		base -= fLines[line]->size();
		line--;
		//return (*fLines[line])[index - base];
	}
	// base < index (item is in the last line, or index out of range)
	return ((unsigned)index > base + fLines[line]->size() - 1 ? NULL :
		(*fLines[line])[index - base]);
}

/* Return the item at the given line and position, or NULL if line index or
 * position are invalid. If visibleOnly is true, return the item at the visible
 * line AND position (a line is visible if at least one of its items is
 * visible); this is useful for user interaction. Line index and position start
 * from 0.
 */
WToolbarItem * WToolbar::ItemAt(unsigned line, unsigned position,
	bool visibleOnly)
{
	const unsigned kLines = fLines.size();

	if (kLines == 0 || line >= kLines)
		return NULL;

	WToolbarLine *_line;
	unsigned index;
	int visible;

	// Find the line
	if (!visibleOnly)
		index = line;
	else {
		visible = -1;
		index = 0;
		while (visible < (int)line && index < kLines) {
			if (CountItemsInLine(index, true) > 0)
				visible++;
			index++;
		}
		if (visible < (int)line)
			return NULL;
		index--;
	}
	_line = fLines[index];
	const unsigned kItems = _line->size();

	// Find the requested item
	if (!visibleOnly) {
		if (position >= kItems)
			return NULL;
		index = position;
	}
	else {
		visible = -1;
		index = 0;
		while (visible < (int)position && index < kItems) {
			if ((*_line)[index]->fVisible)
				visible++;
			index++;
		}
		if (visible < (int)position)
			return NULL;
		index--;
	}
	return (*_line)[index];
}

/* Return true if the line is enabled, false otherwise. A line is enabled if at
 * least one of its controls (not items) is enabled.
 */
bool WToolbar::LineEnabled(unsigned line)
{
	if (line >= fLines.size()) return false;
	WToolbarLine *_line = fLines[line];
	const unsigned kItems = _line->size();
	bool enabled = false;
	for (unsigned i = 0; i < kItems && !enabled; i++) {
		WToolbarControl *control = dynamic_cast<WToolbarControl*>((*_line)[i]);
		if (control != NULL) {
			if (control->fEnabled)
				enabled = true;
		}
	}
	return enabled;
}

/* Return true if the line is visible, false otherwise. A line is visible if at
 * least one of its items is visible. It is a shortcut to
 * CountItems(line, true).
 */
bool WToolbar::LineVisible(unsigned line)
{
	return (CountItemsInLine(line, true) > 0);
}

/* Move the item to a new line and position. See AddItem() for possible line and
 * position values. Since the operation is accomplished by first calling
 * RemoveItem() and then AddItem(), the item will receive DetachedFromToolbar()
 * and AttachedToToolbar() events. If the line where the item is remains empty, it
 * will be deleted.
 * Return true if the operation was successful. It will return false if the
 * item is invalid, if it belongs to another toolbar, or if line/position are
 * not a valid combination (in this case, the item will be removed from the
 * toolbar).
 */
bool WToolbar::MoveItem(WToolbarItem *item, int line, int position)
{
	if (item == NULL) return false;
	if (item->Toolbar() != this) return false;
	bool disableUpdate = fDisableUpdate, ret;
	fDisableUpdate = true;
	ret = RemoveItem(item);
	if (ret)
		ret = AddItem(item, line, position);
	fDisableUpdate = disableUpdate;
	Update();
	return ret;
}

/* Return the position of the item in its line, starting from 0, or -1 if it
 * doesn't belong to this toolbar. To get the item's line index, use
 * item->Line().
 */
int WToolbar::PositionOf(WToolbarItem *item)
{
	if (item == NULL) return -1;
	if (item->Toolbar() != this) return -1;
	WToolbarLine *line = fLines[item->fLine];
	const unsigned kItems = line->size();
	unsigned i = 0;
	while (i < kItems) {
		if ((*line)[i] == item)
			return i;
		i++;
	}
	return -1;
}

/* Remove the item, if it belongs to the toolbar, and return true if the
 * operation was successful. If the item was the last in its line, the line will
 * be deleted and the items in the lines below will have their line index
 * decreased. The item will not be deleted.
 */
bool WToolbar::RemoveItem(WToolbarItem *item)
{
	if (item == NULL) return false;
	if (item->fToolbar != this) return false;

	AssertLocked();

	unsigned line, position;
	bool disableUpdate;

	// Make sure no updates are performed
	disableUpdate = fDisableUpdate;
	fDisableUpdate = true;

	// Remove the item
	if (fMouseDownItem == item)
		fMouseDownItem = NULL;
	line = (unsigned)item->fLine;
	position = (unsigned)PositionOf(item);
	item->fToolbar = NULL;
	item->fFrame = BRect(0.0, 0.0, -1.0, -1.0);
	item->fLine = -1;
	item->DetachedFromToolbar();
	fLines[line]->erase(fLines[line]->begin() + position);

	// If the line was emptied, delete it
	if (fLines[line]->size() == 0) {
		fLines.erase(fLines.begin() + line);
		for (unsigned l = line; l < fLines.size(); l++) {
			WToolbarLine *_line = fLines[l];
			const unsigned kItems = _line->size();
			for (unsigned i = 0; i < kItems; i++)
				(*_line)[i]->fLine--;
		}
	}

	fDisableUpdate = disableUpdate;
	Update();
	return true;
}

/* Enable or disable all the controls (not items) of a line.
 */
void WToolbar::SetLineEnabled(unsigned line, bool enabled)
{
	if (line >= fLines.size()) return;
	WToolbarLine *_line = fLines[line];
	const unsigned kItems = _line->size();
	for (unsigned i = 0; i < kItems; i++) {
		WToolbarControl *control = dynamic_cast<WToolbarControl*>((*_line)[i]);
		if (control != NULL)
			control->fEnabled = enabled;
	}
	Update();
	Invalidate();
}

/* Show or hide all the items of a line.
 */
void WToolbar::SetLineVisible(unsigned line, bool visible)
{
	if (line >= fLines.size()) return;
	WToolbarLine *_line = fLines[line];
	const unsigned kItems = _line->size();
	for (unsigned i = 0; i < kItems; i++)
		(*_line)[i]->fVisible = visible;
	Update();
}

// Properties ==================================================================

/* Return true if the toolbar will auto resize to fit the items.
 */
bool WToolbar::AutoSize(void)
{
	return fAutoSize;
}

/* Return true if the toolbar is enabled. Note that the items' Enabled()
 * property is not related to this; to function, both the item and the toolbar
 * must be enabled.
 */
bool WToolbar::Enabled(void)
{
	return fEnabled;
}

/* Set the auto-sizing of the toolbar. If set on, the toolbar will auto resize
 * to fit its content.
 */
void WToolbar::SetAutoSize(bool auto_size)
{
	AssertLocked();
	if (auto_size == fAutoSize) return;
	fAutoSize = auto_size;
	Update();
}

/* Enable or disable the toolbar. This does not affect the enabled property of
 * items; to function, both the item and the toolbar must be enabled.
 */
void WToolbar::SetEnabled(bool enabled)
{
	AssertLocked();
	if (enabled == fEnabled) return;
	fEnabled = enabled;
	Update();
	Invalidate();
}

// Appearance ==================================================================

/* Return the alignment of the toolbar, W_TOOLBAR_HORIZONTAL or
 * W_TOOLBAR_VERTICAL.
 */
WToolbarAlignment WToolbar::Alignment(void)
{
	return fAppearance.fAlignment;
}

/* Return the background bitmap of the toolbar, or NULL if there is no bitmap.
 */
BBitmap * WToolbar::BackgroundBitmap(void)
{
	return fAppearance.fBackBitmap;
}

/* Return the border type of the toolbar, one of B_FANCY_BORDER,
 * B_PLAIN_BORDER, or B_NO_BORDER.
 */
border_style WToolbar::Border(void)
{
	return fAppearance.fBorder;
}

/* Return the position of the labels in the toolbar buttons:
 * W_TOOLBAR_LABEL_NONE, W_TOOLBAR_LABEL_BOTTOM or B_TOOLBAR_LABEL_SIDE.
 */
WToolbarLabelPosition WToolbar::LabelPosition(void)
{
	return fAppearance.fLabelPosition;
}

/* Return the margin, in view's units, that is left around each item.
 */
float WToolbar::Margin(void)
{
	return fAppearance.fMargin;
}

/* Return the padding, the space left between the border and the items (not
 * including margin), in view's units.
 */
float WToolbar::Padding(void)
{
	return fAppearance.fPadding;
}

/* Return the buttons picture size, in view's units. If pictures are not shown,
 * returns 0.
 */
float WToolbar::PictureSize(void)
{
	return fAppearance.fPicSize;
}

/* Set the alignment of items in the toolbar, W_TOOLBAR_HORIZONTAL or
 * W_TOOLBAR_VERTICAL. Other values will be ignores. If horizontal, the items
 * will be aligned to the left, all the items in the same line will have the
 * same height, and the lines will be stacked vertically, from top to bottom; if
 * vertical, the items will be stacked to the top, all the items in the same
 * line will have the same width, and the lines will be aligned horizontally
 * from left to right. 
 */
void WToolbar::SetAlignment(WToolbarAlignment alignment)
{
	AssertLocked();
	if (alignment == fAppearance.fAlignment ||
	  (alignment != W_TOOLBAR_HORIZONTAL && alignment != W_TOOLBAR_VERTICAL))
		return;
	fAppearance.fAlignment = alignment;
	Update();
}

/* Set the background bitmap. If it is smaller than the toolbar, it will be
 * tiled. The bitmap will become property of the toolbar, you must not delete
 * it. It can be NULL to just delete the previous bitmap, if any.
 * Note: descendants of WToolbar may not use the background bitmap and draw a
 * custom background.
 */
void WToolbar::SetBackgroundBitmap(BBitmap *bitmap)
{
	AssertLocked();
	if (bitmap == fAppearance.fBackBitmap) return;
	if (fAppearance.fBackBitmap != NULL) {
		delete fAppearance.fBackBitmap;
		fAppearance.fBackBitmap = NULL;
	}
	if (bitmap != NULL)
		if (bitmap->IsValid())
			fAppearance.fBackBitmap = bitmap;
		else
			delete bitmap;
	Update();
	Invalidate();
}

/* Set the border type. It can be one of B_FANCY_BORDER (a 3D border),
 * B_PLAIN_BORDER (a thin line around the toolbar in the view's high color,
 * default black), or B_NO_BORDER. A descendant of WToolbar may draw a custom
 * fancy border.
 */
void WToolbar::SetBorder(border_style border)
{
	AssertLocked();
	if (border == fAppearance.fBorder || (border != B_FANCY_BORDER &&
	  border != B_PLAIN_BORDER && border != B_NO_BORDER))
		return;
	fAppearance.fBorder = border;
	Update();
}

/* Set the button's label position. It can be W_TOOLBAR_LABEL_NONE (only the
 * picture will be drawn), W_TOOLBAR_LABEL_BOTTOM (the label will be drawn
 * below the picture), or W_TOOLBAR_LABEL_SIDE (the label will be drawn to the
 * right of the picture). If a button doesn't have a picture, the label will be
 * drawn in the center, regardeless of this setting.
 * Items other than buttons may behave differently.
 */
void WToolbar::SetLabelPosition(WToolbarLabelPosition label_position)
{
	AssertLocked();
	if (label_position == fAppearance.fLabelPosition ||
	  (label_position != W_TOOLBAR_LABEL_BOTTOM &&
	  label_position != W_TOOLBAR_LABEL_SIDE &
	  label_position != W_TOOLBAR_LABEL_NONE))
		return;
	fAppearance.fLabelPosition = label_position;
	Update();
}

/* Set the margin around each item, in view's units. It must be positive or 0.
 * Currently, the value will be rounded down. Default value is 0.0 for all
 * styles.
 */
void WToolbar::SetMargin(float margin)
{
	AssertLocked();
	if (margin == fAppearance.fMargin || floor(margin) < 0.0) return;
	fAppearance.fMargin = floor(margin);
	Update();
}

/* Set the padding, the spaace between the border and the items (not including 
 * the margin), in view's units. It must be positive or 0. Currently, the value
 * will be rounded down. Default value is 2.0 if the style is
 * W_TOOLBAR_STYLE_FLAT or W_TOOLBAR_STYLE_3D, or 0.0 is style is
 * W_TOOLBAR_STYLE_MENU.
 */
void WToolbar::SetPadding(float padding)
{
	AssertLocked();
	if (padding == fAppearance.fPadding || floor(padding) < 0.0) return;
	fAppearance.fPadding = floor(padding);
	Update();
}

/* Set the buttons' picture size, in view's units. Set it to 0 to not show the
 * pictures (if a button does not have a label, the picture will be shown
 * regardeless of this setting). Note that this is only a suggestion; if a
 * button doesn't have a picture of this size, it will use a smaller picture or
 * no picture at all.
 * There are several pre-defined sizes:
 * W_TOOLBAR_PICTURE_NONE		0
 * W_TOOLBAR_PICTURE_XSMALL		16
 * W_TOOLBAR_PICTURE_SMALL		24
 * W_TOOLBAR_PICTURE_MEDIUM		32
 * W_TOOLBAR_PICTURE_LARGE		48
 * W_TOOLBAR_PICTURE_XLARGE		64
 * W_TOOLBAR_PICTURE_XXLARGE	96
 * W_TOOLBAR_PICTURE_XXXLARGE	128
 * W_TOOLBAR_PICTURE_HUGE		256
 * W_TOOLBAR_PICTURE_MAX		512
 * The value can't be greater than W_TOOLBAR_PICTURE_MAX. Default value is
 * W_TOOLBAR_PICTURE_XSMALL for all styles.
 * Items other than buttons may behave differently.
 */
void WToolbar::SetPictureSize(float picture_size)
{
	AssertLocked();
	if (picture_size == fAppearance.fPicSize || picture_size < 0.0 ||
	  picture_size > static_cast<float>(W_TOOLBAR_PICTURE_MAX))
	  	return;
	fAppearance.fPicSize = picture_size;
	Update();
}

/* Set one of the pre-defined styles: W_TOOLBAR_STYLE_FLAT (the default),
 * W_TOOLBAR_STYLE_3D or W_TOOLBAR_STYLE_MENU.
 * The flat style is the typical late '90s - early 2000's look: the buttons
 * do not have a visible border until the mouse pointer passes over them.
 * The 3D style is the typical early '90s look: the buttons have a 3D border
 * around them.
 * The menu style is designed to mimic the look and feel of an Haiku drop-down
 * menu.
 * Items other than buttons may use this information in other ways.
 * Setting the style may change these properties: label position, margin,
 * padding, picture size, font, high color, low color.
 * Values of style other than those described here will be registered, but the
 * toolbar and the libwalter's item classes will treat them like the default
 * value; descendant of WToolbar and item classes may use them.
 */
void WToolbar::SetStyle(int style)
{
	AssertLocked();
	fAppearance.fStyle = style;
	fDisableUpdate = true;
	switch (style) {
		case W_TOOLBAR_STYLE_FLAT:
			fAppearance.fStyle = W_TOOLBAR_STYLE_FLAT;
			if (fDisableStyling) break;
			fAppearance.fLabelPosition = W_TOOLBAR_LABEL_NONE;
			fAppearance.fMargin = 0.0;
			fAppearance.fPadding = 2.0;
			fAppearance.fPicSize =
				static_cast<float>(W_TOOLBAR_PICTURE_XSMALL);
			SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			SetFont(be_plain_font);
			SetHighColor(0, 0, 0);
			break;
		case W_TOOLBAR_STYLE_3D:
			fAppearance.fStyle = W_TOOLBAR_STYLE_3D;
			if (fDisableStyling) break;
			fAppearance.fLabelPosition = W_TOOLBAR_LABEL_NONE;
			fAppearance.fMargin = 0.0;
			fAppearance.fPadding = 2.0;
			fAppearance.fPicSize =
				static_cast<float>(W_TOOLBAR_PICTURE_XSMALL);
			SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			SetFont(be_plain_font);
			SetHighColor(0, 0, 0);
			break;
		case W_TOOLBAR_STYLE_MENU: {
			fAppearance.fStyle = W_TOOLBAR_STYLE_MENU;
			if (fDisableStyling) break;
			menu_info mi;
			BFont font;
			get_menu_info(&mi);
			fAppearance.fLabelPosition = W_TOOLBAR_LABEL_SIDE;
			fAppearance.fMargin = 0.0;
			fAppearance.fPadding = 0.0;
			fAppearance.fPicSize =
				static_cast<float>(W_TOOLBAR_PICTURE_XSMALL);
			SetLowColor(mi.background_color);
			// BeBook says that menu_info has a property called "font",
			// but that's wrong; it has "f_family" and "f_style" instead.
			font = be_plain_font;
			font.SetFamilyAndStyle(mi.f_family, mi.f_style);
			font.SetSize(mi.font_size);
			SetFont(&font);
			SetHighColor(0, 0, 0);
			} break;
		default:
			return;
	}
	fDisableUpdate = false;
	Update();
}

/* Return the style of the toolbar; it can be W_TOOLBAR_STYLE_FLAT,
 * W_TOOLBAR_STYLE_3D, W_TOOLBAR_STYLE_MENU or a custom value.
 */
int WToolbar::Style(void)
{
	return fAppearance.fStyle;
}

// Target - mimic BInvoker =====================================================

bool WToolbar::IsTargetLocal(void) const
{
	return fTarget.IsTargetLocal();
}

BMessenger WToolbar::Messenger(void) const
{
	return fTarget;
}

/* Set the target for all the controls. Target cna be set individually for each
 * control.
 */
status_t WToolbar::SetControlsTarget(BMessenger messenger)
{
	AssertLocked();
	WToolbarControl *control;
	WToolbarLine *line;
	for (unsigned l = 0; l < fLines.size(); l++) {
		line = fLines[l];
		const unsigned kItems = line->size();
		for (unsigned i = 0; i < kItems; i++) {
			control = dynamic_cast<WToolbarControl*>((*line)[i]);
			if (control != NULL)
				control->SetTarget(messenger);
		}
	}
	return B_OK;
}

status_t WToolbar::SetControlsTarget(const BHandler *handler,
	const BLooper *looper)
{
	AssertLocked();
	WToolbarControl *control;
	WToolbarLine *line;
	if (handler == NULL && looper == NULL)
		return B_BAD_VALUE;
	if (handler != NULL) {
		if (handler->Looper() == NULL)
			return B_BAD_VALUE;
		if (looper != NULL && handler->Looper() != looper)
			return B_MISMATCHED_VALUES;
	}
	for (unsigned l = 0; l < fLines.size(); l++) {
		line = fLines[l];
		const unsigned kItems = line->size();
		for (unsigned i = 0; i < kItems; i++) {
			control = dynamic_cast<WToolbarControl*>((*line)[i]);
			if (control != NULL)
				control->SetTarget(handler, looper);
		}
	}
	return B_OK;
}

/* Set the target for the toolbar. The toolbar is not a control, so it does not
 * really needs a target; it is used only when the toolbar is attached to a
 * window, when the target will be given to each of its control.
 */
status_t WToolbar::SetTarget(BMessenger messenger)
{
	AssertLocked();
	fTarget = messenger;
	return B_OK;
}

status_t WToolbar::SetTarget(const BHandler *handler, const BLooper *looper)
{
	AssertLocked();
	if (handler == NULL && looper == NULL)
		return B_BAD_VALUE;
	if (handler != NULL) {
		if (handler->Looper() == NULL)
			return B_BAD_VALUE;
		if (looper != NULL && handler->Looper() != looper)
			return B_MISMATCHED_VALUES;
	}
	fTarget = BMessenger(handler, looper);
	return B_OK;
}

BHandler * WToolbar::Target(BLooper **looper) const
{
	return fTarget.Target(looper);
}

// Other methods ===============================================================

/* Force the toolbar to recalculate the size and position of the items and to
 * auto resize itself if required. See SetAlignment(), FindFlexibleItem(),
 * and SetAutoSize() for more info.
 */
void WToolbar::Update(void)
{
	if (fDisableUpdate) return;
	AssertLocked();

	const unsigned kLines = fLines.size();

	float border, left, top, right, bottom, max, total, width, height, start;
	vector<float> maxs, totals, sizes;
	unsigned i, l, sizeIndex = 0;
	WToolbarLine *line;
	WToolbarItem *item;
	bool flexible;
	BRect r;

	// Tell the items that the toolbar properties may have changed
	for (l = 0; l < kLines; l++) {
		line = fLines[l];
		const unsigned kItems = line->size();
		for (i = 0; i < kItems; i++)
			(*line)[i]->Update();
	}

	// For each line, search for the tallest (or widest) item, calculate total
	// width (or height), and cache items size
	for (l = 0; l < kLines; l++) {
		line = fLines[l];
		const unsigned kItems = line->size();
		max = 0.0;
		total = 0.0;
		for (i = 0; i < kItems; i++) {
			item = (*line)[i];
			if (item->fVisible) {
				item->GetPreferredSize(&width, &height);
				width++;
				height++;
				if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL) {
					if (width > max) max = width;
					total += height + fAppearance.fMargin;
				}
				else {
					if (height > max) max = height;
					total += width + fAppearance.fMargin;
				}
			}
			sizes.push_back(width);
			sizes.push_back(height);
		}
		maxs.push_back(max);
		totals.push_back(total + fAppearance.fMargin);
	}

	// Get the border width
	switch (fAppearance.fBorder) {
		case B_FANCY_BORDER:
			border = ceil(BorderThickness());
			break;
		case B_PLAIN_BORDER:
			border = 1.0;
			break;
		default:
			border = 0.0;
			break;
	}

	// Find the widest (or tallest) line. Used to calculate the size of the
	// flexible items.
	max = (fAutoSize ? 0.0 : (fAppearance.fAlignment == W_TOOLBAR_VERTICAL ?
		Frame().Height() : Frame().Width()) + 1.0 - (border +
		fAppearance.fPadding) * 2.0);
	for (l = 0; l < kLines; l++) {
		if (totals[l] > max)
			max = totals[l];
	}

	// Left (for horizontal) or top (vertical) will not change
	start = border + fAppearance.fPadding + fAppearance.fMargin;
	left = start;
	top = start;

	// Assign each item its frame
	for (l = 0; l < kLines; l++) {
		line = fLines[l];
		const unsigned kItems = line->size();
		flexible = false;
		if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL) {
			top = start;
			right = left + maxs[l] - 1.0;
		}
		else {
			left = start;
			bottom = top + maxs[l] - 1.0;
		}
		for (i = 0; i < kItems; i++) {
			item = (*line)[i];
			if (item->fVisible) {
				r.left = left;
				r.top = top;
				width = sizes[sizeIndex * 2];
				height = sizes[sizeIndex * 2 + 1];
				sizeIndex++;
				if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL) {
					r.right = right;
					r.bottom = top + height - 1.0;
					if (item->fFlexible && !flexible) {
						r.bottom += max - totals[l];
						flexible = true;
					}
					top = r.bottom + fAppearance.fMargin + 1.0;
				}
				else {
					r.right = left + width - 1.0;
					r.bottom = bottom;
					if (item->fFlexible && !flexible) {
						r.right += max - totals[l];
						flexible = true;
					}
					left = r.right + fAppearance.fMargin + 1.0;
				}
				item->fFrame = r;
			}
		}
		if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL)
			left = right + fAppearance.fMargin + 1.0;
		else
			top = bottom + fAppearance.fMargin + 1.0;
	}

	// Auto-resize the toolbar
	if (fAutoSize) {
		// TODO resize accordingly to BView's resizing mode
		float w, h;
		GetPreferredSize(&w, &h);
		if (w != Frame().Width() || h != Frame().Height())
			ResizeToPreferred();
		else {
			if (Window() != NULL)
				Invalidate();
		}
	}
	else {
		if (Window() != NULL)
			Invalidate();
	}

	// If the rects have changed the mouse may be over a different
	// item than before, so we have to simulate a MouseOver() event
	if (Window() != NULL) {
		WToolbarItem *new_over;
		uint32 buttons;
		BPoint point;
		GetMouse(&point, &buttons, false);
		new_over = ItemAt(point);
		if (fMouseDownItem == NULL) {
			if (new_over != fMouseOverItem) {
				if (fMouseOverItem != NULL) {
					fMouseOverItem->MouseMoved(
						point - fMouseOverItem->fFrame.LeftTop(),
						B_EXITED_VIEW, NULL);
				}
				if (new_over != NULL) {
					new_over->MouseMoved(point - new_over->fFrame.LeftTop(),
						B_ENTERED_VIEW, NULL);
				}
				fMouseOverItem = new_over;
			}
			else {
				if (fMouseOverItem != NULL) {
					fMouseOverItem->MouseMoved(
						point - fMouseOverItem->fFrame.LeftTop(),
						B_INSIDE_VIEW, NULL);
				}
			}
		}
		else {
			if (new_over != fMouseDownItem)
				fMouseDownItem->MouseMoved(
					point - fMouseDownItem->fFrame.LeftTop(), B_EXITED_VIEW,
					NULL);
			else
				fMouseDownItem->MouseMoved(
					point - fMouseDownItem->fFrame.LeftTop(), B_INSIDE_VIEW,
					NULL);
		}
	}
}
