// =============================================================================
//
// libwalter ToolbarButton.cpp
//
// Button control for WToolbar
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

/* WToolbarButton is the classic toolbar button. It can have a picture or a
 * label, or both. Currently it supports only bitmaps; SVG will be added later.
 * It can have different pictures for different button size (e.g., "large" or
 * "small") and for different states (enabled, disabled, mouse over, pushed,
 * etc.). See the ButtonDecorator class for more info.
 */

// Standard C headers
#include <math.h>

// Haiku headers
#include <Bitmap.h>

// libwalter headers
#include "BitmapUtils.h"
#include "ButtonDecorator.h"
#include "Toolbar.h"
#include "ToolbarButton.h"
#include "ToolbarSupport.h"

// =============================================================================
// WToolbarButton
// =============================================================================

// Constructors and destructors

/* Creates a new button. name is the name of the item (will not be visible to
 * the user), label is the label that is visible to the user if the toolbar is
 * set to show item labels, picture is the button's picture (currently only
 * bitmaps are supported), message is the message that is sent to the target
 * when the button is clicked, switchMode if true will make the button remain
 * pressed (the state can be checked with Value()). Any of these values can be
 * NULL, if not required.
 * The constructor will generate a grayscale version of the supplied picture to
 * use as a disabled picture. The picture will become property of the button's
 * decorator; do not delete it.
 */
WToolbarButton::WToolbarButton(const char *name, const char *label,
	BBitmap *picture, BMessage *message, bool switchMode)
	: WToolbarControl(name, message)
{
	_InitObject();

	SetLabel(label);
	SetSwitchMode(switchMode);

	if (picture != NULL) {
		BBitmap *bitmaps[8];
		bitmaps[0] = picture;
		bitmaps[1] = BitmapUtils::Grayscale(picture);
		for (int i = 2; i < 8; i++)
			bitmaps[i] = NULL;
		fDecorator->SetPicture(bitmaps);
	}
}

WToolbarButton::WToolbarButton(BMessage *archive)
	: WToolbarControl(archive)
{
	BMessage decArchive;
	bool switchMode;
	int value;

	_InitObject();

	if (archive->FindMessage("WToolbarButton::decorator",
	  &decArchive) == B_OK) {
		BArchivable *archivable;
		archivable = ButtonDecorator::Instantiate(&decArchive);
		if (archivable != NULL) {
			ButtonDecorator *decorator =
				static_cast<ButtonDecorator*>(archivable);
			if (decorator != NULL) {
				delete fDecorator;
				fDecorator = decorator;
			}
			else
				delete archivable;
		}
	}

	if (archive->FindBool("WToolbarButton::switch_mode", &switchMode) == B_OK)
		SetSwitchMode(switchMode);

	if (archive->FindInt32("WToolbarButton::value", (int32*)(&value)) == B_OK)
		SetValue(value);
}	

WToolbarButton::~WToolbarButton()
{
	delete fDecorator;
}

// Private

void
WToolbarButton::_InitObject(void)
{
	fDecorator = new ButtonDecorator(NULL, NULL, BD_POSITION_ABOVE);
	fCanvas = NULL;
	fMouseDown = false;
	fMouseOver = false;
	fStyle = W_TOOLBAR_STYLE_FLAT;
	fSwitchMode = false;
	fValue = B_CONTROL_OFF;
}

// Drawing hooks

/* Returns the thickness of the border, in toolbar's units. The default
 * implementation returns 0 if the toolbar's style is W_TOOLBAR_STYLE_MENU, 2
 * otherwise. You can overload this if your custom style's border has a differnt
 * thickness, but currently the returned value will be rounded down.
 */
float
WToolbarButton::BorderThickness(void)
{
	return (fStyle == W_TOOLBAR_STYLE_MENU ? 0.0 : 2.0);
}

/* Returns the size of the button content, including the extra space for the
 * pushed effect. You can overload this, but currently the returned values will
 * be rounded down.
 */
