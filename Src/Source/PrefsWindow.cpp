/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef PREFS_WINDOW_H
#include "PrefsWindow.h"
#endif

#include <be/interface/Box.h>
#include <be/interface/Font.h>
#include <be/interface/Alert.h>
#include <be/interface/ScrollView.h>
#include <be/interface/Screen.h>
#include <be/support/ClassInfo.h>
#include <be/support/TypeConstants.h>
#include "constants.h"
#include "Preferences.h"
#include "MessageFields.h"
#include "PrefsListItem.h"
#include "ConverterIcons.h"

//======================BitmapView============================================
BitmapView::BitmapView(BRect frame, BBitmap *bitmap) 
				:	BView(frame, "bitmapview", B_FOLLOW_NONE, B_WILL_DRAW),
					m_bitmap(bitmap)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));	
}

BitmapView::~BitmapView()
{
	delete m_bitmap;
}

void BitmapView::Draw(BRect updateRect)
{
	SetDrawingMode(B_OP_ALPHA);
	DrawBitmap(m_bitmap);
}

//===========================PrefsWindow======================================
PrefsWindow::PrefsWindow(BRect frame, BMessenger *messenger) 
				:	BWindow(frame, "Preferences", B_FLOATING_WINDOW, B_NOT_ZOOMABLE|B_NOT_RESIZABLE|B_NOT_CLOSABLE|B_ASYNCHRONOUS_CONTROLS),
					m_msgr(messenger),
					m_colourPrefsChanged(false),
					m_fontSizeChanged(false),
					m_toolbarChanged(false)
{	
	SetFeel(B_NORMAL_WINDOW_FEEL);
	
	m_parent = new BView(Bounds(), "parent", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	m_parent->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(m_parent);
	
	prefsLock.Lock();	
	BFont font(be_fixed_font);
	float fontSize = 10.0f;
	if (preferences.FindFloat(K_FONT_SIZE, &fontSize) == B_OK)
		font.SetSize(fontSize);
	prefsLock.Unlock();
		
	float prefsListWidth = 80.0f;		
	BRect prefsListFrame(10.0f, 10.0f, 10.0f + prefsListWidth, Bounds().bottom - 10.0f);	
	PrefsListView *prefsListView = new PrefsListView(prefsListFrame, m_parent);
	BScrollView *prefsListScroll = new BScrollView("ScrollView", prefsListView, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW,false,false);
	m_parent->AddChild(prefsListScroll);
		
	float 	bwidth = 80.0f,
			bheight = 30.0f,
			bspacing = 10.0f
			;
	
	BRect okButtonFrame(Bounds().right - bspacing - bwidth, Bounds().bottom - bspacing - bheight, Bounds().right - bspacing,Bounds().bottom - bspacing);
	BButton *okButton = new BButton(okButtonFrame, "okButton", "Ok", new BMessage(PrefsConstants::K_PREFS_VIEW_OK));
	m_parent->AddChild(okButton);
	okButton->MakeDefault(true);
	
	BRect cancelButtonFrame = okButtonFrame;
	cancelButtonFrame.OffsetBy(-(bwidth + 2.0f * bspacing), 0.0f);
	BButton *cancelButton = new BButton(cancelButtonFrame,"cancelButton","Cancel",new BMessage(PrefsConstants::K_PREFS_VIEW_CANCEL));
	m_parent->AddChild(cancelButton);
	
	float respiteWidth = 41.0f;
	BRect boxRect(10.0f + prefsListWidth + 10.0f , 2.0f, Bounds().right - 7.0f , Bounds().bottom - 10.0f - respiteWidth);	
	prefsListView->AddItem(new PrefsListItem(constructGeneralBox(boxRect), m_parent, "General"));
	prefsListView->AddItem(new PrefsListItem(constructCommandBox(boxRect), m_parent, "Commands"));
	prefsListView->AddItem(new PrefsListItem(constructColourBox(boxRect), m_parent, "TexView"));
	prefsListView->AddItem(new PrefsListItem(constructToolbarBox(boxRect), m_parent, "Toolbars"));	
	prefsListView->Select(0);
	prefsListView->MakeFocus(true);	
}

PrefsWindow::~PrefsWindow()
{
	delete m_msgr;
}

void PrefsWindow::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		case PrefsConstants::K_PREFS_VIEW_RESET_COLOUR_DEFAULTS:
		{
			ResetToDefaults(PrefsConstants::K_RESET_COLOUR_PREFS);
		}
		break;		
		case PrefsConstants::K_PREFS_VIEW_RESET_COMMAND_DEFAULTS:
		{
			ResetToDefaults(PrefsConstants::K_RESET_COMMAND_PREFS);
		}		
		break;
		case PrefsConstants::K_PREFS_VIEW_RESET_TOOLBAR_DEFAULTS:
		{
			ResetToDefaults(PrefsConstants::K_RESET_TOOLBAR_PREFS);	
		}
		break;
		case PrefsConstants::K_PREFS_VIEW_RESET_GENERAL_DEFAULTS:
		{
			ResetToDefaults(PrefsConstants::K_RESET_GENERAL_PREFS);
		}
		break;
		case PrefsConstants::K_PREFS_UPDATE:
		{			
			//update the preferences message, from view values
			BString prefsID;			
			if (message->FindString(K_PREFS_ID, &prefsID) == B_OK)
			{
				BView *changedView = m_parent->FindView(prefsID.String());
				prefsLock.Lock();
				//different view have different kinds of values
				if (is_instance_of(changedView, BTextControl))
				{
					//a textcontrol value was changed, update preferences with new text
					BTextControl *textControl = dynamic_cast<BTextControl*>(changedView);
					preferences.ReplaceString(prefsID.String(), textControl->Text());
				}
				else if (is_instance_of(changedView, BCheckBox))
				{
					//a checkbox value was changed, update preferences with new bool value(on/off)
					BCheckBox *checkBox = dynamic_cast<BCheckBox*>(changedView);
					preferences.ReplaceBool(prefsID.String(), checkBox->Value());
				}
				else if (is_instance_of(changedView, BSlider))
				{
					//a slider value was changed, update preferences with new slider value
					BSlider *slider = dynamic_cast<BSlider*>(changedView);
					preferences.ReplaceInt32(prefsID.String(), slider->Value());
				}
				else if (is_instance_of(changedView, ColourButton))
				{
					//a colourcontrol value was changed, update preferences with new colour
					ColourButton *colourControl = dynamic_cast<ColourButton*>(changedView);
					preferences.ReplaceData(prefsID.String(),B_RGB_COLOR_TYPE, &colourControl->Value(), sizeof(rgb_color));
				}
				prefsLock.Unlock();
			}
		}
		break;
		case PrefsConstants::K_LOAD_PREFERENCES:
		{
			//set preferences view values to values of the preferences message
			BString prefsID;
			if (message->FindString(K_PREFS_ID, &prefsID) == B_OK)
			{
				//find out which view value has to be updated
				BView *changedView = m_parent->FindView(prefsID.String());
				prefsLock.Lock();
				const char *name;
				uint32 type;
				int32 count;
				for (int32 i = 0; preferences.GetInfo(B_ANY_TYPE, i, &name, &type, &count) == B_OK; i++)
				{
					//find out what kind of field we are using
					switch (type)
					{
						case B_INT32_TYPE:
						{
							int32 value;
							preferences.FindInt32(name, &value);
							if (is_instance_of(changedView, BSlider))
							{
								BSlider *slider = dynamic_cast<BSlider*>(changedView);
								slider->SetValue(value);
							}
						}
						break;
						case B_BOOL_TYPE:
						{
							bool value;
							preferences.FindBool(name, &value);
							if (is_instance_of(changedView, BCheckBox))
							{
								BCheckBox *checkBox = dynamic_cast<BCheckBox*>(changedView);
								checkBox->SetValue(value);
							}
						}
						break;
						case B_RGB_COLOR_TYPE:
						{
							rgb_color *colour;
							ssize_t size;
							preferences.FindData(name, B_RGB_COLOR_TYPE, (const void**)&colour, &size);
							if (is_instance_of(changedView, ColourButton))
							{
								ColourButton *colourControl = dynamic_cast<ColourButton*>(changedView);
								colourControl->SetValue(*colour);
							}
						}
						break;
						case B_STRING_TYPE:
						{
							BString string;
							preferences.FindString(name, &string);
							if (is_instance_of(changedView, ColourButton))
							{
								BTextControl *textControl = dynamic_cast<BTextControl*>(changedView);
								textControl->SetText(string.String());
							}
						}
						break;
					}
				} 				
				prefsLock.Unlock();
				//make sure the new view values are drawn!
				changedView->Invalidate();
			}
		}
		break;
		default:
			BWindow::MessageReceived(message);
		break;
	}
}

