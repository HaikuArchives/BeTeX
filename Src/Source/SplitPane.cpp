/*******************************************************
 *   SplitPane©
 *
 *   SplitPane is a usefull UI component. It alows the
 *   use to ajust two view Horizontaly or Vertacly so
 *   that they are a desired size. This type of Pane
 *   shows up most comonly in Mail/News Readers.
 *
 *   @author  YNOP (ynop@acm.org)
 *   @version beta
 *   @date    Dec. 10 1999
 *******************************************************/
#include <AppKit.h>
#include <InterfaceKit.h>
#include <Path.h>
#include <StorageKit.h>
#include <String.h>
#include <TranslationKit.h>
#include <TranslationUtils.h>

// #include <stdio.h>

#include "SplitPane.h"
// #include "SplitPaneConfig.h"

/*******************************************************
 *   Setup the main view. Add in all the niffty components
 *   we have made and get things rolling
 *******************************************************/
SplitPane::SplitPane(BRect frame, const char* name, BView* one, BView* two, uint32 Mode)
	: BView(frame, name, Mode, B_WILL_DRAW | B_FRAME_EVENTS)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));  // This is default get from parent if exist
	// SetViewColor(B_TRANSPARENT_32_BIT); // go tran so we have control over drawing
	BRect b;
	b = Bounds();
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	PaneOne = one;
	PaneTwo = two;

	align = B_VERTICAL;		   // Most people use it this way
	pos = (int)b.Width() / 2;  // Center is a good start place
	thickness = 10;
	jump = 1;  // 1 makes a smother slide
	VOneDetachable = false;
	VTwoDetachable = false;
	pad = 3;
	MinSizeOne = 0;		  // full left
	MinSizeTwo = 0;		  // full right
	poslocked = false;	  // free movement
	alignlocked = false;  // free alignment
	Draggin = false;
	attached = false;

	WinOne = NULL;
	WinTwo = NULL;
	ConfigWindow = NULL;

	AddChild(one);
	AddChild(two);
}

/*******************************************************
 *   When ready grap the parents color and refreash.
 *******************************************************/
void
SplitPane::AttachedToWindow()
{
	// SetViewColor(Parent()->ViewColor());
	attached = true;
	Update();
}

/*******************************************************
 *   If we are being resized. Fix the stuff we need to fix
 *******************************************************/
void
SplitPane::FrameResized(float, float)
{
	//   if bar is on the left side follow left
	//   else if it is on the right side follow the right
	//   Need to implements smart follow still
	Update();
	Invalidate();
}

/*******************************************************
 *   The main draw stuff here. basicly just the slider
 *******************************************************/
void
SplitPane::Draw(BRect f)
{
	SetHighColor(160, 160, 160);

	if (align == B_VERTICAL) {
		SetHighColor(145, 145, 145);
		// FillRect(BRect(pos,Bounds().top+pad+1,pos,Bounds().bottom-pad-1)); // 145
		FillRect(BRect(pos, Bounds().top + pad + 1, pos, Bounds().bottom - pad - 1));  // 145

		SetHighColor(255, 255, 255);
		FillRect(
			BRect(pos + 1, Bounds().top + pad + 1, pos + 2, Bounds().bottom - pad - 1));  // 255

		if (Parent())
			SetHighColor(Parent()->ViewColor());
		else
			SetHighColor(216, 216, 216);

		FillRect(BRect(pos + 2, Bounds().top + pad + 1, pos + thickness - 2,
			Bounds().bottom - pad - 1));  // 216

		SetHighColor(145, 145, 145);
		FillRect(BRect(pos + thickness - 2, Bounds().top + pad + 1, pos + thickness - 2,
			Bounds().bottom - pad - 1));  // 145

		SetHighColor(96, 96, 96);
		FillRect(BRect(pos + thickness - 1, Bounds().top + pad + 1, pos + thickness - 1,
			Bounds().bottom - pad - 1));  // 96
		// FillRect(BRect(pos+thickness,Bounds().top+pad+1,pos+thickness,Bounds().bottom-pad-1));
	} else {
		SetHighColor(145, 145, 145);
		// FillRect(BRect(Bounds().left+pad+1,pos,Bounds().right-pad-1,pos)); // 145
		FillRect(BRect(Bounds().left + pad + 1, pos, Bounds().right - pad - 1, pos));  // 145

		SetHighColor(255, 255, 255);
		FillRect(
			BRect(Bounds().left + pad + 1, pos + 1, Bounds().right - pad - 1, pos + 2));  // 255

		// SetHighColor(216,216,216);
		SetHighColor(Parent()->ViewColor());
		FillRect(BRect(Bounds().left + pad + 1, pos + 2, Bounds().right - pad - 1,
			pos + thickness - 2));	// 216

		SetHighColor(145, 145, 145);
		FillRect(BRect(Bounds().left + pad + 1, pos + thickness - 2, Bounds().right - pad - 1,
			pos + thickness - 2));	// 145

		SetHighColor(96, 96, 96);
		FillRect(BRect(Bounds().left + pad + 1, pos + thickness - 1, Bounds().right - pad - 1,
			pos + thickness - 1));	// 96
		// FillRect(BRect(Bounds().left+pad+1,pos+thickness,Bounds().right-pad-1,pos+thickness));
	}
}

