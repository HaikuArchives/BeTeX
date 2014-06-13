#ifndef PREFS_WINDOW_H
#define PREFS_WINDOW_H

#include <be/app/Messenger.h>
#include <be/interface/Rect.h>
#include <be/interface/View.h>
#include <be/interface/Window.h>
#include <be/interface/Bitmap.h>
#include <be/interface/Button.h>
#include <be/interface/TextControl.h>
#include <be/interface/CheckBox.h>
#include <be/interface/StringView.h>
#include <be/interface/Slider.h>
#include "ColourButton.h"
#include "PrefsListView.h"

class BitmapView : public BView
{
	public:
		BitmapView(BRect frame, BBitmap *bitmap);
		virtual				~BitmapView();
		
		virtual void		Draw(BRect updateRect);
			
	private:
		BBitmap				*m_bitmap;
};

class PrefsWindow : public BWindow
{
	public:
		PrefsWindow(BRect frame, BMessenger *messenger);
		virtual				~PrefsWindow();
		
		virtual void		MessageReceived(BMessage* msg);
		void				Quit();
		
	private:	
		BView				*constructGeneralBox(BRect frame);
		BView				*constructCommandBox(BRect frame);
		BView				*constructColourBox(BRect frame);
		BView				*constructToolbarBox(BRect frame);
		void				ResetToDefaults(uint32 resetFlag);
		BMessage*			GetPrefsMessage(BString prefsID);
		
	private:
		BMessenger			*m_msgr;		
		BView				*m_parent;						
		
		bool				m_colourPrefsChanged,
							m_fontSizeChanged,
							m_toolbarChanged
							;
};

#endif