void PrefsWindow::Quit()
{
	m_msgr->SendMessage(new BMessage(PrefsConstants::K_PREFS_WINDOW_QUIT));
	BWindow::Quit();
}

BView* PrefsWindow::constructGeneralBox(BRect frame)
{
	BBox* generalBox = new BBox(frame);
	generalBox->SetLabel("General");
	
	float 	offset = 30.0f,
			bwidth = 80.0f,
			bheight = 30.0f,
			bspacing = 10.0f,
			cbheight = 14.0f,
			adjust = 5.0f
			;
			
	float controlX = (offset * 2.0f / 3.0f);
	BRect bubbleFrame = BRect(controlX, offset - adjust, offset + 150.0f, offset + cbheight - adjust);	
	
	ColourButton *bubbleColourButton = new ColourButton(bubbleFrame, GetPrefsMessage(K_BUBBLE_COLOUR), ColourConstants::K_BLACK, "Tooltips");
	generalBox->AddChild(bubbleColourButton);
		
	BRect recentDocsPathFrame(controlX, 50.0f, controlX + 150.0f, 70.0f);
	BCheckBox *showRecentDocsPath = new BCheckBox(recentDocsPathFrame, K_IS_RECENT_DOCS_SHOWN, "Show Path in Recent Docs", GetPrefsMessage(K_IS_RECENT_DOCS_SHOWN));
	generalBox->AddChild(showRecentDocsPath);
	
	BRect isActivationFrame(controlX, 80.0f, controlX + 190.0f, 100.0f);
	BCheckBox *isActivationOk = new BCheckBox(isActivationFrame, K_IS_ACTIVATION_OK, "Activate Window on Opening Docs", GetPrefsMessage(K_IS_ACTIVATION_OK));	
	generalBox->AddChild(isActivationOk);
	
	BRect recentDocsFrame(controlX, 110.0f, controlX + 150.0f, 120.0f);
	BTextControl *numRecentDocs = new BTextControl(recentDocsFrame, K_NUM_RECENT_DOCS, "Number of Recent Docs", "", GetPrefsMessage(K_NUM_RECENT_DOCS));
	numRecentDocs->SetDivider(120.0f);
	numRecentDocs->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	generalBox->AddChild(numRecentDocs);
	
	BRect insertDateFrame(controlX, 140.0f, controlX + 200.0f, 150.0f);
	BTextControl *insertDateFlags = new BTextControl(insertDateFrame, K_DATE_FLAGS, "Insert Date Flags", "", GetPrefsMessage(K_DATE_FLAGS));
	generalBox->AddChild(insertDateFlags);	
	
	BRect generalRect = generalBox->Bounds();
	BRect generalbtnRect(generalRect.right - bspacing - bwidth, generalRect.bottom - bspacing - bheight, generalRect.right - bspacing, generalRect.bottom - bspacing);
	BButton *resetGeneralDefaults = new BButton(generalbtnRect,"fResetGeneralDefaults","Defaults",new BMessage(PrefsConstants::K_PREFS_VIEW_RESET_GENERAL_DEFAULTS));
	generalBox->AddChild(resetGeneralDefaults);
	
	return generalBox;
}

