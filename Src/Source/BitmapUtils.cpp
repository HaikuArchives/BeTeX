// ============================================================================
//
// libwalter BitmapUtils.cpp
//
// Distributed under the terms of the MIT License.
//
// Authors:
//		Gabriele Biffi - http://www.biffuz.it/
//
// ============================================================================

// Standard C headers
#include <stdio.h>

// Haiku headers
#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <Resources.h>
#include <Roster.h>
#include <Screen.h>

// libwalter headers
#include "BitmapUtils.h"

// Local functions

inline uint8 __Yc(uint8 red, uint8 green, uint8 blue)
{
	float R, G, B, Y;
	R = (float)(red) / 255.0;
	G = (float)(green) / 255.0;
	B = (float)(blue) / 255.0;
	Y = (R - G) * 0.299 + G + (B - G) * 0.114;
	return (uint8)(Y * 255.0);
}

/*!
 * \brief Turn a bitmap in grayscale
 *
 * The output bitmap will be of the same size and will use the same color space
 * of the original. Currently supports the following color spaces: B_CMAP8,
 * B_RGB24, B_RGB24_BIG, B_RGB32, B_BRGBA32, B_RGB32_BIG, B_RGBA32_BIG;
 * transparency is maintained. With all other color spaces, the function will
 * return a copy of the bitmap (note that B_GRAY8 and B_GRAY1 are already
 * grayscale).
 *
 * \todo Support more color spaces
 *
 * @param[in] bitmap The bitmap you want to turn in grayscale
 *
 * \return If bitmap is  \c NULL or isn't valid, returns  \c NULL, or a new
 *         bitmap. It's up to you to delete the bitmap when you are done with
 *         it.
 */
BBitmap * BitmapUtils::Grayscale(BBitmap *bitmap)
{
	if (bitmap == NULL) return NULL;
	if (!bitmap->IsValid()) return NULL;

	// To obtain the disabled bitmap, we turn the original bitmap in
	// grayscale, converting each pixel from RGB to YCbCr and taking only
	// the Y component (luminance).

	int32 x, y, width, height, offset, row_length;
	unsigned char *i_bits, *o_bits;
	BBitmap *ret;
	uint8 Yc;

	ret = new BBitmap(bitmap->Bounds(), bitmap->ColorSpace());
	i_bits = (unsigned char*)bitmap->Bits();
	o_bits = (unsigned char*)ret->Bits();
	height = bitmap->Bounds().IntegerHeight() + 1;
	width = bitmap->Bounds().IntegerWidth() + 1;
	row_length = bitmap->BytesPerRow();

	switch (bitmap->ColorSpace()) {

		// B_CMAP8: each pixel (one byte) is the index of a color in the
		// system's color table. The table can be obtained from a BScreen
		// object.
		case B_CMAP8: {
			const rgb_color *color;
			const color_map *map;
			BScreen screen;
			map = screen.ColorMap();
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					offset = y * row_length + x;
					if (i_bits[offset] == B_TRANSPARENT_8_BIT)
						o_bits[offset] = B_TRANSPARENT_8_BIT;
					else {
						color = map->color_list + i_bits[offset];
						Yc = __Yc(color->red, color->green, color->blue);
						o_bits[offset] = screen.IndexForColor(Yc, Yc, Yc);
					}
				}
			}
			} break;

		// B_RGB24 = BGR 8:8:8
		case B_RGB24:
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					offset = y * row_length + x * 3;
					Yc = __Yc(i_bits[offset + 2], i_bits[offset + 1],
						i_bits[offset]);
					o_bits[offset + 1] = Yc;
					o_bits[offset + 2] = Yc;
					o_bits[offset + 3] = Yc;
				}
			}
			break;

		// B_RGB32  = BGRA 8:8:8:8
		// B_RGBA32 = BGRx 8:8:8:8
		case B_RGB32:
		case B_RGBA32:
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					offset = y * row_length + x * 4;
					Yc = __Yc(i_bits[offset + 2], i_bits[offset + 1],
						i_bits[offset]);
					o_bits[offset] = Yc;
					o_bits[offset + 1] = Yc;
					o_bits[offset + 2] = Yc;
					o_bits[offset + 3] = i_bits[offset];
				}
			}
			break;

		// B_RGB24_BIG = RGB 8:8:8
		case B_RGB24_BIG:
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					offset = y * row_length + x * 3;
					Yc = __Yc(i_bits[offset], i_bits[offset + 1],
						i_bits[offset + 2]);
					o_bits[offset + 1] = Yc;
					o_bits[offset + 2] = Yc;
					o_bits[offset + 3] = Yc;
				}
			}
			break;

		// B_RGB32_BIG  = xRGB 8:8:8:8
		// B_RGBA32_BIG = ARGB 8:8:8:8
		case B_RGB32_BIG:
		case B_RGBA32_BIG:
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					offset = y * row_length + x * 4;
					Yc = __Yc(i_bits[offset + 1], i_bits[offset + 2],
						i_bits[offset + 3]);
					o_bits[offset] = i_bits[offset];
					o_bits[offset + 1] = Yc;
					o_bits[offset + 2] = Yc;
					o_bits[offset + 3] = Yc;
				}
			}
			break;

		default:
			memcpy(ret->Bits(), bitmap->Bits(), bitmap->BitsLength());
			break;
	}

	return ret;
}

BBitmap * BitmapUtils::LoadFromResource(const char *name)
{
	if (name == NULL) return NULL;
	if (name[0] == 0) return NULL;

	BArchivable *archivable;
	BResources resources;
	BMessage message;
	const void *data;
	BBitmap *bitmap;
	app_info info;
	BFile file;
	size_t len;

	if (be_app->GetAppInfo(&info) != B_OK) return NULL;
	if (file.SetTo(&(info.ref), B_READ_ONLY) != B_OK) return NULL;
	if (resources.SetTo(&file, false) != B_OK) return NULL;
	data = resources.LoadResource('BBMP', name, &len);
	if (data == NULL || len <= 0) return NULL;
	if (message.Unflatten((const char *)data) != B_OK) return NULL;
	archivable = BBitmap::Instantiate(&message);
	if (archivable == NULL) return NULL;
	bitmap = dynamic_cast<BBitmap*>(archivable);
	if (bitmap == NULL) {
		delete archivable;
		return NULL;
	}
	return bitmap;
}

BBitmap * BitmapUtils::LoadFromResource(int32 id)
{
	BArchivable *archivable;
	BResources resources;
	BMessage message;
	const void *data;
	BBitmap *bitmap;
	app_info info;
	BFile file;
	size_t len;

	if (be_app->GetAppInfo(&info) != B_OK) return NULL;
	if (file.SetTo(&(info.ref), B_READ_ONLY) != B_OK) return NULL;
	if (resources.SetTo(&file, false) != B_OK) return NULL;
	data = resources.LoadResource('BBMP', id, &len);
	if (data == NULL || len <= 0) return NULL;
	if (message.Unflatten((const char *)data) != B_OK) return NULL;
	archivable = BBitmap::Instantiate(&message);
	if (archivable == NULL) return NULL;
	bitmap = dynamic_cast<BBitmap*>(archivable);
	if (bitmap == NULL) {
		delete archivable;
		return NULL;
	}
	return bitmap;
}
