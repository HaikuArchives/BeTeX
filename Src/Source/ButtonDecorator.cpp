// ============================================================================
//
// libwalter ButtonDecorator.cpp
//
// Distributed under the terms of the MIT License.
//
// Authors:
//		Gabriele Biffi - http://www.biffuz.it/
//
// ============================================================================

// Standard C headers
#include <math.h>
#include <stdio.h>

// Haiku headers
#include <Bitmap.h>
#include <Font.h>
#include <Message.h>
#include <Region.h>
#include <View.h>

// libwalter headers
#include "BitmapUtils.h"
#include "ButtonDecorator.h"

// Currently supported color spaces
#define COLORSPACES_COUNT 9
const color_space kColorSpaces[COLORSPACES_COUNT] = {
	B_CMAP8,
	B_RGB24,
	B_RGB32,
	B_RGBA32,
	B_RGB24_BIG,
	B_RGB32_BIG,
	B_RGBA32_BIG,
	B_GRAY8,
	B_GRAY1
};

// Local functions

void __h_align(BDAlignment align, float ps, float lw, float *po,
	float *lo)
{
	if (align != BD_ALIGN_LEFT) {
		if (ps > lw) {
			*lo = (align == BD_ALIGN_RIGHT ? ps - lw :
				ceil((ps - lw) / 2.0));
		}
		else {
			*po = (align == BD_ALIGN_RIGHT ? lw - ps:
				ceil((lw - ps) / 2.0));
		}
	}
}

void __v_align(BDAlignment align, float ps, float lh, float *po,
	float *lo)
{
	if (align != BD_ALIGN_TOP) {
		if (ps > lh) {
			*lo = (align == BD_ALIGN_BOTTOM ? ps - lh :
				ceil((ps - lh) / 2.0));
		}
		else {
			*po = (align == BD_ALIGN_BOTTOM ? lh - ps:
				ceil((lh - ps) / 2.0));
		}
	}
}

bool _check_color_space(color_space space)
{
	int i = 0;
	do {
		if (space == kColorSpaces[i]) return true;
	} while (++i < COLORSPACES_COUNT);
	return false;
}

// ============================================================================
// Private
// ============================================================================