BView* PrefsWindow::constructCommandBox(BRect frame)
{
	BBox* commandBox = new BBox(frame);
	commandBox->SetLabel("Commands");
	
	float 	offset = 30.0f,
			bwidth = 80.0f,
			bheight = 30.0f,
			bspacing = 10.0f,			
			globalTcOffset = 45.0f,		
			bitoffset = 45.0,
			secOffset = 170.0f
			;
				
	BRect textCtrlFrame(offset / 2.0f, offset, offset / 2.0f + 300.0f, offset + 20.0f);
	BStringView *textCtrlInfo = new BStringView(textCtrlFrame, "info", "The \'$\' represents the filename in filename.tex");
	commandBox->AddChild(textCtrlInfo);	
	
	BRect bitRect(offset / 2.0f, offset + globalTcOffset, offset / 2.0f + 31.0f, offset + 31.0f + globalTcOffset);
	BRect secCol(bitRect);	
	secCol.OffsetBy(secOffset, 0.0f);	
	secCol.OffsetBy(0.0f, bitoffset);	
	
	float	textCtrlWidth = 100.0f,
			textCtrlHeight = 20.0f,
			textCtrlCo = 6.0f
			;
			
	BRect bitctrlRect(offset / 2.0f + bitoffset, offset + textCtrlCo + globalTcOffset, offset / 2.0f + bitoffset + textCtrlWidth, textCtrlCo + offset + textCtrlHeight + globalTcOffset);		
	BRect secColCtrl(bitctrlRect);
	secColCtrl.OffsetBy(secOffset, 0.0f);
	secColCtrl.right += 50.0f;
	
	//tex to dvi
	BRect bitmapFrame(0.0f, 0.0f, 31.0f, 31.0f);	
	BBitmap	*texDviBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	texDviBitmap->SetBits(TexToDVI, 3072, 0, B_CMAP8);
	commandBox->AddChild(new BitmapView(bitRect, texDviBitmap));
	
	BTextControl *texDviCtrl = new BTextControl(bitctrlRect, K_LATEX_CMD, NULL, "", GetPrefsMessage(K_LATEX_CMD));
	commandBox->AddChild(texDviCtrl);
	bitctrlRect.OffsetBy(0.0f, bitoffset);
	
	//dvi to pdf
	bitRect.OffsetBy(0.0f, bitoffset);	
	BBitmap* dviPdfBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	dviPdfBitmap->SetBits(DVIToPDF, 3072, 0, B_CMAP8);		
	commandBox->AddChild(new BitmapView(bitRect, dviPdfBitmap));
	
	BTextControl *dviPdfCtrl = new BTextControl(bitctrlRect, K_DVIPDF_CMD, NULL, "",  GetPrefsMessage(K_DVIPDF_CMD));
	commandBox->AddChild(dviPdfCtrl);
	bitctrlRect.OffsetBy(0.0f, bitoffset);
	
	//dvi to ps
	bitRect.OffsetBy(0.0f, bitoffset);	
	BBitmap* dviPsBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	dviPsBitmap->SetBits(DVIToPS, 3072, 0, B_CMAP8);		
	commandBox->AddChild(new BitmapView(bitRect, dviPsBitmap));
	
	BTextControl *dviPsCtrl = new BTextControl(bitctrlRect, K_DVIPS_CMD, NULL, "",  GetPrefsMessage(K_DVIPS_CMD));
	commandBox->AddChild(dviPsCtrl);
	bitctrlRect.OffsetBy(0.0f, bitoffset);
	
	//ps to pdf
	bitRect.OffsetBy(0.0f, bitoffset);	
	BBitmap* psPdfBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	psPdfBitmap->SetBits(PSToPDF, 3072, 0, B_CMAP8);		
	commandBox->AddChild(new BitmapView(bitRect, psPdfBitmap));
	
	BTextControl *psPdfCtrl = new BTextControl(bitctrlRect, K_PS2PDF_CMD, NULL, "",  GetPrefsMessage(K_PS2PDF_CMD));
	commandBox->AddChild(psPdfCtrl);
	bitctrlRect.OffsetBy(0.0f, bitoffset);
	
	//tex to pdf
	bitRect.OffsetBy(0.0f, bitoffset);			
	BBitmap* texPdfBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	texPdfBitmap->SetBits(TexToPDF,3072,0,B_CMAP8);
	commandBox->AddChild(new BitmapView(bitRect, texPdfBitmap));
	
	BTextControl *texPdfCtrl = new BTextControl(bitctrlRect, K_PDFLATEX_CMD, NULL, "",  GetPrefsMessage(K_PDFLATEX_CMD));
	commandBox->AddChild(texPdfCtrl);
	bitctrlRect.OffsetBy(0.0f, bitoffset);
	
	//tex to html
	bitRect.OffsetBy(0.0f, bitoffset);	
	BBitmap* texHtmlBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	texHtmlBitmap->SetBits(TexToHTML, 3072, 0, B_CMAP8);		
	commandBox->AddChild(new BitmapView(bitRect, texHtmlBitmap));
	
	BTextControl *texHtmlCtrl = new BTextControl(bitctrlRect, K_LATEX2HTML_CMD, NULL, "",  GetPrefsMessage(K_LATEX2HTML_CMD));
	commandBox->AddChild(texHtmlCtrl);
	bitctrlRect.OffsetBy(0.0f, bitoffset);
	
	//ps
	bitRect.OffsetBy(0.0f, bitoffset);	
	BBitmap* postScriptBitmap = new BBitmap(bitmapFrame, B_CMAP8);
	postScriptBitmap->SetBits(PrevPS, 3072, 0, B_CMAP8);
	commandBox->AddChild(new BitmapView(secCol, postScriptBitmap));
	
	BTextControl *postScriptCtrl = new BTextControl(secColCtrl, K_POSTSCRIPT_CMD, NULL, "", GetPrefsMessage(K_POSTSCRIPT_CMD));
	commandBox->AddChild(postScriptCtrl);
		
	BRect comboxRect = commandBox->Bounds();
	BRect comboxbtnRect(comboxRect.right - bspacing - bwidth, comboxRect.bottom - bspacing - bheight, comboxRect.right - bspacing, comboxRect.bottom - bspacing);
	BButton *resetCommandDefaults = new BButton(comboxbtnRect,"resetCommandDefaults","Defaults",new BMessage(PrefsConstants::K_PREFS_VIEW_RESET_COMMAND_DEFAULTS));
	commandBox->AddChild(resetCommandDefaults);
	
	return commandBox;
}

