// =============================================================================
//
// libwalter ToolbarItem.cpp
//
// Base class for WToolbar items
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
 * \class WToolbarItem
 * \brief Base class for toolbar items
 *
 * WToolbarItem provides all the basic facilities to create a full featured
 * toolbar item. Everything you want to place in a toolbar must be a descendant
 * of this class; however, if you want a control that your user can interact
 * with, you'll probably want to subclass WToolbarControl. This concept is
 * similar to BView and BControl.
 *
 * libwalter supplies several widely used items and controls, such as
 * WToolbarSeparator and WToolbarButton, and many more.
 *
 * Being a descendant of BHandler, a toolbar item can become a target for
 * messages.
 *
 * An item per line in a toolbar can be set as "flexible": when the toolbar is
 * resized, the item will be stretched horizontally (or vertically) to fit
 * the toolbar; the following items will be pushed to the right (or to the
 * bottom).
 *
 * \par How to create your own item
 * Creating your own toolbar item by subclassing WToolbarItem is not very
 * different than subclassing BView. The most obvious methods you have to
 * overload are Draw() and GetPreferredSize().
 *
 * Inside Draw(), you can determine the visual aspect of your item by inspecting
 * the protected properties fMouseDown and fMouseOver. They are set by the
 * default implementations of MouseDown(), MouseUp() and MouseOver(); take care
 * of these properties if you overload these methods. An easy solution is to
 * call the default implementations inside your own.
 */

// libwalter headers
#include "Toolbar.h"
#include "ToolbarItem.h"

// =============================================================================
// WToolbarItem
// =============================================================================

// Constructors and destructors

/* Creates a new item with the given name. Like a BView's name, it is not
 * visible to the user, unless a descendant draws it. Name can be NULL.
 * A newly created item is not flexible and, once attached to a toolbar, will be
 * visible. 
 */
WToolbarItem::WToolbarItem(const char *name)
	: BHandler(name)
{
	_InitObject();
	SetName(name);
}

WToolbarItem::WToolbarItem(BMessage *archive)
	: BHandler(archive)
{
	bool enabled, visible, flexible;
	BMessage message;
	int line;

	_InitObject();

	if (archive->FindBool("WToolbarItem::flexible", &flexible) == B_OK)
		SetFlexible(flexible);

	if (archive->FindInt32("WToolbarItem::line", (int32*)(&line)) == B_OK)
		fLine = line;

	if (archive->FindBool("WToolbarItem::visible", &visible) == B_OK)
		SetVisible(visible);
}

WToolbarItem::~WToolbarItem()
{
	if (fToolbar != NULL)
		fToolbar->RemoveItem(this);
}

// Private

void
WToolbarItem::_InitObject(void)
{
	fFlexible = false;
	fFrame = BRect(0.0, 0.0, -1.0, -1.0);
	fHeight = 0.0;
	fLine = -1;
	fToolbar = NULL;
	fVisible = true;
	fWidth = 0.0;
}

// BArchivable hooks

/* Archive the item into the target BMessgae. The following fields will be
 * added if the respective properties are different from the default:
 *  WToolbarItem::			bool
 *  WToolbarItem::line		int32
 *  WToolbarItem::visible	bool
 */
status_t
WToolbarItem::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	// Ancestor
	status = BHandler::Archive(archive, deep);

	// Properties

	if (status == B_OK && fFlexible)
		status = archive->AddBool("WToolbarItem::flexible", fFlexible);

	if (status == B_OK && fToolbar != NULL)
		status = archive->AddInt32("WToolbarItem::line", (int32)fLine);

	if (status == B_OK && !fVisible)
		status = archive->AddBool("WToolbarItem::visible", fVisible);

	return status;
}

BArchivable *
WToolbarItem::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "WToolbarItem") ?
		new WToolbarItem(archive) : NULL);
}

// Public

/* This method is called when the item is attached to a toolbar. Default
 * implementation does nothing.
 */
void
WToolbarItem::AttachedToToolbar(void)
{
}

/* Return the bounds of the item, in item's units. This value has no meaning if
 * the item isn't attached to a toolbar.
 */
BRect
WToolbarItem::Bounds(void)
{
	return fFrame.OffsetToCopy(0.0, 0.0);
}

/* This method is called when the item is detached from the toolbar. Default
 * implementation does nothing.
 */
void
WToolbarItem::DetachedFromToolbar(void)
{
}

/* Draws the item on the given canvas (the toolbar itself, or an off-screen
 * buffer). The default implementation does nothing.
 */
void
WToolbarItem::Draw(BView *canvas, BRect update_rect)
{
}