void ButtonDecorator::_calc_rects(BView *view, BRect **picture, BRect **label)
{
	float ps = 0.0, lw = 0.0, lh = 0.0;
	BRect *p = NULL, *l = NULL;

	// Label size
	if (fLabel != NULL) {
		font_height fh;
		view->GetFontHeight(&fh);
		lh = ceil(fh.ascent + fh.descent);
		if (lh > 0.0) {
			lw = ceil(view->StringWidth(fLabel));
			if (lw == 0.0)
				lh = 0.0;
			else {
				lw++;
				lh++;
			}
		}
	}

	// Picture size
	if (fPictureType != BD_PICTURE_NONE)
		ps = floor(fPictureSize);

	// If picture is hidden don't report its rectangle, but only if we have at
	// least a label to show
	if (fVisible == BD_VISIBLE_LABEL && lw > 0.0)
		ps = 0.0;
			

	// If label is hidden don't report its rectangle, but only if we have at
	// least a picture to show
	if (fVisible == BD_VISIBLE_PICTURE && ps > 0.0) {
		lw = 0.0;
		lh = 0.0;
	}

	// Picture only
	if (lw == 0.0 && ps > 0.0) {
		p = new BRect(0.0, 0.0, ps - 1.0, ps - 1.0);
		p->OffsetBy(floor(fMarginLeft), floor(fMarginTop));
		l = NULL;
	}

	// Label only
	if (lw > 0.0 && ps == 0.0) {
		p = NULL;
		l = new BRect(0.0, 0.0, lw - 1.0, lh - 1.0);
		l->OffsetBy(floor(fMarginLeft), floor(fMarginTop));
	}

	// Picture and label
	if (lw > 0.0 && ps > 0.0) {
		float sp = floor(fSpacing) + 1.0, po = 0.0, lo = 0.0;
		switch (fPosition) {
			case BD_POSITION_ABOVE:
				__h_align(fAlign, ps, lw, &po, &lo);
				p = new BRect(0.0, 0.0, ps - 1.0, ps - 1.0);
				p->OffsetBy(floor(fMarginLeft) + po, floor(fMarginTop));
				l = new BRect(0.0, 0.0, lw - 1.0, lh - 1.0);
				l->OffsetBy(floor(fMarginLeft) + lo, p->bottom + sp);
				break;
			case BD_POSITION_BELOW:
				__h_align(fAlign, ps, lw, &po, &lo);
				l = new BRect(0.0, 0.0, lw - 1.0, lh - 1.0);
				l->OffsetBy(floor(fMarginLeft) + lo, floor(fMarginTop));
				p = new BRect(0.0, 0.0, ps - 1.0, ps - 1.0);
				p->OffsetBy(floor(fMarginLeft) + po, l->bottom + sp);
				break;
			case BD_POSITION_LEFT:
				__v_align(fAlign, ps, lh, &po, &lo);
				p = new BRect(0.0, 0.0, ps - 1.0, ps - 1.0);
				p->OffsetBy(floor(fMarginLeft), floor(fMarginTop) + po);
				l = new BRect(0.0, 0.0, lw - 1.0, lh - 1.0);
				l->OffsetBy(p->right + sp, floor(fMarginTop) + lo);
				break;
			case BD_POSITION_RIGHT:
				__v_align(fAlign, ps, lh, &po, &lo);
				l = new BRect(0.0, 0.0, lw - 1.0, lh - 1.0);
				l->OffsetBy(floor(fMarginLeft), floor(fMarginTop) + lo);
				p = new BRect(0.0, 0.0, ps - 1.0, ps - 1.0);
				p->OffsetBy(l->right + sp, floor(fMarginTop) + po);
				break;
		}
	}

	// Report the rectangles
	if (picture != NULL) *picture = p;
	if (label != NULL) *label = l;
}

void ButtonDecorator::_delete_bitmap_set(int num)
{
	BDBitmapSet *set = fBitmaps[num];
	if (set == NULL) return;
	for (int i = 0; i < 8; i++) {
		if (set->bitmap[i] != NULL)
			delete set->bitmap[i];
	}
	delete fBitmaps[num];
	fBitmaps.erase(fBitmaps.begin() + num);
}

int ButtonDecorator::_get_real_pic_index(int status)
{
	if (fPictureType == BD_PICTURE_BITMAP && fCurrentSet != NULL) {
		int s = (status & BD_STATUS_DISABLED ? status - 1 : status);
		while (fCurrentSet->bitmap[s] == NULL) s -= 2;
		if (status & BD_STATUS_DISABLED) s++;
		return s;
	}
	else if (fPictureType == BD_PICTURE_SVG) {
		// TODO
	}
	return -1;
}

void ButtonDecorator::_init(void)
{
	// TODO initialize SVG here if needed
	fCurrentSet = NULL;
	fLabel = NULL;
	fVisible = BD_VISIBLE_BOTH;
	fPictureSize = 1.0;
	fPictureType = BD_PICTURE_NONE;
	fPosition = BD_POSITION_ABOVE;
	fAlign = BD_ALIGN_CENTER;
	fSpacing = 3.0;
	fMarginLeft = 0.0;
	fMarginTop = 0.0;
	fMarginRight = 0.0;
	fMarginBottom = 0.0;
	fNeedsUpdate = true;
}

void ButtonDecorator::_select_current_set(void)
{
	fCurrentSet = GetBitmapSet(floor(fPictureSize));
}

// ============================================================================
// Protected
// ============================================================================