BView* PrefsWindow::constructColourBox(BRect frame)
{
	BBox* colourBox = new BBox(frame);
	colourBox->SetLabel("TexView");

	float	offset = 30.0f,
			texViewWidth = 280.0f,
			texHeight = 200.0f,
			adjust = 5.0f,
			cbheight = 14.0f,
			cbspacing = 10.0f,
			bwidth = 80.0f,
			bheight = 30.0f,
			bspacing = 10.0f	
			;
			
	BRect bounds = colourBox->Bounds();
	
	float texViewX1 = bounds.right - texViewWidth - (offset * 2.0f / 3.0f);
	float texViewX2 = bounds.right - (offset * 2.0f / 3.0f) - B_V_SCROLL_BAR_WIDTH;
	BRect texViewRect(texViewX1, offset - adjust, texViewX2, offset + texHeight - adjust);	
	BRect texViewTextRect(texViewRect);
		
	/*TexView *texView = new TexView(texViewRect, texViewTextRect, prefs);
	BString sampleText = "Welcome to \\betex!\\\\\n%This is a Comment\n$Phe\\alpha r the Calculus$\n\\subsection{About \\betex}\n\\betex is \\textbf{cool}!\\\\\n``To Be Or Not To Be\'\'\n";
	texView->SetText(sampleText.String(),sampleText.Length());
	texView->MakeEditable(false);
	texView->MakeSelectable(false);
	
	BScrollView *texViewScroll = new BScrollView("textViewScroll", texView, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE, false, true, B_FANCY_BORDER);
	texViewScroll->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	colourBox->AddChild(texViewScroll);	
	texView->SetTextRect(texView->Bounds());*/	
	
	//fg colour button
	BRect ccRect((offset*2/3),offset-adjust,offset+150,offset+cbheight-adjust);	
	rgb_color fgColour;
	ColourButton *fgColourButton = new ColourButton(ccRect, GetPrefsMessage(K_FG_COLOUR), fgColour," Text");
	colourBox->AddChild(fgColourButton);
	
	ccRect.OffsetBy(0,cbheight+cbspacing);
	rgb_color bgColour;
	ColourButton *bgColourButton = new ColourButton(ccRect, GetPrefsMessage(K_BG_COLOUR), bgColour, "Background");
	colourBox->AddChild(bgColourButton);
	
	ccRect.OffsetBy(0,cbheight+cbspacing);
	rgb_color genericCmdColour;
	ColourButton *genericCmdColourButton = new ColourButton(ccRect, GetPrefsMessage(K_GEN_CMD_COLOUR), genericCmdColour, "Generic Commands");
	colourBox->AddChild(genericCmdColourButton);
	
	ccRect.OffsetBy(0,cbheight+cbspacing);
	rgb_color formatCmdColour;
	ColourButton *formatCmdColourButton  = new ColourButton(ccRect, GetPrefsMessage(K_FORMAT_CMD_COLOUR), formatCmdColour, "Formatting Commands");
	colourBox->AddChild(formatCmdColourButton);
	
	ccRect.OffsetBy(0,cbheight+cbspacing);
	rgb_color specialCmdColour;
	ColourButton *specialCmdColourButton = new ColourButton(ccRect, GetPrefsMessage(K_SPECIAL_CMD_COLOUR), specialCmdColour, "Special Commands");
	colourBox->AddChild(specialCmdColourButton);
	
	ccRect.OffsetBy(0,cbheight+cbspacing);
	rgb_color mathModeColour;
	ColourButton *mathModeColourButton = new ColourButton(ccRect, GetPrefsMessage(K_MATH_MODE_COLOUR), mathModeColour, "Math Mode");
	colourBox->AddChild(mathModeColourButton);
	
	ccRect.OffsetBy(0,cbheight+cbspacing);
	rgb_color commaColour;
	ColourButton *commaColourButton = new ColourButton(ccRect, GetPrefsMessage(K_COMMA_COLOUR), commaColour, "Quotation Marks");
	colourBox->AddChild(commaColourButton);
	
	ccRect.OffsetBy(0,cbheight+cbspacing);
	rgb_color commentColour;
	ColourButton *commentColourButton = new ColourButton(ccRect,GetPrefsMessage(K_COMMENT_COLOUR),commentColour,"Comments");
	colourBox->AddChild(commentColourButton);
	
	ccRect.OffsetBy(0,cbheight+cbspacing);
	rgb_color puncSymbolColour;
	ColourButton *puncSymbolColourButton = new ColourButton(ccRect,GetPrefsMessage(K_PUNC_SYMBOL_COLOUR),puncSymbolColour,"Punctuation / Symbols");
	colourBox->AddChild(puncSymbolColourButton);
	
	ccRect.OffsetBy(0,cbheight+cbspacing);
	//rgb_color htmlColour;
	//ColourButton *htmlColourButton = new ColourButton(ccRect,GetPrefsMessage(K_HTML_COLOUR), htmlColour,"Urls");
	//colourBox->AddChild(htmlColourButton);
	
	BRect cbRect = colourBox->Bounds();
	BRect cbbtnRect(cbRect.right - bspacing - bwidth, cbRect.bottom - bspacing - bheight, cbRect.right - bspacing, cbRect.bottom - bspacing);
	BButton *resetColourDefaults = new BButton(cbbtnRect,"fResetColorDefaults","Defaults",new BMessage(PrefsConstants::K_PREFS_VIEW_RESET_COLOUR_DEFAULTS));
	colourBox->AddChild(resetColourDefaults);
	
	float	sliderWidth = 300.0f,
			vertOffset = 50.0f,
			minVal = 5.0f,
			maxVal = 30.0f
			;		
	BRect sliderFrame(ccRect.left, ccRect.top + vertOffset, ccRect.left + sliderWidth, ccRect.top + vertOffset + 100.0f);
	BSlider *fontSizeSlider = new BSlider(sliderFrame, K_FONT_SIZE, "Font Size", GetPrefsMessage(K_FONT_SIZE), minVal, maxVal);
	colourBox->AddChild(fontSizeSlider);
	
	fontSizeSlider->SetBarColor(ui_color(B_WINDOW_TAB_COLOR));
	fontSizeSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fontSizeSlider->SetHashMarkCount(26);
	float fontSize = 12.0f;
	fontSizeSlider->SetValue(fontSize);
	fontSizeSlider->SetLimitLabels("Small","Large");
	fontSizeSlider->SetModificationMessage(GetPrefsMessage(K_FONT_SIZE));
		
	rgb_color fire_r = {223,0,50};
	const rgb_color* fillcolor = &fire_r;
	fontSizeSlider->UseFillColor(true,fillcolor);
	
	return colourBox;
}