void
WToolbarButton::ContentSize(float *width, float *height)
{
	float w, h;
	fDecorator->GetPreferredSize(fCanvas, &w, &h);
	if (fStyle != W_TOOLBAR_STYLE_MENU) {
		// One extra pixel for pushed effect
		w += 1.0;
		h += 1.0;
	}
	if (width != NULL)
		*width = w;
	if (height != NULL)
		*height = h;
}

/* Draws the background of the button. You can overload this if you want your
 * buttons to have a custom background.
 */
void
WToolbarButton::DrawBackground(BRect updateRect)
{
	// Draw the menu selection background
	if (Enabled() && Toolbar()->Enabled() && fMouseOver &&
	 	fStyle == W_TOOLBAR_STYLE_MENU) {
		fCanvas->SetLowColor(ui_color(B_MENU_SELECTION_BACKGROUND_COLOR));
		fCanvas->FillRect(Bounds(), B_SOLID_LOW);
	}

	// Draw the background when value is on
	if (fStyle != W_TOOLBAR_STYLE_MENU && fValue == B_CONTROL_ON) {
		fCanvas->SetHighColor(255, 255, 255, 128);
		fCanvas->SetDrawingMode(B_OP_ALPHA);
		fCanvas->FillRect(Bounds(), B_SOLID_HIGH);
	}
}

/* Draws the button's border.
 * If you want to draw a custom border, remember to overload BorderThickness()
 * as well.
 */
void
WToolbarButton::DrawBorder(BRect updateRect)
{
	bool doDraw = false;
	bool pushed = false;

	// Check if the border has to be drawn, and how

	if (fStyle == W_TOOLBAR_STYLE_FLAT) {
		// Flat toolbar: draw only if the mouse is over us or value is on
		if ((fMouseOver && Enabled() && Toolbar()->Enabled()) ||
		 	fValue == B_CONTROL_ON) {
			doDraw = true;
			pushed = (fMouseDown || fValue == B_CONTROL_ON);
		}
	} else if (fStyle == W_TOOLBAR_STYLE_MENU) {
		// Menu: no border needed
	} else {
		// 3D or unknown toolbar: draw it
		doDraw = true;
		pushed = (fMouseDown || fValue == B_CONTROL_ON);
	}

	// Draw the border

	if (doDraw)
		WToolbarSupport::Draw3DBorder(fCanvas, Bounds(), pushed);
}

/* Draws the button's content at position.
 * If you want to draw a custom content, remember to overload ContentSize() as
 * well.
 */
void
WToolbarButton::DrawContent(BPoint position, BRect updateRect)
{
	int status = BD_STATUS_NORMAL;
	float w, h;
	BRect r;

	// Status
	if (fMouseDown || fValue == B_CONTROL_ON)
		status = BD_STATUS_PUSHED;
	if (fMouseOver)
		status |= BD_STATUS_OVER;
	if (!Enabled() || !Toolbar()->Enabled())
		status |= BD_STATUS_DISABLED;

	// Calculate the contect rectangle
	fDecorator->GetPreferredSize(fCanvas, &w, &h);
	r =  BRect(position, BPoint(position.x + w, position.y + h));
	if (fMouseDown || fValue == B_CONTROL_ON)
		r.OffsetBy(1.0, 1.0);

	// Draw the content
	fDecorator->Draw(fCanvas, r, status);
}

/* Returns the padding of the content (the empty space between the button's
 * border and the content). You can overload this to give a custom padding, but
 * currently the value will be rounded down.
 */
float
WToolbarButton::Padding(void)
{
	return 2.0;
}

// BArchivable hooks

/* Archive the button into the given archive. Adds the following fields to the
 * archive if the respective properties are different from the default:
 *  WToolbarButton::decorator	message
 *  WToolbarButton::switch_mode	bool
 *  WToolbarButton::value		int32
 * The decorator field is the archive of the decorator. The default decorator
 * archives all the pictures and the label.
 */
