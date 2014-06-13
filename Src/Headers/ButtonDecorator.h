// ============================================================================
//
// libwalter ButtonDecorator.h
//
// Distributed under the terms of the MIT License.
//
// Authors:
//		Gabriele Biffi - http://www.biffuz.it/
//
// ============================================================================

#ifndef __BUTTONDECORATOR_H__
#define __BUTTONDECORATOR_H__

// Standard C++ headers
#include <vector>
using std::vector;

// Haiku headers
#include <Archivable.h>
#include <Rect.h>

// Haiku classes
class BBitmap;
class BView;

// What is visible
typedef enum {
	BD_VISIBLE_BOTH = 0,
	BD_VISIBLE_LABEL,
	BD_VISIBLE_PICTURE
} BDVisible;

// Position of the picture relative to label
typedef enum {
	BD_POSITION_ABOVE = 0,
	BD_POSITION_BELOW,
	BD_POSITION_LEFT,
	BD_POSITION_RIGHT
} BDPosition;

// Alignment of picture and label
typedef enum {
	// Values eligible for BD_POSITION_ABOVE and BD_POSITION_BELOW
	BD_ALIGN_LEFT = 1,
	BD_ALIGN_RIGHT,
	// Values eligible for BD_POSITION_LEFT and BD_POSITION_RIGHT
	BD_ALIGN_TOP,
	BD_ALIGN_BOTTOM,
	// Value eligible for all positions
	BD_ALIGN_CENTER = 0
} BDAlignment;

// Status constants
#define BD_STATUS_NORMAL				0
#define BD_STATUS_DISABLED				1
#define BD_STATUS_PUSHED				2
#define BD_STATUS_PUSHED_DISABLED		BD_STATUS_PUSHED | BD_STATUS_DISABLED
#define BD_STATUS_OVER					4
#define BD_STATUS_OVER_DISABLED			BD_STATUS_OVER | BD_STATUS_DISABLED
#define BD_STATUS_OVER_PUSHED			BD_STATUS_OVER | BD_STATUS_PUSHED
#define BD_STATUS_OVER_PUSHED_DISABLED	BD_STATUS_OVER | BD_STATUS_PUSHED | \
										BD_STATUS_DISABLED

// Picture type
typedef enum {
	BD_PICTURE_NONE = -1,
	BD_PICTURE_BITMAP = 0,
	BD_PICTURE_SVG
} BDPictureType;

// Bitmap set; contains bitmaps for all states of the same size
typedef struct {
	unsigned	size;
	BBitmap *	bitmap[8];
} BDBitmapSet;

class ButtonDecorator : public BArchivable {

private:

	void	_calc_rects(BView *view, BRect **picture, BRect **label);
	void	_delete_bitmap_set(int num);
	int		_get_real_pic_index(int status);
	void	_init(void);
	void	_select_current_set(void);

	// Content
	vector<BDBitmapSet*>	fBitmaps;
	BDBitmapSet *			fCurrentSet;
	char *					fLabel;

	// Content properties
	BDVisible		fVisible;
	float			fPictureSize;
	BDPictureType	fPictureType;

	// Position and alignment
	BDPosition	fPosition;
	BDAlignment	fAlign;

	// Spacing and margins
	float	fSpacing;
	float	fMarginLeft, fMarginTop, fMarginRight, fMarginBottom;

	// Needs update?
	bool	fNeedsUpdate;

protected:

	virtual	void	DrawLabel(BView *view, BRect rect, int status);
	virtual	void	DrawPicture(BView *view, BRect rect, int status);

public:

			ButtonDecorator(const char *label, BBitmap *picture,
				BDPosition position);
			ButtonDecorator(BMessage *archive);
	virtual	~ButtonDecorator();

	// BArchivable hooks
	static	BArchivable *	Instantiate(BMessage *archive);
	virtual	status_t		Archive(BMessage *archive, bool deep = true) const;

	// Picture
			void	GetPicture(BBitmap *picture[8]);
	virtual	bool	SetPicture(BBitmap *picture[8]);
	virtual	void	DeletePicture(void);

	// Special for bitmaps
			unsigned		CountBitmapSets(void);
			int				IndexOfBitmapSet(BDBitmapSet *set);
			BDBitmapSet *	GetBitmapSet(unsigned size);
			BDBitmapSet *	GetBitmapSetAt(unsigned index);
			bool			DeleteBitmapSet(unsigned size);
			bool			DeleteBitmapSetAt(unsigned index);

	// Label
	const char *	Label(void);
	void			SetLabel(const char *label);

	// Content properties
			BDVisible		Visible(void);
	virtual	bool			SetVisible(BDVisible visible);
			float			PictureSize(void);
	virtual	bool			SetPictureSize(float size);
			BDPictureType	PictureType(void);

	// Position and alignment
			BDPosition	Position(void);
	virtual	bool		SetPosition(BDPosition position);
			BDAlignment	Alignment(void);
	virtual	bool		SetAlignment(BDAlignment align);

	// Spacing and margins
			float	Spacing(void);
	virtual	bool	SetSpacing(float);
			void	Margins(float *left, float *top, float *right,
						float *bottom);
	virtual	bool	SetMargins(float left, float top, float right,
						float bottom);

	// Methods
	virtual	void	GetPreferredSize(BView *view, float *width, float *height);
	virtual	void	Draw(BView *view, BRect rect, int status);
	virtual	bool	NeedsUpdate(void);
};


/*
 * Constructors only covers the simple case of a single picture for all states
 * except disabled: the picture is turned in grayscale to obtain the disabled
 * picture
 *
 * Calling a SetPicture method allows you to use a different picture for each
 * possible state:
 *  - normal
 *  - disabled
 *  - pushed
 *  - pushed, disabled
 *  - mouse over
 *  - mouse over, disabled
 *  - mouse over, pushed
 *  - mouse over, pushed, disabled
 * The following rules applies:
 *  - you must supply at least the normal picture
 *  - pictures will become property of the object
 *  - do not supply the same picture for two different states; if you need,
 *    make a copy of it first
 *  - do not supply a disabled picture without the corresponding non-disabled
 *    picture
 *  - if you don't supply the disabled picture for a state, it will be created
 *    by turning the non-disabled picture in grayscale
 *  - all bitmaps must be square and have the same size (BBitmap version only)
 *  - all bitmaps must use the B_RGBA32 or B_CMAP8 color space and use the
 *    same color space (BBitmap version only)
 *
 * When using SetPicture(BPicture or SVG) method, all existing pictures are
 * deleted
 * When using SetPicture(bitmap) method:
 *  - if current pictures are BPictures or SVGs, they are deleted
 *  - if current pictures are bitmaps, only bitmaps of the same size of the new
 *    bitmaps are deleted and replaced with the new ones; this allows to keep
 *    bitmaps in different sizes for different picture sizes (like "small" or
 *    "large" buttons in a toolbar)
 *
 * Label is given in the constructor, can be NULL
 *
 * By default both label and picture are visible
 * Picture size is taken from bitmap size in the bitmap constructor or
 * given in the SVG constructor; must be at least 1.0
 *
 * Position is given in the constructor
 * Alignment default to BD_ALIGN_CENTER; if the alignment is not eligible for
 * the position, default will be used
 *
 * Spacing is the distance between the picture and the label; defaults to 3.0,
 * can not be negative
 * Margins defaults to 0.0, can not be negative
 *
 */

#endif // __BUTTONDECORATOR_H__