/*******************************************************
 *   Keeps Modes for both panles uptodate and acctually
 *   is the func that sets the location of the slider
 *******************************************************/
void
SplitPane::Update()
{
	Window()->Lock();
	if (align == B_VERTICAL) {
		PaneOne->SetResizingMode(B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM);
		PaneTwo->SetResizingMode(B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM);
		if (pos > (Bounds().Width() - thickness - MinSizeTwo)) {
			if (!poslocked) {
				pos = (int)Bounds().Width() - thickness - MinSizeTwo;
			}
		}
		if (pos < MinSizeOne) {
			if (!poslocked) {
				pos = MinSizeOne;
			}
		}
	} else {
		PaneOne->SetResizingMode(B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
		PaneTwo->SetResizingMode(B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM);
		if (pos > (Bounds().Height() - thickness - MinSizeTwo)) {
			if (!poslocked) {
				pos = (int)Bounds().Height() - thickness - MinSizeTwo;
			}
		}
		if (pos < MinSizeOne) {
			if (!poslocked) {
				pos = MinSizeOne;
			}
		}
	}

	// this block should go in FrameResized .. think about it
	if (align == B_VERTICAL) {
		if (pos >= (Bounds().IntegerWidth() / 2)) {
			// pos should follow the right side
			//  staying the same distans from it that
			//  it is right now
		}
	} else {
		if (pos >= (Bounds().IntegerHeight() / 2)) {
			// should follow bottom and stay the
			//  same distance that we are way from
			//  it now
		}
	}


	if (PaneOne) {
		if (!WinOne) {
			if (align == B_VERTICAL) {
				PaneOne->MoveTo(pad, Bounds().top + pad);
				PaneOne->ResizeTo(pos - pad, Bounds().Height() - pad - pad);  // widht x height
			} else {
				PaneOne->MoveTo(pad, Bounds().top + pad);
				PaneOne->ResizeTo(Bounds().Width() - pad - pad, pos - pad - pad);  // widht x height
			}
		}
	}
	if (PaneTwo) {
		if (!WinTwo) {
			if (align == B_VERTICAL) {
				PaneTwo->MoveTo(pos + thickness, Bounds().top + pad);
				PaneTwo->ResizeTo(
					Bounds().Width() - (pos + thickness) - pad, Bounds().Height() - pad - pad);
			} else {
				PaneTwo->MoveTo(Bounds().left + pad, pos + thickness);
				PaneTwo->ResizeTo(
					Bounds().Width() - pad - pad, Bounds().Height() - pos - pad - thickness);
			}
		}
	}

	Window()->Unlock();
}

/*******************************************************
 *   Hook for when we click. This takes care of all the
 *   little stuff - Like where is the mouse and what is
 *   going on.
 *******************************************************/
void
SplitPane::MouseDown(BPoint where)
{
	Window()->Lock();
	BMessage* currentMsg = Window()->CurrentMessage();
	if (currentMsg->what == B_MOUSE_DOWN) {
		uint32 buttons = 0;
		currentMsg->FindInt32("buttons", (int32*)&buttons);
		uint32 modifiers = 0;
		currentMsg->FindInt32("modifiers", (int32*)&modifiers);
		uint32 clicks = 0;
		currentMsg->FindInt32("clicks", (int32*)&clicks);

		if (buttons & B_SECONDARY_MOUSE_BUTTON) {
			if (!alignlocked) {
				switch (align) {
					case B_VERTICAL:
						align = B_HORIZONTAL;
						break;
					case B_HORIZONTAL:
						align = B_VERTICAL;
						break;
				}
				Update();
				Invalidate();
			}

			/*if(VOneDetachable){
			   WinOne = new
			BWindow(ConvertToScreen(PaneOne->Bounds()),"PanelOne",B_TITLED_WINDOW,B_ASYNCHRONOUS_CONTROLS);
			   RemoveChild(PaneOne);
			   WinOne->AddChild(PaneOne);
			   PaneOne->SetResizingMode(B_FOLLOW_ALL_SIDES);
			   // PaneOne->SetTarget(this);
			   WinOne->Show();
			}*/
		}
		// if((buttons & B_PRIMARY_MOUSE_BUTTON) && (clicks >= 2)){
		// Config window for split pane
		//    (new BAlert(NULL,"This is - or will be - a configuration panel for
		//    SplitPane.","Ok"))->Go();
		// ConfigWindow = new SplitPaneConfig(this);
		// ConfigWindow->Show();
		//}else
		if ((buttons & B_PRIMARY_MOUSE_BUTTON) && !Draggin) {
			if (!poslocked) {
				Draggin = true;	 // this is so we can drag
				here = where;
			}
			SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
		}
	}
	Window()->Unlock();
}

/*******************************************************
 *   If we unclick then stop dragging or whatever it is
 *   we are doing
 *******************************************************/
void
SplitPane::MouseUp(BPoint where)
{
	Draggin = false;  // stop following mouse
}

/*******************************************************
 *   If the mouse moves while we dragg. Then follow it
 *   Also Invalidate so we update the views
 *******************************************************/
void
SplitPane::MouseMoved(BPoint where, uint32 info, const BMessage* m)
{
	if (Draggin) {
		switch (align) {
			case B_HORIZONTAL:
				pos = (int)(where.y) - (thickness / 2);	 //- here.x
				break;
			case B_VERTICAL:
				pos = (int)(where.x) - (thickness / 2);
				break;
		}

		/*
		// This code figures out which jump we are closest
		// to and if needed we "snap" to that.
		int c = Bounds().IntegerWidth() / pos
		Jump * c ... hmmm this is not right at all
		*/

		if (pos < MinSizeOne) {
			pos = MinSizeOne;
		}

		if (align == B_VERTICAL) {
			if (pos > (Bounds().Width() - thickness - MinSizeTwo)) {
				pos = (int)(Bounds().Width() - thickness - MinSizeTwo + 1);
			}
		} else {
			if (pos > (Bounds().Height() - thickness - MinSizeTwo)) {
				pos = (int)(Bounds().Height() - thickness - MinSizeTwo + 1);
			}
		}

		Update();

		Invalidate();
	}
}

/*******************************************************
 *   If you already have a view One, but want to change
 *   if for some odd reason. This should work.
 *******************************************************/
void
SplitPane::AddChildOne(BView* v)
{
	RemoveChild(PaneOne);
	PaneOne = v;
	AddChild(PaneOne);
}
/*void SplitPane::MakePaneTwoFocus()
{
	if(PaneTwo)
		PaneTwo->MakeFocus();

}
*/
/*******************************************************
 *   If you already have a view Two, and want to put
 *   another view there, this is what to use.
 *******************************************************/
void
SplitPane::AddChildTwo(BView* v, bool IsAdded, bool ShowAfterHide)
{
	if (!v->IsHidden())
		v->Hide();

	PaneTwo = v;
	// WinTwo = NULL;


	PaneTwo = v;
	if (IsAdded) {
		Update();
		if (ShowAfterHide) {
			if (v->IsHidden())
				v->Show();
		}
	}
	if (!IsAdded) {
		AddChild(PaneTwo);
	}
	PaneTwo = v;
}

/*******************************************************
 *   Sets is we are horizontal or Vertical. We use the
 *   standard B_HORIZONTAL and B_VERTICAL flags for this
 *******************************************************/
void
SplitPane::SetAlignment(uint a)
{
	align = a;
	if (attached) {
		Update();
	}
	Invalidate();
}

/*******************************************************
 *   Returns wheather the slider is horizontal or vertical
 *******************************************************/
uint
SplitPane::GetAlignment()
{
	return align;
}

/*******************************************************
 *   Sets the location of the bar. (we do no bounds
 *   checking for you so if its off the window thats
 *   your problem)
 *******************************************************/
void
SplitPane::SetBarPosition(int i)
{
	pos = i;
	if (attached) {
		Update();
	}
	Invalidate();
}

/*******************************************************
 *   Returns about where the bar is ...
 *******************************************************/
int
SplitPane::GetBarPosition()
{
	return pos;
}

/*******************************************************
 *   Sets how thick the bar should be.
 *******************************************************/
void
SplitPane::SetBarThickness(int i)
{
	thickness = i;
	if (attached) {
		Update();
	}
	Invalidate();
}

/*******************************************************
 *   Retuns to us the thickness of the slider bar
 *******************************************************/
int
SplitPane::GetBarThickness()
{
	return thickness;
}

/*******************************************************
 *   Sets the amount of jump the bar has when it is
 *   moved. This can also be though of as snap. The bar
 *   will start at 0 and jump(snap) to everry J pixels.
 *******************************************************/
void
SplitPane::SetJump(int i)
{
	jump = i;
	if (attached) {
		Update();
	}
}

/*******************************************************
 *   Lets you know what the jump is .. see SetJump
 *******************************************************/
int
SplitPane::GetJump()
{
	return jump;
}

/*******************************************************
 *   Do we have a View One or is it NULL
 *******************************************************/
bool
SplitPane::HasViewOne()
{
	if (PaneOne)
		return true;
	return false;
}

/*******************************************************
 *   Do we have a View Two .. or is it NULL too
 *******************************************************/
bool
SplitPane::HasViewTwo()
{
	if (PaneTwo)
		return true;
	return false;
}

/*******************************************************
 *   Sets wheather View one is detachable from the
 *   slider view and from the app. This will creat a
 *   window that is detached (floating) from the app.
 *******************************************************/
void
SplitPane::SetViewOneDetachable(bool b)
{
	VOneDetachable = b;
}

/*******************************************************
 *   Sets view tow detachable or not
 *******************************************************/
void
SplitPane::SetViewTwoDetachable(bool b)
{
	VTwoDetachable = b;
}

/*******************************************************
 *   Returns whether the view is detachable
 *******************************************************/
bool
SplitPane::IsViewOneDetachable()
{
	return VOneDetachable;
}

/*******************************************************
 *   Returns if this view is detachable
 *******************************************************/
bool
SplitPane::IsViewTwoDetachable()
{
	return VTwoDetachable;
}

/*******************************************************
 *   Tells the view if the user is alowed to open the
 *   configuration window for the slider.
 *******************************************************/
void
SplitPane::SetEditable(bool b)
{
	// ADD CODE HERE YNOP
}

/*******************************************************
 *   Tells use if the split pane is user editable
 *******************************************************/
bool
SplitPane::IsEditable()
{
	return true;  // ADD SOME MORE CODE HERE
}

/*******************************************************
 *   Sets the inset that the view has.
 *******************************************************/
void
SplitPane::SetViewInsetBy(int i)
{
	pad = i;
	if (attached) {
		Update();
	}
	Invalidate();
}

/*******************************************************
 *   Returns to use the padding around the views
 *******************************************************/
int
SplitPane::GetViewInsetBy()
{
	return pad;
}

/*******************************************************
 *   This sets the minimum size that View one can be.
 *   if the user trys to go past this .. we just stop
 *   By default the minimum size is set to 0 (zero) so
 *   the user can put the slider anywhere.
 *******************************************************/
void
SplitPane::SetMinSizeOne(int i)
{
	MinSizeOne = i;
}

/*******************************************************
 *   Gives us the minimum size that one can be.
 *******************************************************/
int
SplitPane::GetMinSizeOne()
{
	return MinSizeOne;
}

/*******************************************************
 *   This sets the Minimum size that the second view
 *   can be.
 *******************************************************/
void
SplitPane::SetMinSizeTwo(int i)
{
	MinSizeTwo = i;
}

/*******************************************************
 *   Lets us know what that minimum size is.
 *******************************************************/
int
SplitPane::GetMinSizeTwo()
{
	return MinSizeTwo;
}

/*******************************************************
 *   Locks the bar from being moved by the User. The
 *   system can still move the bar (via SetBarPosition)
 *******************************************************/
void
SplitPane::SetBarLocked(bool b)
{
	poslocked = b;
}

/*******************************************************
 *   Returns to use if the bar is in a locked state or
 *   not.
 *******************************************************/
bool
SplitPane::IsBarLocked()
{
	return poslocked;
}

/*******************************************************
 *   Locks the alignment of the bar. The user can no
 *   longer toggle between Horizontal and Vertical
 *   Slider bar. Again you can still progomaticly set
 *   the position how ever you want.
 *******************************************************/
void
SplitPane::SetBarAlignmentLocked(bool b)
{
	alignlocked = b;
}

/*******************************************************
 *   Lets us know about the lock state of the bar
 *******************************************************/
bool
SplitPane::IsBarAlignmentLocked()
{
	return alignlocked;
}

/*******************************************************
 *   Gets the Total state of the bar, alignment, size,
 *   position and many other things that are required
 *   to fully capture the state of the SplitPane.
 *   We pack all of this into a cute little BMessage
 *   so that it is esally expandable and can be saved
 *   off easyaly too.  The SplitPane System does not
 *   however save the state for you. Your program must
 *   grab the state and save it in its config file.
 *******************************************************/
BMessage*
SplitPane::GetState()
{
	BMessage* state;

	state = new BMessage(SPLITPANE_STATE);

	state->AddBool("onedetachable", VOneDetachable);
	state->AddBool("twodetachable", VTwoDetachable);
	state->AddInt32("align", align);
	state->AddInt32("pos", pos);
	state->AddInt32("thick", thickness);
	state->AddInt32("jump", jump);
	state->AddInt32("pad", pad);
	state->AddInt32("minsizeone", MinSizeOne);
	state->AddInt32("minsizetwo", MinSizeTwo);
	state->AddBool("poslock", poslocked);
	state->AddBool("alignlock", alignlocked);
	return state;
	// delete state;
}

/*******************************************************
 *   Sets the state of the SplitPane from a BMessage
 *   like the one recived from GetState().
 *   This is one of three ways the user can rebuild the
 *   state of the SplitPane. The second is to simply
 *   send the SplitPane the state message, it is the
 *   same as calling SetState but it ashyncronouse.
 *   The third way is to use all the Get/Set methouds
 *   for each element of the SplitPane, this way is
 *   long and boarding. I suggest you just send the
 *   View a message :)
 *******************************************************/
void
SplitPane::SetState(BMessage* state)
{
	int32 Npos, Nthickness, Njump, Npad, NMSO, NMST;
	int32 Nalign;  // uint

	if (state->FindBool("onedetachable", &VOneDetachable) != B_OK) {
		VOneDetachable = false;
	}
	if (state->FindBool("towdetachable", &VTwoDetachable) != B_OK) {
		VTwoDetachable = false;
	}
	if (state->FindInt32("align", &Nalign) == B_OK) {
		align = Nalign;
	}
	if (state->FindInt32("pos", &Npos) == B_OK) {
		pos = Npos;
	}
	if (state->FindInt32("thick", &Nthickness) == B_OK) {
		thickness = Nthickness;
	}
	if (state->FindInt32("jump", &Njump) == B_OK) {
		jump = Njump;
	}
	if (state->FindInt32("pad", &Npad) == B_OK) {
		pad = Npad;
	}
	if (state->FindInt32("minsizeonw", &NMSO) == B_OK) {
		MinSizeOne = NMSO;
	}
	if (state->FindInt32("minsizetwo", &NMST) == B_OK) {
		MinSizeTwo = NMST;
	}
	if (state->FindBool("poslock", &poslocked) != B_OK) {
		poslocked = false;
	}
	if (state->FindBool("alignlock", &alignlocked) != B_OK) {
		alignlocked = false;
	}

	Update();
	Invalidate();
}

/*******************************************************
 *   Ok, hmm what does this do. NOT MUCH. if we get a
 *   STATE message then lets set the state. This is here
 *   to provide a asyncronuse way of seting the state and
 *   also to make life easyer.
 *******************************************************/
void
SplitPane::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case SPLITPANE_STATE:
			SetState(msg);
			break;
		default:
			BView::MessageReceived(msg);
			break;
	}
}