status_t
WToolbarButton::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	status = WToolbarControl::Archive(archive, deep);

	if (status == B_OK) {
		BMessage decorator;
		status = fDecorator->Archive(&decorator);
		if (status == B_OK)
			status = archive->AddMessage("WToolbarButton::decorator",
				&decorator);
	}

	if (status == B_OK && fSwitchMode == false)
		status = archive->AddBool("WToolbarButton::switch_mode", fSwitchMode);

	if (status == B_OK && fValue != B_CONTROL_OFF)
		status = archive->AddInt32("WToolbarButton::value", (int32)fValue);

	return status;
}

BArchivable *
WToolbarButton::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "WToolbarButton") ?
		new WToolbarButton(archive) : NULL);
}

// WToolbarItem hooks

void
WToolbarButton::AttachedToToolbar(void)
{
	WToolbarItem::AttachedToToolbar();
	fMouseDown = false;
	fMouseOver = false;
}

void
WToolbarButton::DetachedFromToolbar(void)
{
	WToolbarItem::DetachedFromToolbar();
}

/* Draws the button on the given canvas (the toolbar itself, or an off-screen
 * buffer).
 */
void
WToolbarButton::Draw(BView *canvas, BRect updateRect)
{
	if (Toolbar() == NULL || canvas == NULL)
		return;

	float w, h, ch, cw, border, padding;
	BPoint cp;

	// Prepare infos for protected drawing hooks
	fCanvas = canvas;
	fStyle = Toolbar()->Style();

	// Border and padding. We do it after setting infos because BorderSize()
	// and Padding() may need them
	border = floor(BorderThickness());
	padding = floor(Padding());

	// Draw the background
	fCanvas->PushState();
	DrawBackground(updateRect);
	fCanvas->PopState();

	// Draw the border
	fCanvas->PushState();
	DrawBorder(updateRect);
	fCanvas->PopState();

	// Calculate position of the content
	w = Bounds().Width();
	h = Bounds().Height();
	ContentSize(&cw, &ch);
	cw = floor(cw);
	ch = floor(ch);
	cp.x = floor((w - cw) / 2.0);
	cp.y = floor((h - ch) / 2.0);

	// Draw the content
	// TODO check UpdateRect and set clipping region
	canvas->PushState();
	DrawContent(cp, updateRect);
	canvas->PopState();
}

/* Returns the preferred size of the button, including the border, the padding,
 * and the content. The button must be attached to a toolbar, or both values
 * will be -1. If you aren't interested in one of the values, you can pass NULL.
 */
void
WToolbarButton::GetPreferredSize(float *width, float *height)
{
	// We must be attached to a toolbar
	if (Toolbar() == NULL) {
		if (width != NULL) *width = -1.0;
		if (height != NULL) *height = -1.0;
		return;
	}

	float border, padding, w, h;

	// Set infos for protected drawing hooks
	fCanvas = Toolbar();
	fStyle = Toolbar()->Style();

	// Calculate the size
	border = floor(BorderThickness());
	padding = floor(Padding());
	ContentSize(&w, &h);
	w = floor(w) + border * 2.0 + padding * 2.0;
	h = floor(h) + border * 2.0 + padding * 2.0;

	if (width != NULL) *width = w - 1.0;
	if (height != NULL) *height = h - 1.0;
}

void
WToolbarButton::MouseDown(BPoint point)
{
	if (Toolbar() == NULL)
		return;
	if (fMouseOver) {
		fMouseDown = true;
		Invalidate();
	}
}

void
WToolbarButton::MouseMoved(BPoint point, uint32 transit,
	const BMessage *message)
{
	if (Toolbar() == NULL) return;
	bool oldMouseOver = fMouseOver;
	fMouseOver = (transit == B_ENTERED_VIEW || transit == B_INSIDE_VIEW);
	if (fMouseOver != oldMouseOver)
		Invalidate();
}

void
WToolbarButton::MouseUp(BPoint point)
{
	fMouseDown = false;
	if (Toolbar() != NULL && fMouseOver && Enabled()) {
		if (fSwitchMode)
			fValue = (fValue == B_CONTROL_ON ? B_CONTROL_OFF : B_CONTROL_ON);
		Invalidate();
		Invoke();
	}
}

