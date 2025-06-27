/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef BETEX_H
#include "BeTeX.h"
#endif

#include <be/interface/Bitmap.h>
#include <be/storage/FindDirectory.h>
#include <be/storage/Mime.h>
#include <be/support/String.h>
#include "Constants.h"
#include "MessageFields.h"
#include "Preferences.h"
#include "TeXDocIcons.h"

BeTeX::BeTeX()
	: BApplication(APP_SIG)
{
	// construct user settings dir
	find_directory(B_USER_SETTINGS_DIRECTORY, &m_prefsPath);
	m_prefsPath.Append("betex.settings");
	// load app's preferences
	BMessage preferences_archive;
	status_t result = LoadPreferences(m_prefsPath.Path(), &preferences_archive);
	if (result == B_OK) {
		prefsLock.Lock();
		preferences = (Prefs*)Prefs::Instantiate(&preferences_archive);
		prefsLock.Unlock();
	}
	BMimeType mime(TEX_FILETYPE);
	BMessage ex_msg;
	bool install = true;
	mime.GetFileExtensions(&ex_msg);
	const char* ext;
	if (ex_msg.FindString("extensions", &ext) == B_OK) {
		install = (strcmp(ext, "tex") != 0);
	}

	if (mime.InitCheck() == B_OK && install)  // && !mime.IsInstalled())
	{
		BMessage msg;
		if (msg.AddString("extensions", "tex") == B_OK) {
			mime.SetFileExtensions(&msg);
		}

		BBitmap* large_icon = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
		large_icon->SetBits(TeXIcon, 3072, 0, B_CMAP8);
		mime.SetIcon(large_icon, B_LARGE_ICON);

		BBitmap* small_icon = new BBitmap(BRect(0, 0, 15, 15), B_CMAP8);
		small_icon->SetBits(SmallTeXIcon, 768, 0, B_CMAP8);
		mime.SetIcon(small_icon, B_MINI_ICON);

		mime.SetLongDescription("TeX/LaTeX Document");
		mime.SetShortDescription("TeX");

		mime.SetPreferredApp(APP_SIG);

		// Create some attributes......"
		BMessage attr;
		attr.AddString("attr:name", "Author");
		attr.AddString("attr:public_name", "Author");
		attr.AddInt32("attr:type", B_STRING_TYPE);
		attr.AddBool("attr:public", true);
		attr.AddBool("attr:editable", true);

		// Want more attributes for:
		//  1) Document status (draft, incomplete, complete) etc
		//  2) Document Priority (low, medium, high)
		//  3) Something else?

		/*attr.AddString("attr:name","Author");
		attr.AddString("attr:public_name","Author");
		attr.AddInt32("attr:type",B_STRING_TYPE);
		attr.AddBool("attr:public",true);
		attr.AddBool("attr:editable",true);
		*/
		mime.Install();
	}
	// I could also add some attributes to the tex file
	// such as author, etc.....

	m_mainWindow = new MainWindow(preferences->main_window_rect);
	m_mainWindow->Show();
}

BeTeX::~BeTeX() {}

void
BeTeX::RefsReceived(BMessage* message)
{
	m_mainWindow->PostMessage(message);
	BApplication::RefsReceived(message);
}

bool
BeTeX::QuitRequested()
{
	// TODO : at this point splitPane may have gone
	BMessage preferences_archive;
	preferences->Archive(&preferences_archive);

	if (m_mainWindow->PromptToQuit()) {
		BString text;
		text << "Are you sure want to quit?";

		BAlert* alert = new BAlert("savealert", text.String(), "Cancel", "Force Quit", NULL,
			B_WIDTH_AS_USUAL, B_WARNING_ALERT);
		alert->SetShortcut(0, B_ESCAPE);

		int32 button_index = alert->Go();
		switch (button_index) {
			case 0:
			{
				return false;
			}
			case 1:
			{
				SavePreferences(&preferences_archive, m_prefsPath.Path());
				return true;
			}
		}
		delete alert;
	}
	SavePreferences(&preferences_archive, m_prefsPath.Path());
	return true;
}

void
BeTeX::AboutRequested()
{
	m_mainWindow->PostMessage(new BMessage(AboutMessages::K_ABOUT_WINDOW_LAUNCH));
}

int
main()
{
	BeTeX app;
	app.Run();
	return 0;
}