void ButtonDecorator::DrawLabel(BView *view, BRect rect, int status)
{
	BPoint realPos(rect.LeftTop());
	font_height fe;

	// BeOS draws text at baseline. One pixel above the baseline,
	// to be precise.
	view->GetFontHeight(&fe);
	realPos.y += fe.ascent + 1.0;

	// Alpha blending is to get a nice anti-aliasing
	view->SetDrawingMode(B_OP_ALPHA);

	// Draw a shadow for disabled effect
	if (status & BD_STATUS_DISABLED) {
		view->SetHighColor(tint_color(view->LowColor(), B_LIGHTEN_MAX_TINT));
		view->DrawString(fLabel, BPoint(realPos.x + 1.0, realPos.y + 1.0));
		view->SetHighColor(tint_color(view->LowColor(), B_DARKEN_4_TINT));
	}

	// Draw the label
	view->DrawString(fLabel, realPos);
}

void ButtonDecorator::DrawPicture(BView *view, BRect rect, int status)
{
	// TODO SVG

	if (fCurrentSet == NULL) return;

	int index = _get_real_pic_index(status);

	if (index == -1) return;

	BBitmap *bitmap = fCurrentSet->bitmap[index];
	BPoint realPos(rect.LeftTop());

	// Fancy alpha blending!
	view->SetDrawingMode(B_OP_ALPHA);

	// Center the bitmap
	// TODO resize the bitmap
	if (fCurrentSet->size < floor(fPictureSize)) {
		float offset = floor((floor(fPictureSize) - fCurrentSet->size) /
			2.0);
		realPos.x += offset;
		realPos.y += offset;
	}

	// Draw picture
	view->DrawBitmap(bitmap, realPos);
}

// ============================================================================
// Constructors and destructor
// ============================================================================

ButtonDecorator::ButtonDecorator(const char *label, BBitmap *picture,
	BDPosition position)
{
	BDBitmapSet *set;
	BRect r;
	int i;

	_init();
	SetLabel(label);
	SetPosition(position);

	if (picture == NULL) return;
	if (!picture->IsValid()) return;
	r = picture->Bounds();
	if (r.IntegerWidth() != r.IntegerHeight()) return;
	if (!_check_color_space(picture->ColorSpace())) return;

	set = new BDBitmapSet;
	set->size = r.IntegerWidth() + 1;
	set->bitmap[0] = picture;
	set->bitmap[1] = BitmapUtils::Grayscale(picture);
	for (i = 2; i < 8; i++)
		set->bitmap[i] = NULL;

	fBitmaps.push_back(set);
	fCurrentSet = set;
	fPictureType = BD_PICTURE_BITMAP;
	fPictureSize = set->size;
}

ButtonDecorator::ButtonDecorator(BMessage *archive) :
	BArchivable(archive)
{
	float aFloat, aFloat2, aFloat3, aFloat4;
	const char *aString;
	int32 anInt32;

	_init();
	if (archive == NULL) return; // I feel better

	// Bitmap
	if (archive->FindInt32("ButtonDecorator::bitmap_sets_count",
	  &anInt32) == B_OK) {
	  	BArchivable *archivable;
	  	BBitmap *bitmaps[8];
	  	BMessage *message;
		char name[64];
		int32 i, j;
		for (i = 0; i < anInt32; i++) {
			for (j = 0; j < 8; j++) {
				bitmaps[j] = NULL;
				sprintf(name, "ButtonDecorator::bitmap_%d_%d", (int)i, (int)j);
				message = new BMessage();
				if (archive->FindMessage(name, message) == B_OK) {
					archivable = BBitmap::Instantiate(message);
					if (archivable != NULL) {
						bitmaps[j] = dynamic_cast<BBitmap*>(archivable);
						if (bitmaps[j] == NULL)
							delete archivable;
					}
				}
				delete message;
			}
			SetPicture(bitmaps);
		}
		if (fBitmaps.size() > 0)
			fPictureType = BD_PICTURE_BITMAP;
	}

	// SVG
	if (fPictureType == BD_PICTURE_NONE) {
		// TODO
	}
	
	// Label
	if (archive->FindString("ButtonDecorator::label", &aString) == B_OK)
		SetLabel(aString);

	// Visible
	if (archive->FindInt32("ButtonDecorator::visible", &anInt32) == B_OK)
		SetVisible(static_cast<BDVisible>(anInt32));

	// Position
	if (archive->FindInt32("ButtonDecorator::position", &anInt32) == B_OK)
		SetPosition(static_cast<BDPosition>(anInt32));

	// Alignment
	if (archive->FindInt32("ButtonDecorator::alignment", &anInt32) == B_OK)
		SetAlignment(static_cast<BDAlignment>(anInt32));

	// Picture size
	if (archive->FindFloat("ButtonDecorator::picture_size", &aFloat) == B_OK)
		SetPictureSize(aFloat);

	// Spacing
	if (archive->FindFloat("ButtonDecorator::spacing", &aFloat) == B_OK)
		SetSpacing(aFloat);

	// Margins
	if (archive->FindFloat("ButtonDecorator::left_margin", &aFloat) == B_OK &&
	  archive->FindFloat("ButtonDecorator::top_margin", &aFloat2) == B_OK &&
	  archive->FindFloat("ButtonDecorator::right_margin", &aFloat2) == B_OK &&
	  archive->FindFloat("ButtonDecorator::bottom_margin", &aFloat2) == B_OK)
		SetMargins(aFloat, aFloat2, aFloat3, aFloat4);
}