/* Returns true if the item is flexible, false otherwise. Note that only one
 * item per line can be flexible; other flexible items in the same line will be
 * threated as normal items.
 */
bool
WToolbarItem::Flexible(void)
{
	return fFlexible;
}

/* Returns the frame of the item, in toolbar's units. This value has no meaning
 * if the item isn't attached to a toolbar.
 */
BRect
WToolbarItem::Frame(void)
{
	return fFrame;
}

/* Returns the preferred size of the item, in toolbar's units. The default
 * implementation returns -1 and -1; these values have no meaning if the item
 * isn't attached to a toolbar. You can pass NULL if you aren't interested in
 * one of the values.
 * The toolbar will use this as the minimun size; the item may be stretched
 * horizontally or vertically to align all the items in the same line. If the
 * item is flexible, it will be stretched to fit the toolbar.
 */
void
WToolbarItem::GetPreferredSize(float *width, float *height)
{
	if (width != NULL)
		*width = -1.0;
	if (height != NULL)
		*height = -1.0;
}

/* Returns the absolute index of the item in the toolbar. Returns -1 if the item
 * isn't attached to a toolbar.
 */
int
WToolbarItem::Index(void)
{
	return (fToolbar == NULL ? -1 : fToolbar->IndexOf(this));
}

/* Tells the toolbar to resize (if required) and redraw the item.
 */
void
WToolbarItem::Invalidate(void)
{
	if (fToolbar == NULL)
		return;

	// If size has changed, we've to tell the toolbar to recalculate the
	// control rects, otherwise just to redraw ourself
	float width, height;

	GetPreferredSize(&width, &height);

	if (width != fWidth || height != fHeight) {
		if (fWidth == 0.0 && fHeight == 0.0) {
			fWidth = width;
			fHeight = height;
			fToolbar->Update();
			fToolbar->DrawItem(this);
		} else {
			fWidth = width;
			fHeight = height;
			fToolbar->Update();
		}
	} else
		fToolbar->DrawItem(this);
}

/* Returns the index of the line the item is on. Returns -1 if the item isn't
 * attached to a toolbar.
 */
int
WToolbarItem::Line(void)
{
	return (fToolbar == NULL ? -1 : fLine);
}

/* Just like BView::MouseDown(), this method is called when the user press the
 * primary mouse button when the cursor is over the item (the secondary
 * button is used by the toolbar). The default implementation does nothing.
 */
void
WToolbarItem::MouseDown(BPoint point)
{
}

/* Just like BView::MouseMoved(), this method is called when the user moves the
 * mouse over the item. The point is in item's coordinates. Transit can be
 * B_ENTERED_VIEW, B_INSIDE_VIEW or B_EXITED_VIEW. message is the actual
 * message that triggered the event. The default implementation does nothing.
 */
void
WToolbarItem::MouseMoved(BPoint point, uint32 transit,
	const BMessage *message)
{
}

/* Just like BView::MouseUp(), this method is called when the user releases the
 * left mouse button over the method. The point is in item's coordinates. The
 * default implementation does nothing.
 */
void
WToolbarItem::MouseUp(BPoint point)
{
}

/* Returns the position of the item in the line. If the item isn't attached to a
 * toolbar, returns -1.
 */
int
WToolbarItem::Position(void)
{
	if (fToolbar == NULL)
		return -1;
	return fToolbar->PositionOf(this);
}

/* Set the item to be (or to be not) flexible. Only the first flexible item in a
 * line will be threated as such.
 */
void
WToolbarItem::SetFlexible(bool flexible)
{
	if (fFlexible == flexible)
		return;
	fFlexible = flexible;
	if (fToolbar != NULL)
		fToolbar->Update();
}	

/* Shows or hides the item.
 */
void
WToolbarItem::SetVisible(bool visible)
{
	if (fVisible == visible)
		return;
	fVisible = visible;
	if (fToolbar != NULL)
		fToolbar->Update();
}

/* Returns the toolbar the item belongs to, or NULL if the item doesn't belong
 * to any toolbar.
 */
WToolbar *
WToolbarItem::Toolbar(void)
{
	return fToolbar;
}

/* This method is called by the toolbar when some of the toolbar properties
 * changes. If your item needs to resize, you should call Invalidate(). If your
 * item just needs to be redrawn, it is safe to call Draw() here.
 * The default implementation does nothing.
 */
void
WToolbarItem::Update(void)
{
}

/* Returns true if the item is visible, false otherwise. Note that the item can
 * be "visible" even if it isn't attached to a toolbar.
 */
bool
WToolbarItem::Visible(void)
{
	return fVisible;
}
