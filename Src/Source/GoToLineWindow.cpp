#include "GoToLineWindow.h"

using namespace InterfaceConstants;

GoToLineWindow::GoToLineWindow(BRect r, BMessenger* messenger)
	: BWindow(r, "Go To Line...", B_FLOATING_WINDOW, B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	SetFeel(B_NORMAL_WINDOW_FEEL);

	parent = new BView(Bounds(), "parent", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	parent->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(parent);

	msgr = messenger;
	r = Bounds();
	num = new BTextControl(BRect(5, 10, 40, 30), "num", NULL, NULL, NULL);
	parent->AddChild(num);
	num->MakeFocus(true);
	go = new BButton(BRect(55, 8, 95, 28), "gogogadgetbutton", "Go", new BMessage(K_GTL_WINDOW_GO));
	parent->AddChild(go);
	go->MakeDefault(true);
}
GoToLineWindow::~GoToLineWindow()
{
	delete msgr;
}
void
GoToLineWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case K_GTL_WINDOW_GO:
		{
			BMessage* linemsg = new BMessage(K_GTL_WINDOW_GO);
			BString text;
			text << num->Text();
			bool IsNumeric = true;
			for (int i = 0; i < text.Length(); i++) {
				if (!isdigit(text[i])) {
					IsNumeric = false;
				}
			}
			if (IsNumeric && linemsg->AddInt32("line", atoi(text.String()) - 1) == B_OK) {
				msgr->SendMessage(linemsg);
				Quit();
			}

		} break;
		default:
			BWindow::MessageReceived(msg);
	}
}
void
GoToLineWindow::Quit()
{
	msgr->SendMessage(new BMessage(K_GTL_WINDOW_QUIT));
	BWindow::Quit();
}
