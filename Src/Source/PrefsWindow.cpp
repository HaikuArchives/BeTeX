/*******************************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski                             *
 *                                                                             *
 * All rights reserved.                                                        *
 * Distributed under the terms of the MIT License.                             *
 ******************************************************************************/
#ifndef PREFS_WINDOW_H
#include "PrefsWindow.h"
#endif

#include <be/interface/Alert.h>
#include <be/interface/Box.h>
#include <be/interface/Font.h>
#include <be/interface/Screen.h>
#include <be/interface/ScrollView.h>
#include <be/support/ClassInfo.h>
#include <be/support/TypeConstants.h>
#include "Constants.h"
#include "ConverterIcons.h"
#include "MainWindow.h"
#include "MessageFields.h"
#include "Preferences.h"
#include "PrefsListItem.h"
#include "TexBar.h"
#include "TexView.h"

//======================BitmapView============================================
BitmapView::BitmapView(BRect frame, BBitmap* bitmap)
	: BView(frame, "bitmapview", B_FOLLOW_NONE, B_WILL_DRAW),
	  m_bitmap(bitmap)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

BitmapView::~BitmapView()
{
	delete m_bitmap;
}

void
BitmapView::Draw(BRect updateRect)
{
	SetDrawingMode(B_OP_ALPHA);
	DrawBitmap(m_bitmap);
}

