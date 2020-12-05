// =============================================================================
//
// libwalter ToolbarControl.cpp
//
// Base class for WToolbar controls
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
 * \class WToolbarControl
 * \brief Base class for toolbar controls
 *
 * WToolbarControl provides all the basic facilities to create an archivable
 * and invokable toolbar control. It is a descendant of WToolbarItem and
 * BInvoker, in a similar fashion of BView and BControl.
 *
 * Currently libwalter supplies one control: WToolbarButton.
 *
 * \par How to create your own control
 * Creating your own control by overloading WToolbarControl is not very
 * different than overloading BControl. The most obvious methods you have to
 * overload are Draw() and GetPreferredSize(). If your control needs to send a
 * message after user's action, you may also want to overload MouseUp().
 */

// libwalter headers
#include "Toolbar.h"
#include "ToolbarControl.h"

// =============================================================================
// WToolbarControl
// =============================================================================

// Constructors and destructors

/* Creates a new control. name is the name of the item (can be NULL), it is not
 * visible to the user. message is the message is the message that is sent to
 * the target when the control is invoked (can be NULL).
 * A newly constructed control is enabled by default.
 */
WToolbarControl::WToolbarControl(const char *name, BMessage *message) :
	WToolbarItem(name),
	BInvoker(message, NULL)
{
	_init_object();
}

WToolbarControl::WToolbarControl(BMessage *archive) :
	WToolbarItem(archive),
	BInvoker(NULL, NULL)
{
	bool enabled, visible;
	BMessage message;
	int line;

	_init_object();

	if (archive->FindMessage("WToolbarControl::message", &message) == B_OK)
		SetMessage(new BMessage(message));

	if (archive->FindBool("WToolbarControl::enabled", &enabled) == B_OK)
		SetEnabled(enabled);
}

WToolbarControl::~WToolbarControl()
{
}

// Private

void WToolbarControl::_init_object(void)
{
	fEnabled = true;
}

// BArchivable hooks

/* Archive the control to the target message. Adds the following fields to the
 * supplied message, if the values of the respective properties is different
 * from the defaul:
 *  WToolbarControl::message	message
 *  WToolbarControl::enabled	bool
 */
status_t WToolbarControl::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	// Ancestor
	status = WToolbarItem::Archive(archive, deep);

	// Properties

	if (status == B_OK && Message() != NULL)
		status = archive->AddMessage("WToolbarControl::message", Message());

	if (status == B_OK && !fEnabled)
		status = archive->AddBool("WToolbarControl::enabled", fEnabled);

	return status;
}

BArchivable * WToolbarControl::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "WToolbarControl") ?
		new WToolbarControl(archive) : NULL);
}

// Public

/* If the control doesn't have a target yet, set the target as the toolbar's
 * target.
 */
void WToolbarControl::AttachedToToolbar(void)
{
	if (!(Messenger().IsValid()))
		SetTarget(Toolbar()->Messenger());
	WToolbarItem::AttachedToToolbar();
}

/* The default implementation does nothing.
 */
void WToolbarControl::DetachedFromToolbar(void)
{
	WToolbarItem::DetachedFromToolbar();
}

/* Draws the control on the given canvas (the toolbar itself, or an off-screen
 * buffer). The default implementation does nothing.
 */
void WToolbarControl::Draw(BView *canvas, BRect updateRect)
{
}

/* Return true if the control is enabled, false otherwise.
 */
bool WToolbarControl::Enabled(void)
{
	return fEnabled;
}

/* Returns the preferred size of the control. If you aren't interested in one of
 * the values, you can pass NULL.
 * The default implementation returns -1 for both values.
 */
void WToolbarControl::GetPreferredSize(float *width, float *height)
{
	if (width != NULL) *width = -1.0;
	if (height != NULL) *height = -1.0;
}

/* Just like BView::MouseDown(), this method is called when the user press the
 * primary mouse button when the cursor is over the item (the secondary
 * button is used by the toolbar). The default implementation does nothing.
 */
void WToolbarControl::MouseDown(BPoint point)
{
	WToolbarItem::MouseDown(point);
}

/* Just like BView::MouseMoved(), this method is called when the user moves the
 * mouse over the item. The point is in item's coordinates. Transit can be
 * B_ENTERED_VIEW, B_INSIDE_VIEW or B_EXITED_VIEW. message is the actual
 * message that triggered the event. The default implementation does nothing.
 */
void WToolbarControl::MouseMoved(BPoint point, uint32 transit,
	const BMessage *message)
{
	WToolbarItem::MouseMoved(point, transit, message);
}

/* Just like BView::MouseUp(), this method is called when the user releases the
 * left mouse button over the method. The point is in item's coordinates. The
 * default implementation does nothing.
 */
void WToolbarControl::MouseUp(BPoint point)
{
	WToolbarItem::MouseUp(point);
}

/* Set if the item is enabled or not. What exactly means to be enabled depends
 * from the actual control. The control is then invalidated.
 */
void WToolbarControl::SetEnabled(bool enabled)
{
	if (fEnabled == enabled) return;
	fEnabled = enabled;
	Invalidate();
}

/* This method is called by the toolbar when some of the toolbar properties
 * changes. If your item needs to resize, you should call Invalidte(). If your
 * item just needs to be redrawn, it is safe to call Draw() here.
 * The default implementation does nothing.
 */
void WToolbarControl::Update(void)
{
	WToolbarItem::Update();
}