BView* PrefsWindow::constructToolbarBox(BRect frame)
{
	BBox* toolbarBox = new BBox(frame);
	toolbarBox->SetLabel("Toolbars");
	
	float	vertCbSpacing = 25.0f,
			offset = 30.0f,
			adjust = 5.0f,
			bwidth = 80.0f,
			bheight = 30.0f,
			bspacing = 10.0f
			;
	
	BRect checkboxRect = BRect((offset * 2.0f / 3.0f), offset - adjust,(offset * 2.0f/ 3.0f) + 150.0f, offset - adjust + vertCbSpacing - 5.0f);	
	
	BCheckBox *tbInvertCheck = new BCheckBox(checkboxRect, K_IS_TEXTBAR_INVERTED,"Invert Toolbar Colours", GetPrefsMessage(K_IS_TEXTBAR_INVERTED));
	bool isTexBarInverted = false;
	tbInvertCheck->SetValue(isTexBarInverted);
	toolbarBox->AddChild(tbInvertCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);
		
	BCheckBox *tbGreekHiddenCheck = new BCheckBox(checkboxRect, K_IS_GREEK_HIDDEN,"Greek Letters", GetPrefsMessage(K_IS_GREEK_HIDDEN));
	bool isGreekHidden = false;
	tbGreekHiddenCheck->SetValue(isGreekHidden);
	toolbarBox->AddChild(tbGreekHiddenCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);	
	
	BCheckBox *tbBigHiddenCheck = new BCheckBox(checkboxRect, K_IS_BIG_HIDDEN,"Big Operators", GetPrefsMessage(K_IS_BIG_HIDDEN));
	bool isBigHidden = false;
	tbBigHiddenCheck->SetValue(isBigHidden);
	toolbarBox->AddChild(tbBigHiddenCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);
	
	BCheckBox *tbBinaryHiddenCheck = new BCheckBox(checkboxRect, K_IS_BINARY_HIDDEN,"Binary Operators", GetPrefsMessage(K_IS_BINARY_HIDDEN));
	bool isBinaryHidden = false;
	tbBinaryHiddenCheck->SetValue(isBinaryHidden);
	toolbarBox->AddChild(tbBinaryHiddenCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);
	
	BCheckBox *tbMiscHiddenCheck = new BCheckBox(checkboxRect, K_IS_MISC_HIDDEN,"Misc Symbols", GetPrefsMessage(K_IS_MISC_HIDDEN));
	bool isMiscHidden = false;
	tbMiscHiddenCheck->SetValue(isMiscHidden);
	toolbarBox->AddChild(tbMiscHiddenCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);
	
	BCheckBox *tbBinRelHiddenCheck = new BCheckBox(checkboxRect, K_IS_BIN_REL_HIDDEN,"Binary Relations", GetPrefsMessage(K_IS_BIN_REL_HIDDEN));
	bool isBinHidden = false;
	tbBinRelHiddenCheck->SetValue(isBinHidden);
	toolbarBox->AddChild(tbBinRelHiddenCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);
	
	BCheckBox *tbMMAHiddenCheck = new BCheckBox(checkboxRect, K_IS_MMA_HIDDEN,"Math Mode Accents", GetPrefsMessage(K_IS_MMA_HIDDEN));
	bool isMMAHidden = false;
	tbMMAHiddenCheck->SetValue(isMMAHidden);
	toolbarBox->AddChild(tbMMAHiddenCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);
	
	BCheckBox *tbIntlHiddenCheck = new BCheckBox(checkboxRect, K_IS_INT_HIDDEN,"International", GetPrefsMessage(K_IS_INT_HIDDEN));
	bool isIntlHidden = false;
	tbIntlHiddenCheck->SetValue(isIntlHidden);
	toolbarBox->AddChild(tbIntlHiddenCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);
	
	BCheckBox *tbTypeFaceHiddenCheck = new BCheckBox(checkboxRect, K_IS_TYPE_FACE_HIDDEN,"Type Faces", GetPrefsMessage(K_IS_TYPE_FACE_HIDDEN));
	bool isTypeFaceHidden = false;
	tbTypeFaceHiddenCheck->SetValue(isTypeFaceHidden);
	toolbarBox->AddChild(tbTypeFaceHiddenCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);
	
	BCheckBox *tbFunctionHiddenCheck = new BCheckBox(checkboxRect, K_IS_FUNCTION_HIDDEN,"Functions", GetPrefsMessage(K_IS_FUNCTION_HIDDEN));
	bool isFunctionHidden = false;
	tbFunctionHiddenCheck->SetValue(isFunctionHidden);
	toolbarBox->AddChild(tbFunctionHiddenCheck);
	checkboxRect.OffsetBy(0, vertCbSpacing);		

	BRect toolbarRect = toolbarBox->Bounds();
	BRect toolbarbtnRect(toolbarRect.right-bspacing-bwidth,toolbarRect.bottom-bspacing-bheight,toolbarRect.right-bspacing,toolbarRect.bottom-bspacing);
	BButton *resetToolbarDefaults = new BButton(toolbarbtnRect,"resetToolbarDefaults","Defaults", new BMessage(PrefsConstants::K_PREFS_VIEW_RESET_TOOLBAR_DEFAULTS));
	toolbarBox->AddChild(resetToolbarDefaults);
	
	return toolbarBox;
}

