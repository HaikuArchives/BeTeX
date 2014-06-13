// =============================================================================
//
// libwalter WToolbarSupport.cpp
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

#ifndef _SUPPORT_H_
#define _SUPPORT_H_

// Haiku headers
#include <Rect.h>

// Haiku classes
class BView;

// =============================================================================
// WToolbarSupport
// =============================================================================

class WToolbarSupport {
public:
	static	void			Draw3DBorder(BView *view, BRect rect,
								bool inset = false);
};

#endif // _SUPPORT_H_