ButtonDecorator::~ButtonDecorator()
{
	while (fBitmaps.size() > 0)
		_delete_bitmap_set(0);
	// TODO delete the SVG
	if (fLabel != NULL)
		delete[] fLabel;
}

// ============================================================================
// Public
// ============================================================================

BDAlignment ButtonDecorator::Alignment(void)
{
	return fAlign;
}

status_t ButtonDecorator::Archive(BMessage *archive, bool deep) const
{
	status_t status = BArchivable::Archive(archive, deep);

	// Bitmap
	if (status == B_OK && fPictureType == BD_PICTURE_BITMAP) {
		BMessage *message;
		int32 sets, i, j;
		BBitmap *bitmap;
		char name[64];
		sets = fBitmaps.size();
		status = archive->AddInt32("ButtonDecorator::bitmap_sets_count", sets);
		for (i = 0; i < sets && status == B_OK; i++) {
			for (j = 0; j < 8 && status == B_OK; j++) {
				bitmap = fBitmaps[i]->bitmap[j];
				if (bitmap != NULL) {
					message = new BMessage();
					status = bitmap->Archive(message, deep);
					if (status == B_OK) {
						sprintf(name, "ButtonDecorator::bitmap_%d_%d",
							(int)i, (int)j);
						status = archive->AddMessage(name, message);
					}
					delete message;
				}
			}
		}
	}

	// SVG
	if (status == B_OK && fPictureType == BD_PICTURE_SVG) {
		// SVG
	}

	// Label
	if (status == B_OK && fLabel != NULL)
		status = archive->AddString("ButtonDecorator::label", fLabel);

	// Visible
	if (status == B_OK && fVisible != BD_VISIBLE_BOTH)
		status = archive->AddInt32("ButtonDecorator::visible",
			static_cast<int32>(fVisible));

	// Position
	if (status == B_OK && fPosition != BD_POSITION_ABOVE)
		status = archive->AddInt32("ButtonDecorator::position",
			static_cast<int32>(fPosition));

	// Alignment
	if (status == B_OK && fAlign != BD_ALIGN_CENTER)
		status = archive->AddInt32("ButtonDecorator::alignment",
			static_cast<int32>(fAlign));

	// Picture size
	if (status == B_OK)
		status = archive->AddFloat("ButtonDecorator::picture_size",
			fPictureSize);

	// Spacing
	if (status == B_OK)
		status = archive->AddFloat("ButtonDecorator::spacing", fSpacing);

	// Margins
	if (status == B_OK)
		status = archive->AddFloat("ButtonDecorator::left_margin", fMarginLeft);
	if (status == B_OK)
		status = archive->AddFloat("ButtonDecorator::top_margin", fMarginTop);
	if (status == B_OK)
		status = archive->AddFloat("ButtonDecorator::right_margin",
			fMarginRight);
	if (status == B_OK)
		status = archive->AddFloat("ButtonDecorator::bottom_margin",
			fMarginBottom);

	return status;
}

