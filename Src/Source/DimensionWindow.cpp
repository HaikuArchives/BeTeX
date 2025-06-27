/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef DIMENSION_WINDOW_H
#include "DimensionWindow.h"
#endif

#include <be/interface/Button.h>
#include <be/interface/View.h>
#include <ctype.h>
#include <stdlib.h>
#include "Constants.h"
#include "MessageFields.h"

DimensionWindow::DimensionWindow(BRect frame, BMessenger* messenger, BString standardWidth,
	BString standardHeight, uint32 return_msg)
	: BWindow(frame, "Dimensions", B_FLOATING_WINDOW,
		  B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_NOT_CLOSABLE),
	  m_returnMessage(return_msg),
	  m_ownerMsgr(messenger)
{
	SetFeel(B_NORMAL_WINDOW_FEEL);

	BView* parent = new BView(Bounds(), "parent", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	parent->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(parent);

	float buttonWidth = 50.0f, buttonHeight = 20.0f, buttonPad = 10.0f;

	BRect buttonRect(Bounds().left + (2.0f * buttonPad) - 5.0f,
		Bounds().bottom - (2.0f * buttonPad) - buttonHeight,
		Bounds().left + (2 * buttonPad) + buttonWidth - 5, Bounds().bottom - (2.0f * buttonPad));
	BButton* okButton
		= new BButton(buttonRect, "Ok", "Ok", new BMessage(InterfaceConstants::K_DIM_WINDOW_OK));
	parent->AddChild(okButton);
	okButton->MakeDefault(true);

	buttonRect.OffsetBy((50.0f + (2.0f * buttonPad)), 0.0f);
	BButton* cancelButton = new BButton(
		buttonRect, "Cancel", "Cancel", new BMessage(InterfaceConstants::K_DIM_WINDOW_CANCEL));
	parent->AddChild(cancelButton);

	BRect tcRect(buttonPad, buttonPad, buttonPad + 100.0f, buttonPad + 20.0f);
	m_rowsText = new BTextControl(tcRect, "rowsTC", "Rows", standardWidth.String(), NULL);
	parent->AddChild(m_rowsText);

	tcRect.OffsetBy(0, 30);
	m_colsText = new BTextControl(tcRect, "colsTC", "Columns", standardHeight.String(), NULL);
	parent->AddChild(m_colsText);
}

DimensionWindow::~DimensionWindow()
{
	delete m_ownerMsgr;
}

void
DimensionWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case InterfaceConstants::K_DIM_WINDOW_OK:
		{
			BMessage msg(m_returnMessage);
			// mmm?
			int max = 20;
			int min = 2;
			// get the number of rows entered
			BString numRows = m_rowsText->Text();
			// check whether string contains digits only!
			bool isStrBad = false;
			for (int i = 0; i < numRows.Length(); i++) {
				if (!isdigit(numRows[i])) {
					isStrBad = true;
					break;
				}
			}

			if (!isStrBad) {
				// string only contains digits, check if number of rows within valid boundaries
				if (atoi(numRows.String()) > max || atoi(numRows.String()) < min)
					numRows = "4";
			} else {
				// string also contained alphabetic characters, set numRows to default value
				numRows = "4";
			}
			msg.AddString(K_ROWS, numRows);

			// get the number of columns entered
			BString numCols = m_colsText->Text();
			isStrBad = false;
			for (int i = 0; i < numCols.Length(); i++) {
				if (!isdigit(numCols[i])) {
					isStrBad = true;
					break;
				}
			}

			if (!isStrBad) {
				if (atoi(numCols.String()) > max || atoi(numCols.String()) < min)
					numCols = "4";
			} else {
				numCols = "4";
			}
			msg.AddString(K_COLS, numCols);

			m_ownerMsgr->SendMessage(&msg);
			Quit();
		} break;
		case InterfaceConstants::K_DIM_WINDOW_CANCEL:
		{
			Quit();
		} break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

void
DimensionWindow::Quit()
{
	m_ownerMsgr->SendMessage(new BMessage(InterfaceConstants::K_DIM_WINDOW_QUIT));
	BWindow::Quit();
}
