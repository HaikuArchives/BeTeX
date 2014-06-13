// =============================================================================
//
// libwalter ToolbarSeparator.cpp
//
// Separator item for WToolbar
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

// libwalter headers
#include "Toolbar.h"
#include "ToolbarSeparator.h"

// =============================================================================
// WToolbarSeparator
// =============================================================================

/*!
 * \class WToolbarSeparator
 * \brief Separator control for toolbars.
 *
 * The separator looks like a 3D inset line if the toolbar's style is
 * W_TOOLBAR_STYLE_FLAT or W_TOOLBAR_STYLE_MENU, or a simple empty space if
 * style is W_TOOLBAR_STYLE_3D. It is vertical if the toolbar is horizontal,
 * or horizontal if the toolbar is vertical. The empty space size is the double
 * of toolbar's padding.
 */

// Constructors and destructors

/*!
 * \brief Creates a new toolbar separator.
 *
 * @param[in] name Name of the separator, passed straight to the WToolbarControl
 *                 constructor.
 */
WToolbarSeparator::WToolbarSeparator(const char *name) :
	WToolbarItem(name)
{
}

WToolbarSeparator::WToolbarSeparator(BMessage *archive) :
	WToolbarItem(archive)
{
	// Nothing to do here!
}

WToolbarSeparator::~WToolbarSeparator()
{
}

// BArchivable hooks

status_t WToolbarSeparator::Archive(BMessage *archive, bool deep) const
{
	// We don't have nothing to archive
	return WToolbarItem::Archive(archive, deep);
}

BArchivable * WToolbarSeparator::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "WToolbarSeparator") ?
		new WToolbarSeparator(archive) : NULL);
}

// WToolbarControl hooks

void WToolbarSeparator::Draw(BView *canvas, BRect updateRect)
{
	if (Toolbar() == NULL) return;
	switch (Toolbar()->Style()) {
		case W_TOOLBAR_STYLE_3D:
			break;
		case W_TOOLBAR_STYLE_FLAT:
		case W_TOOLBAR_STYLE_MENU:
		default: {
			float l, r, t, b, p;
			p = ((Toolbar()->Alignment() == W_TOOLBAR_VERTICAL ?
				Bounds().Height() : Bounds().Width()) + 1) / 2.0 - 1.0;
			l = Bounds().left;
			r = Bounds().right;
			t = Bounds().top;
			b = Bounds().bottom;
			canvas->BeginLineArray(2);
			if (Toolbar()->Alignment() == W_TOOLBAR_VERTICAL) {
				canvas->AddLine(BPoint(l, t + p), BPoint(r, t + p),
					tint_color(canvas->LowColor(), B_DARKEN_2_TINT));
				canvas->AddLine(BPoint(l, t + p + 1.0), BPoint(r, t + p + 1.0),
					tint_color(canvas->LowColor(), B_LIGHTEN_MAX_TINT));
			}
			else {
				canvas->AddLine(BPoint(l + p, t), BPoint(l + p, b),
					tint_color(canvas->LowColor(), B_DARKEN_2_TINT));
				canvas->AddLine(BPoint(l + p + 1.0, t), BPoint(l + p + 1.0, b),
					tint_color(canvas->LowColor(), B_LIGHTEN_MAX_TINT));
			}
			canvas->EndLineArray();
			} break;
	}
}

void WToolbarSeparator::GetPreferredSize(float *width, float *height)
{
	float w, h, s = 0, o = 0;
	if (Toolbar() != NULL) {
		switch (Toolbar()->Style()) {
			case W_TOOLBAR_STYLE_3D:
				s = Toolbar()->Padding() * 2.0;
				break;
			case W_TOOLBAR_STYLE_MENU: {
				font_height fe;
				if (Toolbar()->LockLooper()) {
					Toolbar()->GetFontHeight(&fe);
					Toolbar()->UnlockLooper();
				}
				s = fe.ascent + fe.descent + fe.leading;
				o = 2.0;
				} break;
			case W_TOOLBAR_STYLE_FLAT:
			default:
				s = Toolbar()->Padding() * 2.0 + 2.0;
				o = 2.0;
				break;
		}
		if (Toolbar()->Alignment() == W_TOOLBAR_VERTICAL) {
			w = o;
			h = s;
		}
		else {
			w = s;
			h = o;
		}
	}
	if (width != NULL) *width = w;
	if (height != NULL) *height = h;
}