unsigned ButtonDecorator::CountBitmapSets(void)
{
	if (fPictureType != BD_PICTURE_BITMAP) return 0;
	return fBitmaps.size();
}

bool ButtonDecorator::DeleteBitmapSet(unsigned size)
{
	if (fPictureType != BD_PICTURE_BITMAP) return false;
	BDBitmapSet *set = GetBitmapSet(size);
	if (set == NULL) return false;
	int index = IndexOfBitmapSet(set);
	_delete_bitmap_set(index);
	if (fBitmaps.size() == 0)
		fPictureType = BD_PICTURE_NONE;
	return true;
	fNeedsUpdate = true; // TODO only if currently used bitmap has been deleted
	return true;
}

bool ButtonDecorator::DeleteBitmapSetAt(unsigned index)
{
	if (fPictureType != BD_PICTURE_BITMAP || index >= fBitmaps.size())
		return false;
	_delete_bitmap_set(index);
	if (fBitmaps.size() == 0) {
		fCurrentSet = NULL;
		fPictureType = BD_PICTURE_NONE;
	}
	else
		_select_current_set();
	fNeedsUpdate = true; // TODO only if currently used bitmap has been deleted
	return true;
}

void ButtonDecorator::DeletePicture(void)
{
	if (fPictureType == BD_PICTURE_NONE) return;
	if (fPictureType == BD_PICTURE_BITMAP) {
		while (fBitmaps.size() > 0)
			_delete_bitmap_set(0);
		fCurrentSet = NULL;
	}
	else if (fPictureType == BD_PICTURE_SVG) {
		// TODO
	}
	fPictureType = BD_PICTURE_NONE;
	fNeedsUpdate = true;
}

void ButtonDecorator::Draw(BView *view, BRect rect, int status)
{
	if (view == NULL || !rect.IsValid())
		return;
	if (!(status == BD_STATUS_NORMAL || status == BD_STATUS_DISABLED ||
	  status == BD_STATUS_PUSHED || status == BD_STATUS_PUSHED_DISABLED ||
	  status == BD_STATUS_OVER || status == BD_STATUS_OVER_DISABLED ||
	  status == BD_STATUS_OVER_PUSHED ||
	  status == BD_STATUS_OVER_PUSHED_DISABLED))
		return;

	BRect *picture, *label;
	BRegion region;

	_calc_rects(view, &picture, &label);
	if (picture == NULL && label == NULL) return;

	if (picture != NULL) {
		view->PushState();
		picture->OffsetBy(rect.left, rect.top);
		region.Set(*picture);
		view->ConstrainClippingRegion(&region);
		DrawPicture(view, *picture, status);
		delete picture;
		view->PopState();
	}

	if (label != NULL) {
		view->PushState();
		label->OffsetBy(rect.left, rect.top);
		region.Set(*label);
		view->ConstrainClippingRegion(&region);
		DrawLabel(view, *label, status);
		delete label;
		view->PopState();
	}

	fNeedsUpdate = false;
}

BDBitmapSet * ButtonDecorator::GetBitmapSet(unsigned size)
{
	if (fPictureType != BD_PICTURE_BITMAP || size == 0) return NULL;
	BDBitmapSet *candidate = NULL;
	unsigned i = 0;
	while (i < fBitmaps.size()) {
		if (fBitmaps[i]->size == size)
			return fBitmaps[i];
		if (fBitmaps[i]->size < size) {
			if (candidate == NULL)
				candidate = fBitmaps[i];
			else {
				if (fBitmaps[i]->size > candidate->size)
					candidate = fBitmaps[i];
			}
		}
		i++;
	}
	return candidate;
}

BDBitmapSet * ButtonDecorator::GetBitmapSetAt(unsigned index)
{
	return (fPictureType != BD_PICTURE_BITMAP || index >= fBitmaps.size() ?
		NULL : fBitmaps[index]);
}

