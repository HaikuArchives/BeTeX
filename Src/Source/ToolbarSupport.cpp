// =============================================================================
//
// libwalter ToolbarSupport.cpp
//
// Miscellaneous functions for WToolbar and related classes
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

/*!
 * \class WToolbarSupport
 * \brief Miscellaneous support methods
 */

// Haiku headers
#include <View.h>

// libwalter headers
#include "ToolbarSupport.h"

// =============================================================================
// WToolbarSupport
// =============================================================================

/*!
 * \brief Draw a 3D border
 *
 * The border is drawn using the view's low color as a base.
 *
 * @param[in] view The target view
 * @param[in] rect The rectangle wich will contain the border
 * @param[in] inset Leave \c false if you want a raised border, \c true for an
 *                  inset border
 */
void WToolbarSupport::Draw3DBorder(BView *view, BRect rect, bool inset)
{
	if (view == NULL || !(rect.IsValid())) return;

	rgb_color lighter, light, dark, shadow;
	float l, t, r, b;

	view->PushState();

	lighter = tint_color(view->LowColor(), B_DARKEN_2_TINT);
	light = tint_color(view->LowColor(), B_LIGHTEN_MAX_TINT);
	dark = tint_color(view->LowColor(), B_DARKEN_1_TINT);
	shadow = tint_color(view->LowColor(), B_DARKEN_3_TINT);

	l = rect.left;
	t = rect.top;
	r = rect.right;
	b = rect.bottom;

	view->BeginLineArray(8);
	if (inset) {
		view->AddLine(BPoint(r, t + 1), BPoint(r, b), lighter);
		view->AddLine(BPoint(l + 1, b), BPoint(r, b), lighter);
		view->AddLine(BPoint(r - 1, t + 2), BPoint(r - 1, b - 1), light);
		view->AddLine(BPoint(l + 2, b - 1), BPoint(r - 1, b - 1), light);
		view->AddLine(BPoint(l, t), BPoint(l, b), shadow);
		view->AddLine(BPoint(l, t), BPoint(r, t), shadow);
		view->AddLine(BPoint(l + 1, t + 1), BPoint(l + 1, b - 1), dark);
		view->AddLine(BPoint(l + 1, t + 1), BPoint(r - 1, t + 1), dark);
	}
	else {
		view->AddLine(BPoint(l, t), BPoint(r - 1, t), lighter);
		view->AddLine(BPoint(l, t), BPoint(l, b - 1), lighter);
		view->AddLine(BPoint(l + 1, t + 1), BPoint(r - 2, t + 1), light);
		view->AddLine(BPoint(l + 1, t + 1), BPoint(l + 1, b - 2), light);
		view->AddLine(BPoint(l, b), BPoint(r, b), shadow);
		view->AddLine(BPoint(r, t), BPoint(r, b), shadow);
		view->AddLine(BPoint(l + 1, b - 1), BPoint(r - 1, b - 1), dark);
		view->AddLine(BPoint(r - 1, t + 1), BPoint(r - 1, b - 1), dark);
	}
	view->EndLineArray();

	view->PopState();
}
