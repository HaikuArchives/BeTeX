// ============================================================================
//
// libwalter BitmapUtils.h
//
// Distributed under the terms of the MIT License.
//
// Authors:
//		Gabriele Biffi - http://www.biffuz.it/
//
// ============================================================================

#ifndef __BITMAPUTILS_H__
#define __BITMAPUTILS_H__

// Haiku headers
#include <SupportDefs.h>

// Haiku classes
class BBitmap;

class BitmapUtils {
public:

	static	BBitmap *	Grayscale(BBitmap *bitmap);

	static	BBitmap *	LoadFromResource(const char *name);
	static	BBitmap *	LoadFromResource(int32 id);
};

#endif // __BITMAPUTILS_H__
