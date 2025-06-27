#include "MainTBar.h"

#include "ConverterIcons.h"

using namespace MenuConstants;
using namespace ToolbarConstants;

void
ChurnBitmap(BBitmap* bitmap)
{  // B G R A

	unsigned char* p = (unsigned char*)bitmap->Bits();
	int size = bitmap->BitsLength();

	for (int i = 0; i < size; i += 4) {
		p[i + 3] = (unsigned char)255;

		if ((int)p[i] == 255 && (int)p[i + 1] == 0 && (int)p[i + 2] == 255) {
			p[i + 3] = (unsigned char)0;
		}
	}
}

MainTBar::MainTBar(BRect r, BubbleHelper* h)
	: BView(r, "maintbar", B_FOLLOW_NONE, B_WILL_DRAW)
{
	helper = h;
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	r = Bounds();
	int offset = 34;
	BRect tiler = BRect(1, 1, offset, offset);
	BBitmap* NewDocBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	NewDocBitmap->SetBits(IconNewDoc, 3072, 0, B_CMAP8);
	AddChild(TBNewDoc = new TButton(NULL, tiler, new BMessage(K_MENU_FILE_NEW), NewDocBitmap));
	helper->SetHelp(TBNewDoc, "New");
	tiler.OffsetBy(offset, 0);

	BBitmap* OpenBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	OpenBitmap->SetBits(IconOpen, 3072, 0, B_CMAP8);
	AddChild(TBOpen = new TButton(NULL, tiler, new BMessage(K_MENU_FILE_OPEN), OpenBitmap));
	helper->SetHelp(TBOpen, "Open");
	tiler.OffsetBy(offset, 0);

	BBitmap* SaveBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	SaveBitmap->SetBits(IconSave, 3072, 0, B_CMAP8);
	AddChild(TBSave = new TButton(NULL, tiler, new BMessage(K_MENU_FILE_SAVE), SaveBitmap));
	helper->SetHelp(TBSave, "Save");
	tiler.OffsetBy(offset, 0);

	BBitmap* PrinterBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	PrinterBitmap->SetBits(IconPrintMe, 3072, 0, B_CMAP8);
	AddChild(TBPrint = new TButton(NULL, tiler, new BMessage(K_MENU_FILE_PRINT), PrinterBitmap));
	helper->SetHelp(TBPrint, "Print");
	tiler.OffsetBy(offset, 0);

	BBitmap* DelTmpBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	DelTmpBitmap->SetBits(IconDelTmp, 3072, 0, B_CMAP8);
	AddChild(
		TBDelTmp = new TButton(NULL, tiler, new BMessage(K_CMD_DELETE_TEMP_FILES), DelTmpBitmap));
	helper->SetHelp(TBDelTmp, "Delete Temporary Files");
	tiler.OffsetBy(offset, 0);

	BBitmap* EmptyDelTmpBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	EmptyDelTmpBitmap->SetBits(IconEmptyTmp, 3072, 0, B_CMAP8);
	TBDelTmp->SetAuxBitmap(EmptyDelTmpBitmap);
	TBDelTmp->SetAuxIcon(true);

	BBitmap* ViewLogBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	ViewLogBitmap->SetBits(IconViewLog, 3072, 0, B_CMAP8);
	AddChild(
		TBViewLog = new TButton(NULL, tiler, new BMessage(K_CMD_VIEW_LOG_FILE), ViewLogBitmap));
	helper->SetHelp(TBViewLog, "View Logfile");
	tiler.OffsetBy(offset, 0);

	BBitmap* TrackerBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	TrackerBitmap->SetBits(IconOpenCWDTracker, 3072, 0, B_CMAP8);
	AddChild(TBOpenTracker
			 = new TButton(NULL, tiler, new BMessage(K_CMD_LAUNCH_TRACKER), TrackerBitmap));
	helper->SetHelp(TBOpenTracker, "Open Parent Folder");
	tiler.OffsetBy(offset, 0);

	BBitmap* TerminalBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	TerminalBitmap->SetBits(IconOpenCWDTerminal, 3072, 0, B_CMAP8);
	AddChild(TBOpenTerminal
			 = new TButton(NULL, tiler, new BMessage(K_CMD_LAUNCH_TERMINAL), TerminalBitmap));
	helper->SetHelp(TBOpenTerminal, "Open Terminal in Parent Folder");
	tiler.OffsetBy(offset, 0);

	BBitmap* TexDviBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	TexDviBitmap->SetBits(IconTexToDVI, 3072, 0, B_CMAP8);
	AddChild(TBTexDvi = new TButton(NULL, tiler, new BMessage(K_CMD_COMPILE_TEXDVI), TexDviBitmap));
	helper->SetHelp(TBTexDvi, "Convert TeX to DVI");
	tiler.OffsetBy(offset, 0);

	BBitmap* DviPdfBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	DviPdfBitmap->SetBits(IconDVIToPDF, 3072, 0, B_CMAP8);
	AddChild(TBDviPdf = new TButton(NULL, tiler, new BMessage(K_CMD_COMPILE_DVIPDF), DviPdfBitmap));
	helper->SetHelp(TBDviPdf, "Convert DVI to PDF");
	tiler.OffsetBy(offset, 0);

	BBitmap* DviPsBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	DviPsBitmap->SetBits(IconDVIToPS, 3072, 0, B_CMAP8);
	AddChild(TBDviPs = new TButton(NULL, tiler, new BMessage(K_CMD_COMPILE_DVIPS), DviPsBitmap));
	helper->SetHelp(TBDviPs, "Convert DVI to PS");
	tiler.OffsetBy(offset, 0);

	BBitmap* PsPdfBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	PsPdfBitmap->SetBits(IconPSToPDF, 3072, 0, B_CMAP8);
	AddChild(TBPsPdf = new TButton(NULL, tiler, new BMessage(K_CMD_COMPILE_PSPDF), PsPdfBitmap));
	helper->SetHelp(TBPsPdf, "Convert PS to PDF");
	tiler.OffsetBy(offset, 0);

	BBitmap* TexPdfBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	TexPdfBitmap->SetBits(IconTexToPDF, 3072, 0, B_CMAP8);
	AddChild(TBTexPdf = new TButton(NULL, tiler, new BMessage(K_CMD_COMPILE_TEXPDF), TexPdfBitmap));
	helper->SetHelp(TBTexPdf, "Convert TeX to PDF");
	tiler.OffsetBy(offset, 0);

	BBitmap* TexHtmlBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	TexHtmlBitmap->SetBits(IconTexToHTML, 3072, 0, B_CMAP8);
	AddChild(
		TBTexHtml = new TButton(NULL, tiler, new BMessage(K_CMD_COMPILE_TEXHTML), TexHtmlBitmap));
	helper->SetHelp(TBTexHtml, "Convert TeX to HTML");
	tiler.OffsetBy(offset, 0);

	// Previewers
	/*
	BBitmap* PrevDVIBitmap = new BBitmap(BRect(0,0,31,31),B_CMAP8);
	PrevDVIBitmap->SetBits(IconPrevDVI,3072,0,B_CMAP8);
	AddChild(TBPrevDVI = new TButton(NULL, tiler,new
	BMessage(K_CMD_LAUNCH_DVIVIEWER),PrevDVIBitmap)); helper->SetHelp(TBPrevDVI,"View DVI
	Document"); tiler.OffsetBy(offset,0);
	*/
	BBitmap* PrevPSBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	PrevPSBitmap->SetBits(IconPrevPS, 3072, 0, B_CMAP8);
	AddChild(
		TBPrevPS = new TButton(NULL, tiler, new BMessage(K_CMD_LAUNCH_PSVIEWER), PrevPSBitmap));
	helper->SetHelp(TBPrevPS, "View PS Document");
	tiler.OffsetBy(offset, 0);

	BBitmap* PrevPDFBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	PrevPDFBitmap->SetBits(IconPrevPDF, 3072, 0, B_CMAP8);
	AddChild(
		TBPrevPDF = new TButton(NULL, tiler, new BMessage(K_CMD_LAUNCH_BE_PDF), PrevPDFBitmap));
	helper->SetHelp(TBPrevPDF, "View PDF Document");
	tiler.OffsetBy(offset, 0);

	BBitmap* PrevHTMLBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	PrevHTMLBitmap->SetBits(IconPrevHTML, 3072, 0, B_CMAP8);
	AddChild(TBPrevHTML
			 = new TButton(NULL, tiler, new BMessage(K_CMD_LAUNCH_HTMLVIEWER), PrevHTMLBitmap));
	helper->SetHelp(TBPrevHTML, "View HTML Document");
	tiler.OffsetBy(offset, 0);

	SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_NO_VERTICAL));
	SetExplicitMaxSize(BSize(32 * (CountChildren() + 1), 33));
}
void
MainTBar::Draw(BRect r)
{
	r = Bounds();

	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_2_TINT));
	StrokeLine(BPoint(r.left, r.top), BPoint(r.left, r.bottom));
	StrokeLine(BPoint(r.left, r.top), BPoint(r.right, r.top));

	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	StrokeLine(BPoint(r.left, r.bottom), BPoint(r.right, r.bottom));
	StrokeLine(BPoint(r.right, r.top + 1), BPoint(r.right, r.bottom));
}