void ButtonDecorator::GetPicture(BBitmap *picture[8])
{
	bool isnull = (fPictureType == BD_PICTURE_BITMAP || fCurrentSet == NULL);
	if (!isnull) {
		for (int  i = 0; i < 8; i++)
			picture[i] = fCurrentSet->bitmap[i];
	}
	else {
		for (int i = 0; i < 8; i++)
			picture[i] = NULL;
	}
}

void ButtonDecorator::GetPreferredSize(BView *view, float *width, float *height)
{
	if (view == NULL || (width == NULL && height == NULL)) return;

	BRect *picture, *label;
	float w = floor(fMarginLeft) + floor(fMarginRight),
		h = floor(fMarginTop) + floor(fMarginBottom);

	_calc_rects(view, &picture, &label);

	// Picture only
	if (picture != NULL && label == NULL) {
		w += picture->Width();
		h += picture->Height();
	}

	// Label only
	if (picture == NULL && label != NULL) {
		w += label->Width();
		h += label->Height();
	}

	// Picture and label
	if (picture != NULL && label != NULL) {
		float ps = picture->Width() + 1.0,
			lw = label->Width() + 1.0,
			lh = label->Height() + 1.0,
			sp = floor(fSpacing);
		switch (fPosition) {
			case BD_POSITION_ABOVE:
			case BD_POSITION_BELOW:
				w += (lw > ps ? lw : ps) - 1.0;
				h += ps + sp + lh - 1.0;
				break;
			case BD_POSITION_LEFT:
			case BD_POSITION_RIGHT:
				w += ps + sp + lw - 1.0;
				h += lh > ps ? lh : ps - 1.0;
				break;
		}
	}

	// Report the results
	if (width != NULL) *width = w;
	if (height != NULL) *height = h;
}

int ButtonDecorator::IndexOfBitmapSet(BDBitmapSet *set)
{
	if (fPictureType != BD_PICTURE_BITMAP || set == NULL) return -1;
	for (unsigned i = 0; i < fBitmaps.size(); i++) {
		if (fBitmaps[i] == set) return i;
	}
	return -1;
}

BArchivable * ButtonDecorator::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "ButtonDecorator") ?
		new ButtonDecorator(archive) : NULL);
}

const char * ButtonDecorator::Label(void)
{
	return fLabel;
}

void ButtonDecorator::Margins(float *left, float *top, float *right,
	float *bottom)
{
	if (left != NULL) *left = fMarginLeft;
	if (top != NULL) *top = fMarginLeft;
	if (right != NULL) *right = fMarginLeft;
	if (bottom != NULL) *bottom = fMarginLeft;
}

bool ButtonDecorator::NeedsUpdate(void)
{
	return fNeedsUpdate;
}

float ButtonDecorator::PictureSize(void)
{
	return fPictureSize;
}

BDPictureType ButtonDecorator::PictureType(void)
{
	return fPictureType;
}

BDPosition ButtonDecorator::Position(void)
{
	return fPosition;
}

bool ButtonDecorator::SetAlignment(BDAlignment align)
{
	if (align != BD_ALIGN_LEFT && align != BD_ALIGN_RIGHT &&
	  align != BD_ALIGN_TOP && align != BD_ALIGN_BOTTOM)
		return false;
	if (align != fAlign) {
		fAlign = align;
		fNeedsUpdate = true;
	}
	return true;
}

void ButtonDecorator::SetLabel(const char *label)
{
	if (fLabel != NULL) {
		delete[] fLabel;
		fLabel = NULL;
	}
	if (label == NULL) return;
	size_t len = strlen(label);
	if (len == 0) return;
	fLabel = new char[len + 1];
	memcpy(fLabel, label, len + 1);
	fNeedsUpdate = true;
}