//===========================PrefsWindow======================================
PrefsWindow::PrefsWindow(BRect frame, BMessenger* messenger, BView* texBar)
	: BWindow(frame, "Preferences", B_FLOATING_WINDOW,
		  B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_ASYNCHRONOUS_CONTROLS),
	  m_msgr(messenger),
	  m_texBar(texBar),
	  m_colorPrefsChanged(false),
	  m_fontSizeChanged(false)
{
	prefsLock.Lock();
	preferences->Archive(&initialPreferences);
	prefsLock.Unlock();
	SetFeel(B_NORMAL_WINDOW_FEEL);

	m_parent = new BView(Bounds(), "parent", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	m_parent->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(m_parent);

	BFont font(be_fixed_font);
	font.SetSize(preferences->FontSize);

	float prefsListWidth = 80.0;
	BRect prefsListFrame(10.0, 10.0, 10.0 + prefsListWidth, Bounds().bottom - 10.0);
	PrefsListView* prefsListView
		= new PrefsListView(prefsListFrame, m_parent, new BMessenger(this));
	BScrollView* prefsListScroll = new BScrollView(
		"ScrollView", prefsListView, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW, false, false);
	m_parent->AddChild(prefsListScroll);

	float bwidth = 80.0, bheight = 30.0, bspacing = 10.0;

	BRect okButtonFrame(Bounds().right - bspacing - bwidth, Bounds().bottom - bspacing - bheight,
		Bounds().right - bspacing, Bounds().bottom - bspacing);
	BButton* okButton = new BButton(
		okButtonFrame, "okButton", "Ok", new BMessage(PrefsConstants::K_PREFS_VIEW_OK));
	m_parent->AddChild(okButton);
	okButton->MakeDefault(true);

	BRect cancelButtonFrame = okButtonFrame;
	cancelButtonFrame.OffsetBy(-(bwidth + 2.0 * bspacing), 0.0);
	BButton* cancelButton = new BButton(cancelButtonFrame, "cancelButton", "Cancel",
		new BMessage(PrefsConstants::K_PREFS_VIEW_CANCEL));
	m_parent->AddChild(cancelButton);

	float respiteWidth = 41.0;
	BRect boxRect(10.0 + prefsListWidth + 10.0, 2.0, Bounds().right - 7.0,
		Bounds().bottom - 10.0 - respiteWidth);
	prefsListView->AddItem(new PrefsListItem(constructGeneralBox(boxRect), m_parent, "General"));
	prefsListView->AddItem(new PrefsListItem(constructCommandBox(boxRect), m_parent, "Commands"));
	prefsListView->AddItem(new PrefsListItem(constructColorBox(boxRect), m_parent, "TexView"));
	prefsListView->AddItem(new PrefsListItem(constructToolbarBox(boxRect), m_parent, "Toolbars"));
	prefsListView->Select(0);
	prefsListView->MakeFocus(true);
}

PrefsWindow::~PrefsWindow()
{
	delete m_msgr;
}

void
PrefsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case PrefsConstants::K_PREFS_VIEW_OK:
		{
			string commands[] = {K_LATEX_CMD, K_DVIPDF_CMD, K_DVIPS_CMD, K_PS2PDF_CMD,
				K_PDFLATEX_CMD, K_LATEX2HTML_CMD, K_POSTSCRIPT_CMD};
			prefsLock.Lock();
			for (int i = 0; i < 7; i++) {
				BTextControl* tc = (BTextControl*)FindView(commands[i].c_str());
				preferences->commands[commands[i]] = tc->Text();
			}
			prefsLock.Unlock();
			Quit();
		} break;
		case PrefsConstants::K_PREFS_VIEW_CANCEL:
		{
			// restore preferences
			prefsLock.Lock();
			preferences = (Prefs*)Prefs::Instantiate(&initialPreferences);
			// broadcast preferences
			BMessage notice(PrefsConstants::K_PREFS_UPDATE);
			SendNotices(PrefsConstants::K_PREFS_UPDATE, &notice);
			prefsLock.Unlock();
			Quit();
		} break;
		case PrefsConstants::K_PREFS_VIEW_RESET_COLOR_DEFAULTS:
		{
			ResetToDefaults(PrefsConstants::K_RESET_TEXVIEW_PREFS);
			BMessage msg = BMessage(PrefsConstants::K_PREFS_UPDATE);
			MessageReceived(&msg);
		} break;
		case PrefsConstants::K_PREFS_VIEW_RESET_COMMAND_DEFAULTS:
		{
			ResetToDefaults(PrefsConstants::K_RESET_COMMAND_PREFS);
			BMessage msg = BMessage(PrefsConstants::K_PREFS_UPDATE);
			MessageReceived(&msg);
		} break;
		case PrefsConstants::K_PREFS_VIEW_RESET_TOOLBAR_DEFAULTS:
		{
			ResetToDefaults(PrefsConstants::K_RESET_TOOLBAR_PREFS);
			BMessage msg = BMessage(PrefsConstants::K_PREFS_UPDATE);
			MessageReceived(&msg);
		} break;
		case PrefsConstants::K_PREFS_VIEW_RESET_GENERAL_DEFAULTS:
		{
			ResetToDefaults(PrefsConstants::K_RESET_GENERAL_PREFS);
			BMessage msg = BMessage(PrefsConstants::K_PREFS_UPDATE);
			MessageReceived(&msg);
		} break;
		case PrefsConstants::K_PREFS_UPDATE:
		{
			// update the preferences message, from view values
			BString prefsID;
			BMessage notice(PrefsConstants::K_PREFS_UPDATE);
			if (message->FindString(K_PREFS_ID, &prefsID) == B_OK) {
				BView* changedView = m_parent->FindView(prefsID.String());
				prefsLock.Lock();

				// temporary BMessage reflecting the preferences
				BMessage preferences_archive;
				preferences->Archive(&preferences_archive);

				// different view have different kinds of values
				if (is_instance_of(changedView, BTextControl)) {
					// a textcontrol value was changed, update preferences with new text
					BTextControl* textControl = dynamic_cast<BTextControl*>(changedView);
					preferences_archive.ReplaceString(prefsID.String(), textControl->Text());
				} else if (is_instance_of(changedView, BCheckBox)) {
					// a checkbox value was changed, update preferences with new bool value(on/off)
					BCheckBox* checkBox = dynamic_cast<BCheckBox*>(changedView);

					// Inverted logic for those
					bool val;
					if (prefsID == K_IS_GREEK_ABSENT || prefsID == K_IS_BIG_ABSENT
						|| prefsID == K_IS_BINARY_ABSENT || prefsID == K_IS_MISC_ABSENT
						|| prefsID == K_IS_BIN_REL_ABSENT || prefsID == K_IS_MMA_ABSENT
						|| prefsID == K_IS_INT_ABSENT || prefsID == K_IS_TYPE_FACE_ABSENT
						|| prefsID == K_IS_FUNCTION_ABSENT) {
						val = !checkBox->Value();
					} else {
						val = checkBox->Value();
					}

					preferences_archive.ReplaceBool(prefsID.String(), val);

				} else if (is_instance_of(changedView, BSlider)) {
					// a slider value was changed, update preferences with new slider value
					BSlider* slider = dynamic_cast<BSlider*>(changedView);
					preferences_archive.ReplaceInt32(prefsID.String(), slider->Value());
				} else if (is_instance_of(changedView, ColorButton)) {
					// a colorcontrol value was changed, update preferences with new color
					ColorButton* colorControl = dynamic_cast<ColorButton*>(changedView);
					rgb_color new_color = colorControl->Value();
					preferences_archive.ReplaceData(
						prefsID.String(), B_RGB_COLOR_TYPE, &new_color, sizeof(rgb_color));
				}
				// Don't leak old preferences
				delete preferences;
				// Reinstatiate
				preferences = (Prefs*)Prefs::Instantiate(&preferences_archive);
				prefsLock.Unlock();
			}
			SendNotices(PrefsConstants::K_PREFS_UPDATE, &notice);
		} break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

void
PrefsWindow::Quit()
{
	m_msgr->SendMessage(new BMessage(PrefsConstants::K_PREFS_WINDOW_QUIT));
	BWindow::Quit();
}

BView*
PrefsWindow::constructGeneralBox(BRect frame)
{
	BBox* generalBox = new BBox(frame);
	generalBox->SetLabel("General");

	float offset = 30.0, bwidth = 80.0, bheight = 30.0, bspacing = 10.0, cbheight = 14.0,
		  adjust = 5.0;

	float controlX = (offset * 2.0 / 3.0);
	BRect bubbleFrame
		= BRect(controlX, offset - adjust, offset + 150.0, offset + cbheight - adjust);

	ColorButton* bubbleColorButton = new ColorButton(bubbleFrame, K_BUBBLE_COLOR,
		GetPrefsMessage(K_BUBBLE_COLOR), preferences->bubble_color, "Tooltips");
	generalBox->AddChild(bubbleColorButton);

	BRect recentDocsPathFrame(controlX, 50.0, controlX + 160.0, 70.0);
	BCheckBox* showRecentDocsPath = new BCheckBox(recentDocsPathFrame, K_IS_RECENT_DOCS_SHOWN,
		"Show Path in Recent Docs", GetPrefsMessage(K_IS_RECENT_DOCS_SHOWN));
	showRecentDocsPath->SetValue(preferences->IsRecentDocsPathShown);
	generalBox->AddChild(showRecentDocsPath);

	BRect isActivationFrame(controlX, 80.0, controlX + 210.0, 100.0);
	BCheckBox* isActivationOk = new BCheckBox(isActivationFrame, K_IS_ACTIVATION_OK,
		"Activate Window on Opening Docs", GetPrefsMessage(K_IS_ACTIVATION_OK));
	isActivationOk->SetValue(preferences->IsActivationOk);
	generalBox->AddChild(isActivationOk);

	BRect recentDocsFrame(controlX, 110.0, controlX + 160.0, 120.0);
	BTextControl* numRecentDocs = new BTextControl(recentDocsFrame, K_NUM_RECENT_DOCS,
		"Number of Recent Docs", "", GetPrefsMessage(K_NUM_RECENT_DOCS));
	numRecentDocs->SetDivider(130.0);
	numRecentDocs->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	numRecentDocs->SetText(preferences->NumRecentDocs);
	generalBox->AddChild(numRecentDocs);

	BRect insertDateFrame(controlX, 140.0, controlX + 200.0, 150.0);
	BTextControl* insertDateFlags = new BTextControl(
		insertDateFrame, K_DATE_FLAGS, "Insert Date Flags", "", GetPrefsMessage(K_DATE_FLAGS));
	insertDateFlags->SetText(preferences->dateFlags);
	generalBox->AddChild(insertDateFlags);

	BRect generalRect = generalBox->Bounds();
	BRect generalbtnRect(generalRect.right - bspacing - bwidth,
		generalRect.bottom - bspacing - bheight, generalRect.right - bspacing,
		generalRect.bottom - bspacing);
	BButton* resetGeneralDefaults = new BButton(generalbtnRect, "fResetGeneralDefaults", "Defaults",
		new BMessage(PrefsConstants::K_PREFS_VIEW_RESET_GENERAL_DEFAULTS));
	generalBox->AddChild(resetGeneralDefaults);

	return generalBox;
}

BView*
PrefsWindow::constructCommandBox(BRect frame)
{
	BBox* commandBox = new BBox(frame);
	commandBox->SetLabel("Commands");

	float offset = 30.0, bwidth = 80.0, bheight = 30.0, bspacing = 10.0, globalTcOffset = 45.0,
		  bitoffset = 45.0, secOffset = 170.0;

	BRect textCtrlFrame(offset / 2.0, offset, offset / 2.0 + 300.0, offset + 20.0);
	BStringView* textCtrlInfo = new BStringView(
		textCtrlFrame, "info", "The \'$\' represents the filename in filename.tex");
	commandBox->AddChild(textCtrlInfo);

	BRect bitRect(
		offset / 2.0, offset + globalTcOffset, offset / 2.0 + 31.0, offset + 31.0 + globalTcOffset);
	BRect secCol(bitRect);
	secCol.OffsetBy(secOffset, 0.0);
	// secCol.OffsetBy(0.0, bitoffset);

	float textCtrlWidth = 105.0, textCtrlHeight = 20.0, textCtrlCo = 6.0;

	BRect bitctrlRect(offset / 2.0 + bitoffset, offset + textCtrlCo + globalTcOffset,
		offset / 2.0 + bitoffset + textCtrlWidth,
		textCtrlCo + offset + textCtrlHeight + globalTcOffset);
	BRect secColCtrl(bitctrlRect);
	secColCtrl.OffsetBy(secOffset, 0.0);
	secColCtrl.right += 50.0;

	// tex to dvi
	BRect bitmapFrame(0.0, 0.0, 31.0, 31.0);
	BBitmap* texDviBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	texDviBitmap->SetBits(IconTexToDVI, 3072, 0, B_CMAP8);
	commandBox->AddChild(new BitmapView(bitRect, texDviBitmap));

	BTextControl* texDviCtrl
		= new BTextControl(bitctrlRect, K_LATEX_CMD, NULL, "", GetPrefsMessage(K_LATEX_CMD));
	texDviCtrl->SetText(preferences->commands["latex_cmd"]);
	commandBox->AddChild(texDviCtrl);
	bitctrlRect.OffsetBy(0.0, bitoffset);

	// dvi to pdf
	bitRect.OffsetBy(0.0, bitoffset);
	BBitmap* dviPdfBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	dviPdfBitmap->SetBits(IconDVIToPDF, 3072, 0, B_CMAP8);
	commandBox->AddChild(new BitmapView(bitRect, dviPdfBitmap));

	BTextControl* dviPdfCtrl
		= new BTextControl(bitctrlRect, K_DVIPDF_CMD, NULL, "", GetPrefsMessage(K_DVIPDF_CMD));
	dviPdfCtrl->SetText(preferences->commands["dvipdf_cmd"]);
	commandBox->AddChild(dviPdfCtrl);
	bitctrlRect.OffsetBy(0.0, bitoffset);

	// dvi to ps
	bitRect.OffsetBy(0.0, bitoffset);
	BBitmap* dviPsBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	dviPsBitmap->SetBits(IconDVIToPS, 3072, 0, B_CMAP8);
	commandBox->AddChild(new BitmapView(bitRect, dviPsBitmap));

	BTextControl* dviPsCtrl
		= new BTextControl(bitctrlRect, K_DVIPS_CMD, NULL, "", GetPrefsMessage(K_DVIPS_CMD));
	dviPsCtrl->SetText(preferences->commands["dvips_cmd"]);
	commandBox->AddChild(dviPsCtrl);
	bitctrlRect.OffsetBy(0.0, bitoffset);

	// ps to pdf
	bitRect.OffsetBy(0.0, bitoffset);
	BBitmap* psPdfBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	psPdfBitmap->SetBits(IconPSToPDF, 3072, 0, B_CMAP8);
	commandBox->AddChild(new BitmapView(bitRect, psPdfBitmap));

	BTextControl* psPdfCtrl
		= new BTextControl(bitctrlRect, K_PS2PDF_CMD, NULL, "", GetPrefsMessage(K_PS2PDF_CMD));
	psPdfCtrl->SetText(preferences->commands["ps2pdf_cmd"]);
	commandBox->AddChild(psPdfCtrl);
	bitctrlRect.OffsetBy(0.0, bitoffset);

	// tex to pdf
	bitRect.OffsetBy(0.0, bitoffset);
	BBitmap* texPdfBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	texPdfBitmap->SetBits(IconTexToPDF, 3072, 0, B_CMAP8);
	commandBox->AddChild(new BitmapView(bitRect, texPdfBitmap));

	BTextControl* texPdfCtrl
		= new BTextControl(bitctrlRect, K_PDFLATEX_CMD, NULL, "", GetPrefsMessage(K_PDFLATEX_CMD));
	texPdfCtrl->SetText(preferences->commands["pdflatex_cmd"]);
	commandBox->AddChild(texPdfCtrl);
	bitctrlRect.OffsetBy(0.0, bitoffset);

	// tex to html
	bitRect.OffsetBy(0.0, bitoffset);
	BBitmap* texHtmlBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	texHtmlBitmap->SetBits(IconTexToHTML, 3072, 0, B_CMAP8);
	commandBox->AddChild(new BitmapView(bitRect, texHtmlBitmap));

	BTextControl* texHtmlCtrl = new BTextControl(
		bitctrlRect, K_LATEX2HTML_CMD, NULL, "", GetPrefsMessage(K_LATEX2HTML_CMD));
	texHtmlCtrl->SetText(preferences->commands["latex2html_cmd"]);
	commandBox->AddChild(texHtmlCtrl);
	bitctrlRect.OffsetBy(0.0, bitoffset);

	// ps
	bitRect.OffsetBy(0.0, bitoffset);
	BBitmap* postScriptBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	postScriptBitmap->SetBits(IconPrevPS, 3072, 0, B_CMAP8);
	commandBox->AddChild(new BitmapView(secCol, postScriptBitmap));

	BTextControl* postScriptCtrl = new BTextControl(
		secColCtrl, K_POSTSCRIPT_CMD, NULL, "", GetPrefsMessage(K_POSTSCRIPT_CMD));
	postScriptCtrl->SetText(preferences->commands["postscript_cmd"]);
	commandBox->AddChild(postScriptCtrl);

	BRect comboxRect = commandBox->Bounds();
	BRect comboxbtnRect(comboxRect.right - bspacing - bwidth,
		comboxRect.bottom - bspacing - bheight, comboxRect.right - bspacing,
		comboxRect.bottom - bspacing);
	BButton* resetCommandDefaults = new BButton(comboxbtnRect, "resetCommandDefaults", "Defaults",
		new BMessage(PrefsConstants::K_PREFS_VIEW_RESET_COMMAND_DEFAULTS));
	commandBox->AddChild(resetCommandDefaults);

	return commandBox;
}

BView*
PrefsWindow::constructColorBox(BRect frame)
{
	BBox* colorBox = new BBox(frame);
	colorBox->SetLabel("TexView");

	float offset = 30.0, texViewWidth = 280.0, texHeight = 230.0, adjust = 5.0, cbheight = 14.0,
		  cbspacing = 10.0, bwidth = 80.0, bheight = 30.0, bspacing = 10.0;

	BRect bounds = colorBox->Bounds();

	float texViewX1 = bounds.right - texViewWidth - (offset * 2.0 / 3.0);
	float texViewX2 = bounds.right - (offset * 2.0 / 3.0) - B_V_SCROLL_BAR_WIDTH;
	BRect texViewRect(texViewX1, offset - adjust, texViewX2, offset + texHeight - adjust);
	BRect texViewTextRect(texViewRect);

	TexView* texView = new TexView(texViewRect, texViewTextRect);
	StartWatching(texView, PrefsConstants::K_PREFS_UPDATE);
	// Find MainWindow
	// TODO Is there a better solution ?

	MainWindow* mw;
	for (int i = 0; i < be_app->CountWindows(); i++) {
		if (dynamic_cast<MainWindow*>(be_app->WindowAt(i))) {
			mw = (MainWindow*)be_app->WindowAt(i);
			break;
		}
	}
	// List of ProjectItems
	StartWatching(mw->GetProjectView(), PrefsConstants::K_PREFS_UPDATE);

	// Sample text

	BString sampleText
		= "Welcome to \\betex!\\\\\n%This is a Comment\n$Phe\\alpha r the "
		  "Calculus$\n\\subsection{About \\betex}\n\\betex is \\textbf{cool}!\\\\\n``To Be Or Not "
		  "To Be\'\'\n";
	texView->SetText(sampleText.String(), sampleText.Length());
	texView->MakeEditable(false);
	texView->MakeSelectable(false);

	BScrollView* texViewScroll
		= new BScrollView("textViewScroll", texView, B_FOLLOW_LEFT | B_FOLLOW_TOP,
			B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE, false, true, B_FANCY_BORDER);
	texViewScroll->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	colorBox->AddChild(texViewScroll);
	texView->SetTextRect(texView->Bounds());

	// fg color button
	BRect ccRect((offset * 2 / 3), offset - adjust, offset + 150, offset + cbheight - adjust);

	ColorButton* fgColorButton = new ColorButton(
		ccRect, K_FG_COLOR, GetPrefsMessage(K_FG_COLOR), preferences->fg_color, " Text");
	colorBox->AddChild(fgColorButton);

	ccRect.OffsetBy(0, cbheight + cbspacing);
	ColorButton* bgColorButton = new ColorButton(
		ccRect, K_BG_COLOR, GetPrefsMessage(K_BG_COLOR), preferences->bg_color, "Background");
	colorBox->AddChild(bgColorButton);

	ccRect.OffsetBy(0, cbheight + cbspacing);
	ColorButton* genericCmdColorButton = new ColorButton(ccRect, K_GEN_CMD_COLOR,
		GetPrefsMessage(K_GEN_CMD_COLOR), preferences->generic_cmd_color, "Generic Commands");
	colorBox->AddChild(genericCmdColorButton);

	ccRect.OffsetBy(0, cbheight + cbspacing);
	ColorButton* formatCmdColorButton = new ColorButton(ccRect, K_FORMAT_CMD_COLOR,
		GetPrefsMessage(K_FORMAT_CMD_COLOR), preferences->format_cmd_color, "Formatting Commands");
	colorBox->AddChild(formatCmdColorButton);

	ccRect.OffsetBy(0, cbheight + cbspacing);
	ColorButton* specialCmdColorButton = new ColorButton(ccRect, K_SPECIAL_CMD_COLOR,
		GetPrefsMessage(K_SPECIAL_CMD_COLOR), preferences->special_cmd_color, "Special Commands");
	colorBox->AddChild(specialCmdColorButton);

	ccRect.OffsetBy(0, cbheight + cbspacing);
	ColorButton* mathModeColorButton = new ColorButton(ccRect, K_MATH_MODE_COLOR,
		GetPrefsMessage(K_MATH_MODE_COLOR), preferences->math_mode_color, "Math Mode");
	colorBox->AddChild(mathModeColorButton);

	ccRect.OffsetBy(0, cbheight + cbspacing);
	ColorButton* commaColorButton = new ColorButton(ccRect, K_COMMA_COLOR,
		GetPrefsMessage(K_COMMA_COLOR), preferences->comma_color, "Quotation Marks");
	colorBox->AddChild(commaColorButton);

	ccRect.OffsetBy(0, cbheight + cbspacing);
	ColorButton* commentColorButton = new ColorButton(ccRect, K_COMMENT_COLOR,
		GetPrefsMessage(K_COMMENT_COLOR), preferences->comment_color, "Comments");
	colorBox->AddChild(commentColorButton);

	ccRect.OffsetBy(0, cbheight + cbspacing);
	ColorButton* puncSymbolColorButton
		= new ColorButton(ccRect, K_PUNC_SYMBOL_COLOR, GetPrefsMessage(K_PUNC_SYMBOL_COLOR),
			preferences->punc_symbol_color, "Punctuation / Symbols");
	colorBox->AddChild(puncSymbolColorButton);

	ccRect.OffsetBy(0, cbheight + cbspacing);
	// TODO REMOVE URL	ColorButton *htmlColorButton = new ColorButton(ccRect, K_HTML_COLOR,
	// GetPrefsMessage(K_HTML_COLOR), preferences->html_color,"Urls");
	// TODO REMOVE URL	colorBox->AddChild(htmlColorButton);

	BRect cbRect = colorBox->Bounds();
	BRect cbbtnRect(cbRect.right - bspacing - bwidth, cbRect.bottom - bspacing - bheight,
		cbRect.right - bspacing, cbRect.bottom - bspacing);
	BButton* resetColorDefaults = new BButton(cbbtnRect, "fResetColorDefaults", "Defaults",
		new BMessage(PrefsConstants::K_PREFS_VIEW_RESET_COLOR_DEFAULTS));
	colorBox->AddChild(resetColorDefaults);

	float sliderWidth = 300.0, vertOffset = 40.0, minVal = 5.0, maxVal = 30.0;
	BRect sliderFrame(ccRect.left, ccRect.top + vertOffset, ccRect.left + sliderWidth,
		ccRect.top + vertOffset + 100.0);
	BSlider* fontSizeSlider = new BSlider(
		sliderFrame, K_FONT_SIZE, "Font Size", GetPrefsMessage(K_FONT_SIZE), minVal, maxVal);

	fontSizeSlider->SetBarColor(ui_color(B_WINDOW_TAB_COLOR));
	fontSizeSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fontSizeSlider->SetHashMarkCount(26);
	fontSizeSlider->SetValue(preferences->FontSize);
	fontSizeSlider->SetLimitLabels("Small", "Large");
	fontSizeSlider->SetModificationMessage(GetPrefsMessage(K_FONT_SIZE));
	colorBox->AddChild(fontSizeSlider);

	rgb_color fire_r = {223, 0, 50};
	fontSizeSlider->UseFillColor(true, &fire_r);

	return colorBox;
}

BView*
PrefsWindow::constructToolbarBox(BRect frame)
{
	BBox* toolbarBox = new BBox(frame);
	toolbarBox->SetLabel("Toolbars");

	float vertCbSpacing = 25.0, offset = 30.0, adjust = 5.0, bwidth = 80.0, bheight = 30.0,
		  bspacing = 10.0;

	BRect checkboxRect = BRect((offset * 2.0 / 3.0), offset - adjust, (offset * 2.0 / 3.0) + 150.0,
		offset - adjust + vertCbSpacing - 5.0);

	BCheckBox* tbInvertCheck = new BCheckBox(checkboxRect, K_IS_TEXBAR_INVERTED,
		"Invert Toolbar Colors", GetPrefsMessage(K_IS_TEXBAR_INVERTED));
	bool isTexBarInverted = preferences->IsTexBarInverted;
	tbInvertCheck->SetValue(isTexBarInverted);
	toolbarBox->AddChild(tbInvertCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);

	BCheckBox* tbGreekAbsentCheck = new BCheckBox(
		checkboxRect, K_IS_GREEK_ABSENT, "Greek Letters", GetPrefsMessage(K_IS_GREEK_ABSENT));
	bool isGreekAbsent = preferences->IsGreekAbsent;
	tbGreekAbsentCheck->SetValue(!isGreekAbsent);
	toolbarBox->AddChild(tbGreekAbsentCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);

	BCheckBox* tbBigAbsentCheck = new BCheckBox(
		checkboxRect, K_IS_BIG_ABSENT, "Big Operators", GetPrefsMessage(K_IS_BIG_ABSENT));
	bool isBigAbsent = preferences->IsBigAbsent;
	tbBigAbsentCheck->SetValue(!isBigAbsent);
	toolbarBox->AddChild(tbBigAbsentCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);

	BCheckBox* tbBinaryAbsentCheck = new BCheckBox(
		checkboxRect, K_IS_BINARY_ABSENT, "Binary Operators", GetPrefsMessage(K_IS_BINARY_ABSENT));
	bool isBinaryAbsent = preferences->IsBinaryAbsent;
	tbBinaryAbsentCheck->SetValue(!isBinaryAbsent);
	toolbarBox->AddChild(tbBinaryAbsentCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);

	BCheckBox* tbMiscAbsentCheck = new BCheckBox(
		checkboxRect, K_IS_MISC_ABSENT, "Misc Symbols", GetPrefsMessage(K_IS_MISC_ABSENT));
	bool isMiscAbsent = preferences->IsMiscAbsent;
	tbMiscAbsentCheck->SetValue(!isMiscAbsent);
	toolbarBox->AddChild(tbMiscAbsentCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);

	BCheckBox* tbBinRelAbsentCheck = new BCheckBox(checkboxRect, K_IS_BIN_REL_ABSENT,
		"Binary Relations", GetPrefsMessage(K_IS_BIN_REL_ABSENT));
	bool isBinRelAbsent = preferences->IsBinRelAbsent;
	tbBinRelAbsentCheck->SetValue(!isBinRelAbsent);
	toolbarBox->AddChild(tbBinRelAbsentCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);

	BCheckBox* tbMMAAbsentCheck = new BCheckBox(
		checkboxRect, K_IS_MMA_ABSENT, "Math Mode Accents", GetPrefsMessage(K_IS_MMA_ABSENT));
	bool isMMAAbsent = preferences->IsMMAAbsent;
	tbMMAAbsentCheck->SetValue(!isMMAAbsent);
	toolbarBox->AddChild(tbMMAAbsentCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);

	BCheckBox* tbIntlAbsentCheck = new BCheckBox(
		checkboxRect, K_IS_INT_ABSENT, "International", GetPrefsMessage(K_IS_INT_ABSENT));
	bool isIntlAbsent = preferences->IsIntlAbsent;
	tbIntlAbsentCheck->SetValue(!isIntlAbsent);
	toolbarBox->AddChild(tbIntlAbsentCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);

	BCheckBox* tbTypeFaceAbsentCheck = new BCheckBox(
		checkboxRect, K_IS_TYPE_FACE_ABSENT, "Type Faces", GetPrefsMessage(K_IS_TYPE_FACE_ABSENT));
	bool isTypeFaceAbsent = preferences->IsTypeFaceAbsent;
	tbTypeFaceAbsentCheck->SetValue(!isTypeFaceAbsent);
	toolbarBox->AddChild(tbTypeFaceAbsentCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);

	BCheckBox* tbFunctionAbsentCheck = new BCheckBox(
		checkboxRect, K_IS_FUNCTION_ABSENT, "Functions", GetPrefsMessage(K_IS_FUNCTION_ABSENT));
	bool isFunctionAbsent = preferences->IsFunctionAbsent;
	tbFunctionAbsentCheck->SetValue(!isFunctionAbsent);
	toolbarBox->AddChild(tbFunctionAbsentCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);

	BRect toolbarRect = toolbarBox->Bounds();
	BRect toolbarbtnRect(toolbarRect.right - bspacing - bwidth,
		toolbarRect.bottom - bspacing - bheight, toolbarRect.right - bspacing,
		toolbarRect.bottom - bspacing);
	BButton* resetToolbarDefaults = new BButton(toolbarbtnRect, "resetToolbarDefaults", "Defaults",
		new BMessage(PrefsConstants::K_PREFS_VIEW_RESET_TOOLBAR_DEFAULTS));
	toolbarBox->AddChild(resetToolbarDefaults);

	StartWatching(m_texBar, PrefsConstants::K_PREFS_UPDATE);

	return toolbarBox;
}

void
PrefsWindow::ResetToDefaults(uint32 resetFlag)
{
	prefsLock.Lock();

	if ((resetFlag & PrefsConstants::K_RESET_GENERAL_PREFS) != 0) {
		preferences->ResetGeneral();

		/* Reset Widgets */
		ColorButton* bubbleColorButton = (ColorButton*)FindView(K_BUBBLE_COLOR);
		bubbleColorButton->SetValue(preferences->bubble_color);

		BCheckBox* checkBox = (BCheckBox*)FindView(K_IS_RECENT_DOCS_SHOWN);

		checkBox->SetValue(preferences->IsRecentDocsPathShown);

		checkBox = (BCheckBox*)FindView(K_IS_ACTIVATION_OK);
		checkBox->SetValue(preferences->IsActivationOk);

		BTextControl* textControl = (BTextControl*)FindView(K_NUM_RECENT_DOCS);
		textControl->SetText(preferences->NumRecentDocs);

		textControl = (BTextControl*)FindView(K_DATE_FLAGS);
		textControl->SetText(preferences->dateFlags);
	}

	if ((resetFlag & PrefsConstants::K_RESET_COMMAND_PREFS) != 0) {
		preferences->ResetCommands();

		BTextControl* texDvi = (BTextControl*)FindView(K_LATEX_CMD);
		texDvi->SetText(preferences->commands["latex_cmd"]);

		BTextControl* dvipdf = (BTextControl*)FindView(K_DVIPDF_CMD);
		dvipdf->SetText(preferences->commands["dvipdf_cmd"]);

		BTextControl* dvips = (BTextControl*)FindView(K_DVIPS_CMD);
		dvips->SetText(preferences->commands["dvips_cmd"]);

		BTextControl* ps2pdf = (BTextControl*)FindView(K_PS2PDF_CMD);
		ps2pdf->SetText(preferences->commands["ps2pdf_cmd"]);

		BTextControl* pdflatex = (BTextControl*)FindView(K_PDFLATEX_CMD);
		pdflatex->SetText(preferences->commands["pdflatex_cmd"]);

		BTextControl* latex2html = (BTextControl*)FindView(K_LATEX2HTML_CMD);
		latex2html->SetText(preferences->commands["latex2html_cmd"]);

		BTextControl* postscript = (BTextControl*)FindView(K_POSTSCRIPT_CMD);
		postscript->SetText(preferences->commands["postscript_cmd"]);
	}

	if ((resetFlag & PrefsConstants::K_RESET_TEXVIEW_PREFS) != 0) {
		preferences->ResetColors();

		ColorButton* colorButton;

		colorButton = (ColorButton*)FindView(K_FG_COLOR);
		colorButton->SetValue(preferences->fg_color);

		colorButton = (ColorButton*)FindView(K_BG_COLOR);
		colorButton->SetValue(preferences->bg_color);

		colorButton = (ColorButton*)FindView(K_GEN_CMD_COLOR);
		colorButton->SetValue(preferences->generic_cmd_color);

		colorButton = (ColorButton*)FindView(K_FORMAT_CMD_COLOR);
		colorButton->SetValue(preferences->format_cmd_color);

		colorButton = (ColorButton*)FindView(K_SPECIAL_CMD_COLOR);
		colorButton->SetValue(preferences->special_cmd_color);

		colorButton = (ColorButton*)FindView(K_MATH_MODE_COLOR);
		colorButton->SetValue(preferences->math_mode_color);

		colorButton = (ColorButton*)FindView(K_COMMA_COLOR);
		colorButton->SetValue(preferences->comma_color);

		colorButton = (ColorButton*)FindView(K_COMMENT_COLOR);
		colorButton->SetValue(preferences->comment_color);

		colorButton = (ColorButton*)FindView(K_PUNC_SYMBOL_COLOR);
		colorButton->SetValue(preferences->punc_symbol_color);
		/*
				colorButton = (ColorButton *)FindView(K_HTML_COLOR);
				colorButton->SetValue(preferences->html_color);
		*/
		BSlider* fontSizeSlider = (BSlider*)FindView(K_FONT_SIZE);
		fontSizeSlider->SetValue(preferences->FontSize);

		// Find MainWindow
		// TODO Is there a better solution ?

		MainWindow* mw;
		for (int i = 0; i < be_app->CountWindows(); i++) {
			if (dynamic_cast<MainWindow*>(be_app->WindowAt(i))) {
				mw = (MainWindow*)be_app->WindowAt(i);
				break;
			}
		}
		// TODO Test if mw is null ?
		mw->Lock();
		if (mw->CurrentTexView()) {
			mw->CurrentTexView()->UpdateColors();
			mw->CurrentTexView()->UpdateFontSize();
		}
		mw->Unlock();
	}

	if ((resetFlag & PrefsConstants::K_RESET_TOOLBAR_PREFS) != 0) {
		preferences->ResetToolbar();

		BCheckBox* tbInvertToolBarCheck = (BCheckBox*)FindView(K_IS_TEXBAR_INVERTED);
		bool isInvertToolBar = preferences->IsTexBarInverted;
		tbInvertToolBarCheck->SetValue(isInvertToolBar);

		BCheckBox* tbGreekAbsentCheck = (BCheckBox*)FindView(K_IS_GREEK_ABSENT);
		bool isGreekAbsent = preferences->IsGreekAbsent;
		tbGreekAbsentCheck->SetValue(!isGreekAbsent);

		BCheckBox* tbBigAbsentCheck = (BCheckBox*)FindView(K_IS_BIG_ABSENT);
		bool isBigAbsent = preferences->IsBigAbsent;
		tbBigAbsentCheck->SetValue(!isBigAbsent);

		BCheckBox* tbBinaryAbsentCheck = (BCheckBox*)FindView(K_IS_BINARY_ABSENT);
		bool isBinaryAbsent = preferences->IsBinaryAbsent;
		tbBinaryAbsentCheck->SetValue(!isBinaryAbsent);

		BCheckBox* tbMiscAbsentCheck = (BCheckBox*)FindView(K_IS_MISC_ABSENT);
		bool isMiscAbsent = preferences->IsMiscAbsent;
		tbMiscAbsentCheck->SetValue(!isMiscAbsent);

		BCheckBox* tbBinRelAbsentCheck = (BCheckBox*)FindView(K_IS_BIN_REL_ABSENT);
		bool isBinRelAbsent = preferences->IsBinRelAbsent;
		tbBinRelAbsentCheck->SetValue(!isBinRelAbsent);

		BCheckBox* tbMMAAbsentCheck = (BCheckBox*)FindView(K_IS_MMA_ABSENT);
		bool isMMAAbsent = preferences->IsMMAAbsent;
		tbMMAAbsentCheck->SetValue(!isMMAAbsent);

		BCheckBox* tbIntlAbsentCheck = (BCheckBox*)FindView(K_IS_INT_ABSENT);
		bool isIntlAbsent = preferences->IsIntlAbsent;
		tbIntlAbsentCheck->SetValue(!isIntlAbsent);

		BCheckBox* tbTypeFaceAbsentCheck = (BCheckBox*)FindView(K_IS_TYPE_FACE_ABSENT);
		bool isTypeFaceAbsent = preferences->IsTypeFaceAbsent;
		tbTypeFaceAbsentCheck->SetValue(!isTypeFaceAbsent);

		BCheckBox* tbFunctionAbsentCheck = (BCheckBox*)FindView(K_IS_FUNCTION_ABSENT);
		bool isFunctionAbsent = preferences->IsFunctionAbsent;
		tbFunctionAbsentCheck->SetValue(!isFunctionAbsent);

		// To update the Hidden statuses in the "preferences" object
		BMessage notice(PrefsConstants::K_PREFS_UPDATE);
		SendNotices(PrefsConstants::K_PREFS_UPDATE, &notice);
	}


	// TODO : displace in menu managing code
	preferences->ResetLayout();

	prefsLock.Unlock();
}

BMessage*
PrefsWindow::GetPrefsMessage(BString prefsID)
{
	BMessage* prefsMessage = new BMessage(PrefsConstants::K_PREFS_UPDATE);
	prefsMessage->AddString(K_PREFS_ID, prefsID);
	return prefsMessage;
}