void PrefsWindow::ResetToDefaults(uint32 resetFlag)
{
	prefsLock.Lock();
	
	if (resetFlag & PrefsConstants::K_RESET_COLOUR_PREFS == 1)
	{
		preferences.AddData(K_FG_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_BLACK,sizeof(rgb_color));
		preferences.AddData(K_BG_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_WHITE,sizeof(rgb_color));
		preferences.AddData(K_GEN_CMD_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_RED,sizeof(rgb_color));
		preferences.AddData(K_FORMAT_CMD_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_GREEN,sizeof(rgb_color));
		preferences.AddData(K_SPECIAL_CMD_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_PURPLE,sizeof(rgb_color));
		preferences.AddData(K_MATH_MODE_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_NAVY_BLUE,sizeof(rgb_color));
		preferences.AddData(K_COMMA_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_RED,sizeof(rgb_color));
		preferences.AddData(K_COMMENT_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_BLUE,sizeof(rgb_color));
		preferences.AddData(K_PUNC_SYMBOL_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_PURPLE,sizeof(rgb_color));
		//preferences.AddData(K_HTML_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_ORANGE,sizeof(rgb_color));	
		preferences.AddData(K_RGBTEXT_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_BLACK,sizeof(rgb_color));
		preferences.AddData(K_BUBBLE_COLOUR,B_RGB_COLOR_TYPE,&ColourConstants::K_BUBBLE_DEFAULT,sizeof(rgb_color));
	}			
	
	if (resetFlag & PrefsConstants::K_RESET_COMMAND_PREFS == 1)
	{
		preferences.AddString(K_LATEX_CMD, "latex $.tex");
		preferences.AddString(K_DVIPDF_CMD, "dvipdf $.dvi");
		preferences.AddString(K_DVIPS_CMD, "dvips -o $.ps $.dvi");
		preferences.AddString(K_PS2PDF_CMD, "ps2pdf $.ps");
		preferences.AddString(K_PDFLATEX_CMD, "pdflatex $.tex");
		preferences.AddString(K_LATEX2HTML_CMD, "latex2html $.tex");
		preferences.AddString(K_POSTSCRIPT_CMD, "gs -sDEVICE=bealpha4 $.ps");
	}
	
	if (resetFlag & PrefsConstants::K_RESET_TOOLBAR_PREFS == 1)
	{			
		preferences.AddBool(K_IS_GREEK_HIDDEN, false);
		preferences.AddBool(K_IS_BIG_HIDDEN, true);
		preferences.AddBool(K_IS_BINARY_HIDDEN, true);
		preferences.AddBool(K_IS_MISC_HIDDEN, true);
		preferences.AddBool(K_IS_BIN_REL_HIDDEN, true);
		preferences.AddBool(K_IS_MMA_HIDDEN, true);
		preferences.AddBool(K_IS_INT_HIDDEN, true);
		preferences.AddBool(K_IS_TYPE_FACE_HIDDEN, true);
		preferences.AddBool(K_IS_FUNCTION_HIDDEN, true);
		
		preferences.AddBool(K_IS_TEXTBAR_INVERTED, true);
		preferences.AddBool(K_IS_RECENT_DOCS_SHOWN, true);
		preferences.AddBool(K_IS_ACTIVATION_OK, true);
	}	
	
	if (resetFlag & PrefsConstants::K_RESET_GENERAL_PREFS == 1)
	{
		preferences.AddFloat(K_FONT_SIZE, 12.0f);
		preferences.AddBool(K_AUTO_INDENT, true);
		preferences.AddBool(K_IS_SYNTAX, true);
		
		preferences.AddBool(K_IS_CASE_SENSITIVE, false);
		preferences.AddBool(K_IS_WRAP_AROUND, false);
		preferences.AddBool(K_IS_SEARCH_BACKWARDS, false);
		preferences.AddBool(K_IS_ALL_DOCS, false);	
		
		preferences.AddString(K_NUM_RECENT_DOCS, "20");	
		preferences.AddString(K_COLS, "4");
		preferences.AddString(K_ROWS, "4");
		preferences.AddString(K_DATE_FLAGS, "");
	}
		
 	BScreen screen;
	BRect mainWindowRect(0,0,500,500);
	if(screen.IsValid())
	{
		BRect f = screen.Frame();
		mainWindowRect = BRect(f.left+4,f.top+24,f.right-5,f.bottom-5);
	}	
	preferences.AddRect(K_MAIN_WINDOW_RECT, mainWindowRect);
	//???
	preferences.AddMessage(K_SPLIT_MSG, NULL);
	preferences.AddMessage(K_SPLIT_LEFT_MSG, NULL);
	
	prefsLock.Unlock();
}

BMessage* PrefsWindow::GetPrefsMessage(BString prefsID)
{
	BMessage *prefsMessage = new BMessage(PrefsConstants::K_PREFS_UPDATE);
	prefsMessage->AddString(K_PREFS_ID, prefsID);
	return prefsMessage;
}