bool ButtonDecorator::SetMargins(float left, float top, float right,
	float bottom)
{
	if (left < 0.0 || top < 0.0 || right < 0.0 || bottom < 0.0)
		return false;
	if (left != fMarginLeft || top != fMarginTop || right == fMarginRight ||
	  bottom != fMarginBottom) {
		fMarginLeft = left;
		fMarginTop = top;
		fMarginRight = right;
		fMarginBottom = bottom;
		fNeedsUpdate = true;
	}
	return true;
}

bool ButtonDecorator::SetPicture(BBitmap *picture[8])
{
	color_space space, sp;
	BDBitmapSet *set;
	unsigned i, j;
	float size;
	BRect r;

	// Check validity of first bitmap
	if (picture[0] == NULL) return false;
	if (!picture[0]->IsValid()) return false;
	r = picture[0]->Bounds();
	size = r.IntegerHeight();
	space = picture[0]->ColorSpace();
	if (r.IntegerWidth() != size || !_check_color_space(space))
	  	return false;

	// Check that no picture is given twice
	for (i = 0; i < 8; i++) {
		if (picture[i] != NULL) {
			for (j = 0; j < 8; j++) {
				if (i != j && picture[i] == picture[j])
					return false;
			}
		}
	}

	// Check that no disabled pictures are given without the corresponding
	// non-disabled
	for (i = 2; i < 7; i += 2) {
		if (picture[i] == NULL && picture[i + 1] != NULL)
			return false;
	}

	// Check that all bitmaps are valid, have the same size and the same color
	// space
	for (i = 1; i < 8; i++) {
		if (picture[i] != NULL) {
			if (!picture[i]->IsValid())
				return false;
			r = picture[i]->Bounds();
			sp = picture[i]->ColorSpace();
			if (r.IntegerHeight() != size || r.IntegerWidth() != size ||
			  sp != space)
				return false;
		}
	}

	// If we are using SVG, delete it
	// TODO

	// Create the bitmap set
	set = new BDBitmapSet;
	set->size = (unsigned)size + 1;
	for (i = 0; i < 8; i++) {
		if (i % 2 == 1 && picture[i] == NULL)
			set->bitmap[i] = BitmapUtils::Grayscale(picture[i - 1]);
		else
			set->bitmap[i] = picture[i];
	}

	// Find a set to replace, or just add it
	i = 0;
	while (i < fBitmaps.size()) {
		if (fBitmaps[i]->size == set->size) {
			_delete_bitmap_set(i);
			fBitmaps.push_back(set);
			break;
		}
		i++;
	}
	if (i == fBitmaps.size())
		fBitmaps.push_back(set);

	// Check if update is needed
	fPictureType = BD_PICTURE_BITMAP;
	set = fCurrentSet;
	_select_current_set();
	if (fCurrentSet != set)
		fNeedsUpdate = true;

	return true;
}

bool ButtonDecorator::SetPictureSize(float size)
{
	if (size < 1.0) return false;
	if (size != fPictureSize) {
		fPictureSize = size;
		_select_current_set();
		fNeedsUpdate = true;
	}
	return true;
}

bool ButtonDecorator::SetPosition(BDPosition position)
{
	if (position != BD_POSITION_ABOVE && position != BD_POSITION_BELOW &&
	  position != BD_POSITION_LEFT && position != BD_POSITION_RIGHT)
		return false;
	if (position != fPosition) {
		fPosition = position;
		fNeedsUpdate = true;
	}
	return true;
}

bool ButtonDecorator::SetSpacing(float spacing)
{
	if (spacing < 0.0) return false;
	if (spacing != fSpacing) {
		fSpacing = spacing;
		fNeedsUpdate = true;
	}
	return true;
}

bool ButtonDecorator::SetVisible(BDVisible visible)
{
	if (visible != BD_VISIBLE_BOTH && visible != BD_VISIBLE_LABEL &&
	  visible != BD_VISIBLE_PICTURE)
		return false;
	if (visible != fVisible) {
		fVisible = visible;
		fNeedsUpdate = true;
	}
	return true;
}

float ButtonDecorator::Spacing(void)
{
	return fSpacing;
}

BDVisible ButtonDecorator::Visible(void)
{
	return fVisible;
}