void
WToolbarButton::Update(void)
{
	if (Toolbar() == NULL) return;

	float picsize = Toolbar()->PictureSize();

	// Label position
	switch (Toolbar()->LabelPosition()) {
		case W_TOOLBAR_LABEL_NONE:
			fDecorator->SetVisible(BD_VISIBLE_PICTURE);
			break;
		case W_TOOLBAR_LABEL_BOTTOM:
			fDecorator->SetPosition(BD_POSITION_ABOVE);
			fDecorator->SetVisible(picsize == 0.0 ?
				BD_VISIBLE_LABEL : BD_VISIBLE_BOTH);
			break;
		case W_TOOLBAR_LABEL_SIDE:
			fDecorator->SetPosition(BD_POSITION_LEFT);
			fDecorator->SetVisible(picsize == 0.0 ?
				BD_VISIBLE_LABEL : BD_VISIBLE_BOTH);
			break;
	}

	// Picture size
	if (picsize > 0.0)
		fDecorator->SetPictureSize(picsize);
}

// Other methods

unsigned
WToolbarButton::CountBitmapSets(void)
{
	return fDecorator->CountBitmapSets();
}

bool
WToolbarButton::DeleteBitmapSet(unsigned size)
{
	if (size == 0)
		return false;
	return fDecorator->DeleteBitmapSet(size);
}

bool
WToolbarButton::DeleteBitmapSetAt(unsigned index)
{
	return fDecorator->DeleteBitmapSetAt(index);
}

void WToolbarButton::DeletePicture(void)
{
	fDecorator->DeletePicture();
}

BDBitmapSet *
WToolbarButton::GetBitmapSet(unsigned size)
{
	if (size == 0)
		return NULL;
	return fDecorator->GetBitmapSet(size);
}

BDBitmapSet *
WToolbarButton::GetBitmapSetAt(unsigned index)
{
	return fDecorator->GetBitmapSetAt(index);
}

BBitmap * WToolbarButton::GetMenuCheckMark(void)
{
	static const unsigned char kCheckMarkBitmapData[] = {
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x17, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x17, 0x17, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x17, 0xFF, 0xFF,
		0xFF, 0x0C, 0x0C, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x17, 0x17, 0xFF, 0xFF,
		0xFF, 0x0C, 0x0C, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x17, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0x0C, 0x0C, 0xFF, 0x0C, 0x0C, 0x17, 0x17, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0x0C, 0x0C, 0xFF, 0x0C, 0x0C, 0x17, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x0C, 0x17, 0x17, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x0C, 0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0x17, 0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};
	BBitmap *ret = new BBitmap(BRect(0.0, 0.0, 11.0, 11.0), B_CMAP8);
	memcpy(ret->Bits(), kCheckMarkBitmapData, 144);
	return ret;
}

void
WToolbarButton::GetPicture(BBitmap *picture[8])
{
	if (picture == NULL)
		return;
	fDecorator->GetPicture(picture);
}

int
WToolbarButton::IndexOfBitmapSet(BDBitmapSet *set)
{
	if (set == NULL)
		return -1;
	return fDecorator->IndexOfBitmapSet(set);
}

const char *
WToolbarButton::Label(void)
{
	return fDecorator->Label();
}

void
WToolbarButton::SetLabel(const char *label)
{
	fDecorator->SetLabel(label);
}

bool
WToolbarButton::SetPicture(BBitmap *picture[8])
{
	if (picture == NULL)
		return false;
	if (picture[0] == NULL)
		return false;
	return fDecorator->SetPicture(picture);
}

void
WToolbarButton::SetSwitchMode(bool switchMode)
{
	if (fSwitchMode == switchMode) return;
	fSwitchMode = switchMode;
	Invalidate();
}

void
WToolbarButton::SetValue(int32 value)
{
	if ((value != B_CONTROL_ON && value != B_CONTROL_OFF) || (fValue == value))
		return;
	fValue = value;
	Invalidate();
}

bool
WToolbarButton::SwitchMode(void)
{
	return fSwitchMode;
}

int32
WToolbarButton::Value(void)
{
	return fValue;
}
