// =============================================================================
//
// libwalter Toolbar.h
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

#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

// Standard C++ headers
#include <vector>

// Haiku headers
#include <View.h>
#include <Messenger.h>

// libwalter classes
class WToolbarItem;

// Toolbar pre-defined styles
enum {
	W_TOOLBAR_STYLE_FLAT	= 0,
	W_TOOLBAR_STYLE_3D		= 1,
	W_TOOLBAR_STYLE_MENU	= 2
};

// Toolbar alignment
typedef enum {
	W_TOOLBAR_HORIZONTAL,
	W_TOOLBAR_VERTICAL
} WToolbarAlignment;

// Label position
typedef enum {
	W_TOOLBAR_LABEL_NONE,
	W_TOOLBAR_LABEL_BOTTOM,
	W_TOOLBAR_LABEL_SIDE
} WToolbarLabelPosition;

// Standard picture sizes
enum {
	W_TOOLBAR_PICTURE_NONE		=   0,
	W_TOOLBAR_PICTURE_XSMALL	=  16,
	W_TOOLBAR_PICTURE_SMALL		=  24,
	W_TOOLBAR_PICTURE_MEDIUM	=  32,
	W_TOOLBAR_PICTURE_LARGE		=  48,
	W_TOOLBAR_PICTURE_XLARGE	=  64,
	W_TOOLBAR_PICTURE_XXLARGE	=  96,
	W_TOOLBAR_PICTURE_XXXLARGE	= 128,
	W_TOOLBAR_PICTURE_HUGE		= 256,
	W_TOOLBAR_PICTURE_MAX		= 512
};

// Items limits
#define W_TOOLBAR_MAX_LINES					  256
#define W_TOOLBAR_MAX_ITEMS_PER_LINE		  256
#define W_TOOLBAR_LAST_LINE					0x100
#define W_TOOLBAR_LAST_POSITION				0x100

// Toolbar appearance
typedef struct {
	WToolbarAlignment		fAlignment;		// Horizontal or vertical
	border_style			fBorder;		// As defined in View.h
	BBitmap *				fBackBitmap;	// Background bitmap
	WToolbarLabelPosition	fLabelPosition;	// Bottom or side
	float					fMargin;		// Free space around each item
	float					fPadding;		// Distance between border and items
	float					fPicSize;		// Picture size (0 = no pic)
	int						fStyle;			// One of the pre-defined styles
} WToolbarAppearance;

// =============================================================================
// WToolbar
// =============================================================================

class WToolbar : public BView {
private:
	typedef vector<WToolbarItem*> WToolbarLine;
			void					_draw_item(unsigned line, unsigned position,
										BRect updateRect);
			void					_init_object(void);

	// Appearance
			WToolbarAppearance		fAppearance;

	// Properties
			bool					fAutoSize;
			bool					fEnabled;
			BMessenger				fTarget;

	// Internals
			vector<WToolbarLine*>	fLines;
			bool					fDisableUpdate;
			bool					fDisableStyling;
			WToolbarItem *			fMouseDownItem;
			WToolbarItem *			fMouseOverItem;

protected:
			void					AssertLocked(void);
	virtual	float					BorderThickness(void);
	virtual	void					DrawBackground(BRect updateRect);
	virtual	void					DrawBorder(BRect updateRect);

public:
									WToolbar(BRect frame, const char *name,
										int style = W_TOOLBAR_STYLE_FLAT,
										WToolbarAlignment alignment =
										W_TOOLBAR_HORIZONTAL,
										bool auto_size = true,
										border_style border =
										B_FANCY_BORDER,
										uint32 resizing_mode =
										B_FOLLOW_LEFT | B_FOLLOW_TOP,
										uint32 flags = B_FRAME_EVENTS |
										B_FULL_UPDATE_ON_RESIZE |
										B_WILL_DRAW);
									WToolbar(BMessage *archive);
	virtual							~WToolbar();

	// BArchivable hooks
	virtual	status_t				Archive(BMessage *archive,
										bool deep = true) const;
	static	BArchivable *			Instantiate(BMessage *archive);

	// BHandler hooks
	//virtual	status_t			GetSupportedSuites(BMessage *message);
	virtual	void					MessageReceived(BMessage *message);
	//virtual	BHandler *			ResolveSpecifier(BMessage *message,
	//									int32 index, BMessage *specifier,
	//									int32 what, const char *property);

	// BView hooks
	virtual	void					AttachedToWindow(void);
	virtual	void					DetachedFromWindow(void);
	virtual	void					Draw(BRect updateRect);
	virtual	void					FrameResized(float width, float height);
	virtual	void					GetPreferredSize(float *width,
										float *height);
	virtual	void					MouseDown(BPoint point);
	virtual	void					MouseMoved(BPoint point, uint32 transit,
										const BMessage *message);
	virtual	void					MouseUp(BPoint point);

	// Items management
			bool					AddItem(WToolbarItem *item,
										int line = W_TOOLBAR_LAST_LINE,
										int position = W_TOOLBAR_LAST_POSITION);
			unsigned				CountItems(bool visibleOnly = false);
			unsigned				CountItemsInLine(unsigned line,
										bool visibleOnly = false);
			unsigned				CountLines(bool visibleOnly = false);
			void					DeleteLine(unsigned line);
			void					DrawItem(WToolbarItem *item);
			WToolbarItem *			FindFlexibleItem(unsigned line = 0);
			WToolbarItem *			FindItem(const char *name);
			int						IndexOf(WToolbarItem *item);
			WToolbarItem *			ItemAt(BPoint point);
			WToolbarItem *			ItemAt(int index);
			WToolbarItem *			ItemAt(unsigned line, unsigned position,
										bool visibleOnly = false);
			bool					LineEnabled(unsigned line);
			bool					LineVisible(unsigned line);
			bool					MoveItem(WToolbarItem *item, 
										int line,
										int position = W_TOOLBAR_LAST_POSITION);
			int						PositionOf(WToolbarItem *item);
			bool					RemoveItem(WToolbarItem *item);
			void					SetLineEnabled(unsigned line, bool enabled);
			void					SetLineVisible(unsigned line, bool visible);

	// Properties
			bool					AutoSize(void);
			bool					Enabled(void);
			void					SetAutoSize(bool auto_size);
			void					SetEnabled(bool enabled);

	// Appearance
			WToolbarAlignment		Alignment(void);
			BBitmap *				BackgroundBitmap(void);
			border_style			Border(void);
			WToolbarLabelPosition	LabelPosition(void);
			float					Margin(void);
			float					Padding(void);
			float					PictureSize(void);
			void					SetAlignment(
										WToolbarAlignment alignment);
			void					SetBackgroundBitmap(BBitmap *bitmap);
			void					SetBorder(border_style border);
			void					SetLabelPosition(WToolbarLabelPosition
										label_position);
			void					SetMargin(float margin);
			void					SetPadding(float padding);
			void					SetPictureSize(float picture_size);
	virtual	void					SetStyle(int style);
			int						Style(void);

	// Target
			bool					IsTargetLocal(void) const;
			BMessenger				Messenger(void) const;
			status_t				SetControlsTarget(BMessenger messenger);
			status_t				SetControlsTarget(const BHandler *handler,
										const BLooper *looper = NULL);
			status_t				SetTarget(BMessenger messenger);
			status_t				SetTarget(const BHandler *handler,
										const BLooper *looper = NULL);
			BHandler *				Target(BLooper **looper = NULL) const;

	// Other methods
			void					Update(void);
};

#endif // _TOOLBAR_H_
