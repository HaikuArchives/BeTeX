#ifndef _GOTOLINE_WINDOW_H_
#define _GOTOLINE_WINDOW_H_

#include <Window.h>
#include <View.h>
#include <Button.h>
#include <iostream>
#include <Messenger.h>
#include "Constants.h"
#include <TextControl.h>
#include <Font.h>
#include <cctype>
#include <String.h>
#include <string>
class GoToLineWindow : public BWindow
{
	public:
		GoToLineWindow(BRect r,BMessenger* messenger);
		~GoToLineWindow();
		virtual void MessageReceived(BMessage* msg);
		void Quit();
	private:
		BView* parent;
		BMessenger* msgr;
		BButton* go;
		BTextControl* num;
};
#endif

