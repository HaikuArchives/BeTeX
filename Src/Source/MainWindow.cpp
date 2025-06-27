#ifndef MAIN_WINDOW_H
#include "MainWindow.h"
#endif

#include <be/app/Application.h>
#include <be/app/Clipboard.h>
#include <be/app/MessageRunner.h>
#include <be/app/Roster.h>
#include <be/interface/Alert.h>
#include <be/interface/Alignment.h>
#include <be/interface/LayoutBuilder.h>
#include <be/interface/Menu.h>
#include <be/interface/PrintJob.h>
#include <be/interface/ScrollBar.h>
#include <be/storage/Directory.h>
#include <be/storage/Entry.h>
#include <be/storage/File.h>
#include <be/storage/Mime.h>
#include <be/storage/Node.h>
#include <be/storage/NodeInfo.h>

#include <unistd.h>
// #include <stdio.h>
using std::cout;
using std::endl;

#include "Constants.h"
#include "ConverterIcons.h"
#include "DocView.h"
#include "MainTBar.h"
#include "MessageFields.h"
#include "Preferences.h"
#include "TexFileFilter.h"
#include "Toolbar.h"
#include "ToolbarButton.h"

using namespace MenuConstants;
using namespace PrefsConstants;
using namespace ColorPrefsConstants;
using namespace SearchWindowConstants;
using namespace InterfaceConstants;
using namespace ToolbarConstants;
using namespace AboutMessages;

MainWindow::MainWindow(BRect frame)
	: BWindow(frame, "BeTeX", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	// Setup our MenuBar
	BMenuBar* menubar = new BMenuBar(frame, "menu_bar");

	CreateMenuBar(menubar);

	float statusBarHeight = 14.0f;
	// Create bubblehelper
	helper = new BubbleHelper(preferences->bubble_color);

	// Create Toolbar
	float menuBarHeight = (menubar->Bounds()).Height();


	BRect toolBarFrame(0.0f, menuBarHeight + 1.0f, frame.Width(), menuBarHeight + 37.0f);
	m_toolBar = new MainTBar(toolBarFrame, helper);

	TLIST_VIEW_WIDTH = 178;
	BRect Trect(0.0f, 0.0f, frame.Width(), frame.Height() - statusBarHeight);

	BRect Textrect = Trect;
	Textrect.InsetBy(10, 10);

	m_texToolBar = new TexBar(
		BRect(0, (frame.Height() / 2) - 99, TLIST_VIEW_WIDTH, frame.Height() - statusBarHeight),
		helper);
	BScrollView* toolbarScroll = new BScrollView(
		"tbscroller", m_texToolBar, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS, false, true);

	DocView* docView = new DocView(Trect);
	docScroll = new BScrollView(
		"docScroll", docView, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS, false, true);

	m_projectView
		= new ProjectView(BRect(0, 0, TLIST_VIEW_WIDTH, frame.Height() / 2 - 100), docScroll);
	BScrollView* projectScroll = new BScrollView(
		"ScrollView", m_projectView, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS, false, true);

	m_verticalSplit = new SplitPane(BRect(0, frame.top, TLIST_VIEW_WIDTH, frame.Height()),
		"vertical", projectScroll, toolbarScroll, B_FOLLOW_ALL_SIDES);

	m_verticalSplit->SetAlignment(B_HORIZONTAL);
	m_verticalSplit->SetEditable(false);
	LEFT_BAR_V_POS = frame.Height() / 4;
	m_verticalSplit->SetBarPosition(LEFT_BAR_V_POS);

	m_horizontalSplit
		= new SplitPane(frame, "horizontal", m_verticalSplit, docScroll, B_FOLLOW_ALL_SIDES);
	m_projectView->SetSplitPane(m_horizontalSplit);

	m_horizontalSplit->SetAlignment(B_VERTICAL);
	m_horizontalSplit->SetEditable(false);
	m_horizontalSplit->SetBarPosition(TLIST_VIEW_WIDTH);

	BRect statusBarFrame(0.0f, frame.bottom - statusBarHeight, frame.Width(), frame.bottom);
	m_statusBar = new StatusBar(statusBarFrame);

	untitled_no = 1;
	// const int UPDATE_TIME = 600000000/2;	// 10/2=5 minutes
	printer_settings = NULL;

	BPath templatePath;
	if (find_directory(B_SYSTEM_VAR_DIRECTORY, &templatePath) == B_OK) {
		templatePath.Append("betex");
		templatePath.Append("templates");
		TemplateDir = templatePath.Path();
	}
	searchPanel = NULL;	 // new SearchWindow(BRect(100,100,250,250),this);
	gtlPanel = NULL;
	aboutPanel = NULL;
	prefsPanel = NULL;
	rgbTxtChooser = NULL;
	dimChooser = NULL;
	RemoveAfterSave = false;

	BMessage recentrefs;
	be_roster->GetRecentDocuments(&recentrefs, 20, TEX_FILETYPE, APP_SIG);

	// Setup SaveFilePanel
	savePanel
		= new BFilePanel(B_SAVE_PANEL, NULL, NULL, B_FILE_NODE, false, NULL, new TexFileFilter());
	savePanel->SetTarget(this);

	// Setup OpenFilePanel
	openPanel
		= new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_FILE_NODE, true, NULL, new TexFileFilter());
	openPanel->SetTarget(this);

	openfolderPanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_DIRECTORY_NODE, true);
	openfolderPanel->SetTarget(this);

	insertfilePanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_FILE_NODE, false,
		new BMessage(MenuConstants::K_MENU_INSERT_FILE_RECEIVED), new TexFileFilter());
	insertfilePanel->SetTarget(this);

	ResetPermissions();
	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_NO_VERTICAL))
		.Add(menubar)
		.Add(m_toolBar)
		.Add(m_horizontalSplit)
		.Add(m_statusBar)
		.End();
	// Update split panes according to preferences
	if (preferences->splitmsg != NULL) {
		Lock();
		m_horizontalSplit->SetState(preferences->splitmsg);
		UnlockLooper();
	} else {
		preferences->splitmsg = m_horizontalSplit->GetState();
	}
	if (preferences->split_leftmsg != NULL) {
		Lock();
		m_verticalSplit->SetState(preferences->split_leftmsg);
		Unlock();
	} else {
		preferences->split_leftmsg = m_verticalSplit->GetState();
	}

	// Resize
	BSize size = GetLayout()->PreferredSize();
	ResizeTo(size.Width(), size.Height());

	m_statusBar->SetText("BeTeX");
}

MainWindow::~MainWindow()
{
	prefsLock.Lock();
	preferences->main_window_rect = Frame();
	preferences->splitmsg = m_horizontalSplit->GetState();
	preferences->split_leftmsg = m_verticalSplit->GetState();
	prefsLock.Unlock();
	// clean up bubblehelper
	if (helper != NULL)
		delete helper;
}

void
MainWindow::CreateMenuBar(BMenuBar* menuBar)
{
	BMenu* fileMenu = new BMenu("File");
	menuBar->AddItem(fileMenu);

	fileMenu->AddItem(
		fnew = new BMenuItem("New", new BMessage(MenuConstants::K_MENU_FILE_NEW), 'N'));
	fileMenu->AddItem(fopen = new BMenuItem(opensubmenu = new BMenu("Open" B_UTF8_ELLIPSIS),
						  new BMessage(MenuConstants::K_MENU_FILE_OPEN)));
	fopen->SetShortcut('O', 0);

	fileMenu->AddItem(fopentemplate = new BMenuItem(
						  fopentemplatesubmenu = new BMenu("Open Template" B_UTF8_ELLIPSIS), NULL));
	fileMenu->AddItem(fopenfolder = new BMenuItem("Open Folder" B_UTF8_ELLIPSIS,
						  new BMessage(MenuConstants::K_MENU_FILE_OPEN_FOLDER)));
	fileMenu->AddItem(
		fclose = new BMenuItem("Close", new BMessage(MenuConstants::K_MENU_FILE_CLOSE), 'W'));
	fileMenu->AddItem(
		fsave = new BMenuItem("Save", new BMessage(MenuConstants::K_MENU_FILE_SAVE), 'S'));
	fileMenu->AddItem(fsaveas = new BMenuItem("Save As" B_UTF8_ELLIPSIS,
						  new BMessage(MenuConstants::K_MENU_FILE_SAVEAS)));
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(fpgsetup = new BMenuItem("Page Setup" B_UTF8_ELLIPSIS,
						  new BMessage(MenuConstants::K_MENU_FILE_PAGE_SETUP)));
	fileMenu->AddItem(fprint = new BMenuItem("Print" B_UTF8_ELLIPSIS,
						  new BMessage(MenuConstants::K_MENU_FILE_PRINT), 'P'));
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(fprefs = new BMenuItem("Preferences" B_UTF8_ELLIPSIS,
						  new BMessage(MenuConstants::K_MENU_FILE_PREFS), ','));
	fileMenu->AddItem(freset_layout = new BMenuItem(
						  "Reset Layout", new BMessage(MenuConstants::K_MENU_FORMAT_RESET_LAYOUT)));
	fileMenu->AddItem(fabout = new BMenuItem("About BeTeX" B_UTF8_ELLIPSIS,
						  new BMessage(MenuConstants::K_MENU_FILE_ABOUT)));
	fileMenu->AddItem(
		fquit = new BMenuItem("Quit", new BMessage(MenuConstants::K_MENU_FILE_QUIT), 'Q'));

	BMenu* editMenu = new BMenu("Edit");
	menuBar->AddItem(editMenu);

	editMenu->AddItem(fundo = new BMenuItem("Undo", new BMessage(B_UNDO), 'Z'));
	editMenu->AddSeparatorItem();
	editMenu->AddItem(fcut = new BMenuItem("Cut", new BMessage(B_CUT), 'X'));
	editMenu->AddItem(fcopy = new BMenuItem("Copy", new BMessage(B_COPY), 'C'));
	editMenu->AddItem(fpaste = new BMenuItem("Paste", new BMessage(B_PASTE), 'V'));
	editMenu->AddSeparatorItem();
	editMenu->AddItem(fselall = new BMenuItem("Select All", new BMessage(B_SELECT_ALL), 'A'));
	editMenu->AddSeparatorItem();
	editMenu->AddItem(fsearchreplace = new BMenuItem("Find" B_UTF8_ELLIPSIS,
						  new BMessage(MenuConstants::K_MENU_EDIT_SEARCH), 'F'));
	editMenu->AddItem(fgotoline = new BMenuItem("Go To Line" B_UTF8_ELLIPSIS,
						  new BMessage(MenuConstants::K_MENU_EDIT_GOTOLINE), 'G'));

	BMenu* formatMenu = new BMenu("Format");
	menuBar->AddItem(formatMenu);
	formatMenu->AddItem(
		fbold = new BMenuItem("Bold", new BMessage(MenuConstants::K_MENU_FORMAT_BOLD), 'B'));
	formatMenu->AddItem(
		femph = new BMenuItem("Emphasized", new BMessage(MenuConstants::K_MENU_FORMAT_EMPH), 'E'));
	formatMenu->AddItem(
		fital = new BMenuItem("Italic", new BMessage(MenuConstants::K_MENU_FORMAT_ITAL), 'I'));
	formatMenu->AddSeparatorItem();
	formatMenu->AddItem(fshiftleft = new BMenuItem("Shift Left",
							new BMessage(MenuConstants::K_MENU_FORMAT_SHIFT_LEFT), '['));
	formatMenu->AddItem(fshiftright = new BMenuItem("Shift Right",
							new BMessage(MenuConstants::K_MENU_FORMAT_SHIFT_RIGHT), ']'));
	formatMenu->AddSeparatorItem();
	formatMenu->AddItem(fcomment = new BMenuItem("Comment Selection",
							new BMessage(MenuConstants::K_MENU_FORMAT_COMMENTLINE)));
	formatMenu->AddItem(funcomment = new BMenuItem("UnComment Selection",
							new BMessage(MenuConstants::K_MENU_FORMAT_UNCOMMENTLINE)));

	BMenu* insertMenu = new BMenu("Insert");
	menuBar->AddItem(insertMenu);
	insertMenu->AddItem(finsertfile = new BMenuItem("File" B_UTF8_ELLIPSIS,
							new BMessage(MenuConstants::K_MENU_INSERT_FILE)));
	insertMenu->AddItem(
		fdate = new BMenuItem("Date", new BMessage(MenuConstants::K_MENU_INSERT_DATE)));
	insertMenu->AddItem(
		farray = new BMenuItem("Array", new BMessage(MenuConstants::K_MENU_INSERT_ARRAY)));
	insertMenu->AddItem(
		fmatrix = new BMenuItem("Matrix", new BMessage(MenuConstants::K_MENU_INSERT_MATRIX)));
	insertMenu->AddItem(
		ftabular = new BMenuItem("Tabular", new BMessage(MenuConstants::K_MENU_INSERT_TABULAR)));
	insertMenu->AddItem(
		fequation = new BMenuItem("Equation", new BMessage(MenuConstants::K_MENU_INSERT_EQUATION)));
	insertMenu->AddItem(frgbcolor = new BMenuItem("Colored Text",
							new BMessage(MenuConstants::K_MENU_INSERT_COLORED_TEXT)));

	insertMenu->AddItem(flists = new BMenuItem(flistsubmenu = new BMenu("Lists"), NULL));
	flistsubmenu->AddItem(
		fitemize = new BMenuItem("Itemize", new BMessage(MenuConstants::K_MENU_INSERT_ITEMIZE)));
	flistsubmenu->AddItem(fdescription = new BMenuItem("Description",
							  new BMessage(MenuConstants::K_MENU_INSERT_DESCRIPTION)));
	flistsubmenu->AddItem(fenumerate = new BMenuItem(
							  "Enumerate", new BMessage(MenuConstants::K_MENU_INSERT_ENUMERATE)));

	insertMenu->AddItem(
		fenvironments = new BMenuItem(fenvironmentssubmenu = new BMenu("Environments"), NULL));
	fenvironmentssubmenu->AddItem(
		ffigure = new BMenuItem("Figure", new BMessage(MenuConstants::K_MENU_INSERT_FIGURE)));
	fenvironmentssubmenu->AddItem(
		ftable = new BMenuItem("Table", new BMessage(MenuConstants::K_MENU_INSERT_TABLE)));
	fenvironmentssubmenu->AddSeparatorItem();
	fenvironmentssubmenu->AddItem(
		fcenter = new BMenuItem("Center", new BMessage(MenuConstants::K_MENU_INSERT_CENTER)));
	fenvironmentssubmenu->AddItem(fflushleft = new BMenuItem("FlushLeft",
									  new BMessage(MenuConstants::K_MENU_INSERT_FLUSHLEFT)));
	fenvironmentssubmenu->AddItem(fflushright = new BMenuItem("FlushRight",
									  new BMessage(MenuConstants::K_MENU_INSERT_FLUSHRIGHT)));
	fenvironmentssubmenu->AddSeparatorItem();
	fenvironmentssubmenu->AddItem(
		feqnarray = new BMenuItem("EqnArray", new BMessage(MenuConstants::K_MENU_INSERT_EQNARRAY)));
	fenvironmentssubmenu->AddItem(ffeqnarraystar = new BMenuItem("EqnArray*",
									  new BMessage(MenuConstants::K_MENU_INSERT_EQNARRAY_STAR)));
	fenvironmentssubmenu->AddSeparatorItem();
	fenvironmentssubmenu->AddItem(
		fverbatim = new BMenuItem("Verbatim", new BMessage(MenuConstants::K_MENU_INSERT_VERBATIM)));
	fenvironmentssubmenu->AddItem(
		fquote = new BMenuItem("Quote", new BMessage(MenuConstants::K_MENU_INSERT_QUOTE)));

	BMenu* compileMenu = new BMenu("Compile");
	menuBar->AddItem(compileMenu);
	compileMenu->AddItem(ftexdvi = new BMenuItem("tex->dvi",
							 new BMessage(ToolbarConstants::K_CMD_COMPILE_TEXDVI), '1'));
	compileMenu->AddItem(fdvipdf = new BMenuItem("dvi->pdf",
							 new BMessage(ToolbarConstants::K_CMD_COMPILE_DVIPDF), '2'));
	compileMenu->AddItem(fdvips = new BMenuItem(
							 "dvi->ps", new BMessage(ToolbarConstants::K_CMD_COMPILE_DVIPS), '3'));
	compileMenu->AddItem(fpspdf = new BMenuItem(
							 "ps->pdf", new BMessage(ToolbarConstants::K_CMD_COMPILE_PSPDF), '4'));
	compileMenu->AddItem(ftexpdf = new BMenuItem("tex->pdf",
							 new BMessage(ToolbarConstants::K_CMD_COMPILE_TEXPDF), '5'));
	compileMenu->AddItem(ftexhtml = new BMenuItem("tex->html",
							 new BMessage(ToolbarConstants::K_CMD_COMPILE_TEXHTML), '6'));

	BMenu* previewMenu = new BMenu("Preview");
	menuBar->AddItem(previewMenu);
	previewMenu->AddItem(fpostscript = new BMenuItem("Postscript" B_UTF8_ELLIPSIS,
							 new BMessage(ToolbarConstants::K_CMD_LAUNCH_PSVIEWER), '7'));
	previewMenu->AddItem(fpdf = new BMenuItem("Pdf" B_UTF8_ELLIPSIS,
							 new BMessage(ToolbarConstants::K_CMD_LAUNCH_BE_PDF), '8'));
	previewMenu->AddItem(fhtml = new BMenuItem("Html" B_UTF8_ELLIPSIS,
							 new BMessage(ToolbarConstants::K_CMD_LAUNCH_HTMLVIEWER), '9'));
}

void
MainWindow::InsertText(const char* text)
{
	TexView* tv = CurrentTexView();
	if (tv != NULL) {
		BString cmd(text);
		int lbrk, rbrk;
		lbrk = cmd.FindLast('{');
		rbrk = cmd.FindLast('}');
		bool InsertTextIntoBrk = (lbrk >= 0 && rbrk >= 0 && rbrk - lbrk == 1);
		int32 start, finish;
		tv->GetSelection(&start, &finish);
		if (start != finish) {
			if (InsertTextIntoBrk) {
				BString leftstr;
				cmd.CopyInto(leftstr, 0, lbrk + 1);
				BString rightstr;
				cmd.CopyInto(rightstr, rbrk, cmd.Length() - rbrk);

				tv->Insert(start, leftstr.String(), leftstr.Length(), NULL);
				tv->Insert(finish + leftstr.Length(), rightstr.String(), rightstr.Length(), NULL);
				tv->Select(finish + rightstr.Length() + leftstr.Length(),
					finish + rightstr.Length() + leftstr.Length());
			} else {
				tv->Delete(start, finish);
				tv->Insert(start, cmd.String(), cmd.Length(), NULL);
				tv->Select(start + cmd.Length(), start + cmd.Length());
			}
		} else {
			tv->Insert(start, cmd.String(), cmd.Length(), NULL);
			if (InsertTextIntoBrk)
				tv->Select(start + cmd.Length() - 1, start + cmd.Length() - 1);
			else
				tv->Select(start + cmd.Length(), start + cmd.Length());
		}
		tv->ScrollToSelection();
	}
}

void
MainWindow::UpdateStatusBar()
{
	int current = m_projectView->CurrentSelection();
	if (m_projectView->CountItems() == 0 || current < 0)
		m_statusBar->SetText("BeTeX");

	else {
		if (current >= 0) {
			TexView* tv = CurrentTexView();
			ProjectItem* item = CurrentTListItem();

			if (tv == NULL || item == NULL)
				return;

			BString displayme;

			int32 offset, finish;
			tv->GetSelection(&offset, &finish);

			vector<int> sols;
			vector<int> eols;
			tv->FillSolEol(sols, eols, 0, tv->TextLength() - 1);
			int line = sols.size();
			for (int i = sols.size() - 1; i >= 0; i--) {
				if (offset >= sols[i])	// && offset <= eols[i])
				{
					line = i;
					break;
				}
			}
			if (line < 0)
				displayme << item->Label();	 // was FName()
			else {
				int position = offset - sols[line];
				line++;
				position++;
				// last lines don't display........ :P
				displayme << item->Label() << " [" << (int)line << "," << (int)position << "]";
			}
			m_statusBar->SetText(displayme.String());
		}
	}
}

ProjectView*
MainWindow::GetProjectView()
{
	return m_projectView;
}

TexView*
MainWindow::CurrentTexView()
{
	int current = m_projectView->CurrentSelection();
	if (current >= 0) {
		TexView* tv;
		ProjectItem* item;
		item = (ProjectItem*)m_projectView->ItemAt(current);
		tv = (TexView*)(item->ChildView());
		return tv;
	}
	return NULL;
}

ProjectItem*
MainWindow::CurrentTListItem()
{
	int current = m_projectView->CurrentSelection();
	if (current >= 0) {
		ProjectItem* item;
		item = (ProjectItem*)m_projectView->ItemAt(current);
		return item;
	}
	return NULL;
}

// rewrite this function completely!
void
MainWindow::Execute(const char* script, const char* cmd)
{
	int current = m_projectView->CurrentSelection();
	if (current >= 0) {
		//$ Terminal /boot/beos/bin/sh -c "echo crap"
		ProjectItem* li = (ProjectItem*)m_projectView->ItemAt(current);
		entry_ref ref = li->GetRef();
		BPath path(&ref);
		if (path.InitCheck() == B_OK) {
			BPath parent;
			path.GetParent(&parent);

			BString initial;
			initial << "#!/bin/sh\n" << "cd \"" << parent.Path() << "\"\n";
			BString command;
			command << cmd << "\n";
			BString fname;
			fname << path.Leaf();
			fname.Replace(".tex", "", 1);
			command.ReplaceAll("$", fname.String());

			BString echostr;
			echostr << "echo \"\\$ " << "TEXMFOUTPUT=/tmp " << command.String() << "\"";
			echostr.ReplaceAll("\n", "");
			echostr << "\n";

			BString end;
			end << "echo \"(Press Enter To Continue)\"\nread\nexit 0\n";

			BFile file(script, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);

			if (file.InitCheck() == B_OK) {
				file.Write(initial.String(), initial.Length());
				file.Write(echostr.String(), echostr.Length());
				file.Write(command.String(), command.Length());
				file.Write(end.String(), end.Length());
				file.SetPermissions(X_OK);
				file.Unset();
				const char* args[] = {script, NULL};
				// TODO : for BeOS R5, the signature is x-vnd.Be-SHEL, how to deal both Haiku and
				// BeOS ?
				be_roster->Launch("application/x-vnd.Haiku-Terminal", 1, args);
			}
		}
	}
	ResetPermissions();
}

void
MainWindow::ResetPermissions()
{
	TexView* tv = CurrentTexView();
	ProjectItem* li = CurrentTListItem();
	// replace by loops where possible!
	if (tv == NULL) {
		// disable those things not used......
		// edit items
		fclose->SetEnabled(false);
		fsave->SetEnabled(false);
		fsaveas->SetEnabled(false);
		fpgsetup->SetEnabled(false);
		fprint->SetEnabled(false);
		// edit items
		fundo->SetEnabled(false);
		// fredo->SetEnabled(false);

		fbold->SetEnabled(false);
		femph->SetEnabled(false);
		fital->SetEnabled(false);
		fshiftleft->SetEnabled(false);
		fshiftright->SetEnabled(false);


		fcut->SetEnabled(false);
		fcopy->SetEnabled(false);
		fpaste->SetEnabled(false);
		fselall->SetEnabled(false);
		fsearchreplace->SetEnabled(false);
		fgotoline->SetEnabled(false);
		fcomment->SetEnabled(false);
		funcomment->SetEnabled(false);

		ftexdvi->SetEnabled(false);
		fdvipdf->SetEnabled(false);
		fdvips->SetEnabled(false);
		fpspdf->SetEnabled(false);
		ftexpdf->SetEnabled(false);
		ftexhtml->SetEnabled(false);

		fpostscript->SetEnabled(false);
		fpdf->SetEnabled(false);
		fhtml->SetEnabled(false);

		m_toolBar->TBSave->SetEnabled(false);
		m_toolBar->TBPrint->SetEnabled(false);
		// m_toolBar->TBDelTmp->SetEnabled(false);
		m_toolBar->TBViewLog->SetEnabled(false);
		m_toolBar->TBTexDvi->SetEnabled(false);
		m_toolBar->TBDviPdf->SetEnabled(false);
		m_toolBar->TBDviPs->SetEnabled(false);
		m_toolBar->TBPsPdf->SetEnabled(false);
		m_toolBar->TBTexDvi->SetEnabled(false);
		m_toolBar->TBTexPdf->SetEnabled(false);
		m_toolBar->TBTexHtml->SetEnabled(false);
		m_toolBar->TBOpenTracker->SetEnabled(false);
		m_toolBar->TBOpenTerminal->SetEnabled(false);
		m_toolBar->TBPrevPDF->SetEnabled(false);
		m_toolBar->TBPrevPS->SetEnabled(false);
		m_toolBar->TBPrevHTML->SetEnabled(false);
		// m_toolBar->TBDelTmp->SetAuxIcon(true);


		finsertfile->SetEnabled(false);
		fdate->SetEnabled(false);
		farray->SetEnabled(false);
		fmatrix->SetEnabled(false);
		ftabular->SetEnabled(false);
		fequation->SetEnabled(false);
		frgbcolor->SetEnabled(false);
		flists->SetEnabled(false);
		flistsubmenu->SetEnabled(false);
		/*
		   fitemize->SetEnabled(false);
		   fdescription->SetEnabled(false);
		   fenumerate->SetEnabled(false);
		   */
		// fcases->SetEnabled(false);
		fenvironments->SetEnabled(false);
		fenvironmentssubmenu->SetEnabled(false);
		/*ffigure->SetEnabled(false);
		  ftable->SetEnabled(false);
		//fetabular->SetEnabled(false);
		//fetabularstar->SetEnabled(false);
		//ftabbing->SetEnabled(false);
		fcenter->SetEnabled(false);
		fflushleft->SetEnabled(false);
		fflushright->SetEnabled(false);
		feqnarray->SetEnabled(false);
		ffeqnarraystar->SetEnabled(false);
		fquote->SetEnabled(false);
		//fminipage->SetEnabled(false);
		*/
	} else {
		bool DoesPdfExist = false;
		bool DoesPsExist = false;
		bool DoesDviExist = false;
		bool DoesHtmlExist = false;
		bool DoesLogExist = false;
		bool DoesTexExist = li->IsHomely();

		bool IsSomeTextPasteable = false;

		bool IsSaveNeeded = li->IsSaveNeeded() || !DoesTexExist;

		bool IsNonZeroText = tv->TextLength() > 0;
		int32 start, finish;
		tv->GetSelection(&start, &finish);
		bool IsSomeTextSelected = (start != finish);

		BPath parent;
		entry_ref ref = li->GetRef();
		TexPath.SetTo(&ref);
		if (TexPath.InitCheck() == B_OK) {
			basename = "";
			basename << TexPath.Leaf();
			basename.Replace(".tex", "", 1);


			TexPath.GetParent(&parent);
			parent_dir = parent;
		}

		if (DoesTexExist) {
			BString pdf_path;
			BString dvi_path;
			BString ps_path;
			BString html_path;
			BString log_path;

			pdf_path << TexPath.Path();
			pdf_path.Replace(".tex", ".pdf", 1);

			log_path << TexPath.Path();
			log_path.Replace(".tex", ".log", 1);

			ps_path << TexPath.Path();
			ps_path.Replace(".tex", ".ps", 1);
			dvi_path << TexPath.Path();
			dvi_path.Replace(".tex", ".dvi", 1);
			html_path << parent_dir.Path() << "/" << basename.String() << "/" << basename.String()
					  << ".html";

			BEntry log_entry(log_path.String());
			BEntry pdf_entry(pdf_path.String());
			BEntry ps_entry(ps_path.String());
			BEntry dvi_entry(dvi_path.String());
			BEntry html_entry(html_path.String());

			DoesPdfExist = pdf_entry.Exists();
			DoesPsExist = ps_entry.Exists();
			DoesDviExist = dvi_entry.Exists();
			DoesHtmlExist = html_entry.Exists();
			DoesLogExist = log_entry.Exists();
		}
		// Main Toolbar Buttons

		m_toolBar->TBPrint->SetEnabled(IsNonZeroText);
		m_toolBar->TBSave->SetEnabled(IsSaveNeeded);

		m_toolBar->TBViewLog->SetEnabled(DoesLogExist);
		m_toolBar->TBTexDvi->SetEnabled(DoesTexExist);
		m_toolBar->TBDviPdf->SetEnabled(DoesDviExist);
		m_toolBar->TBDviPs->SetEnabled(DoesDviExist);
		m_toolBar->TBPsPdf->SetEnabled(DoesPsExist);
		m_toolBar->TBTexDvi->SetEnabled(DoesTexExist);
		m_toolBar->TBTexPdf->SetEnabled(DoesTexExist);
		m_toolBar->TBTexHtml->SetEnabled(DoesTexExist);

		m_toolBar->TBOpenTracker->SetEnabled(DoesTexExist);
		m_toolBar->TBOpenTerminal->SetEnabled(DoesTexExist);
		m_toolBar->TBPrevPDF->SetEnabled(DoesPdfExist);
		m_toolBar->TBPrevPS->SetEnabled(DoesPsExist);
		m_toolBar->TBPrevHTML->SetEnabled(DoesHtmlExist);

		// Menu Items
		fprint->SetEnabled(IsNonZeroText);
		fpgsetup->SetEnabled(IsNonZeroText);
		fsave->SetEnabled(IsSaveNeeded);
		fsaveas->SetEnabled(true);	// IsSaveNeeded);

		fclose->SetEnabled(true);
		fbold->SetEnabled(true);
		femph->SetEnabled(true);
		fital->SetEnabled(true);


		finsertfile->SetEnabled(true);
		fdate->SetEnabled(true);
		farray->SetEnabled(true);
		fmatrix->SetEnabled(true);
		ftabular->SetEnabled(true);
		fequation->SetEnabled(true);
		frgbcolor->SetEnabled(true);
		flists->SetEnabled(true);
		flistsubmenu->SetEnabled(true);
		/*
		   fitemize->SetEnabled(true);
		   fdescription->SetEnabled(true);
		   fenumerate->SetEnabled(true);
		   */
		// fcases->SetEnabled(true);
		fenvironments->SetEnabled(true);
		fenvironmentssubmenu->SetEnabled(true);

		// always on
		fundo->SetEnabled(true);
		// fredo->SetEnabled(false);

		fcut->SetEnabled(IsSomeTextSelected);
		fcopy->SetEnabled(IsSomeTextSelected);

		fshiftleft->SetEnabled(true);	// IsSomeTextSelected);
		fshiftright->SetEnabled(true);	// IsSomeTextSelected);


		fselall->SetEnabled(!IsSomeTextSelected);
		fcomment->SetEnabled(IsSomeTextSelected);
		funcomment->SetEnabled(IsSomeTextSelected);

		fsearchreplace->SetEnabled(true);
		fgotoline->SetEnabled(true);

		BMessage* clip = NULL;


		// should do some watching.....
		if (be_clipboard->Lock()) {
			IsSomeTextPasteable = ((clip = be_clipboard->Data()));	// if((clip = be_clip
			be_clipboard->Unlock();
		}

		fpaste->SetEnabled(IsSomeTextPasteable);

		ftexdvi->SetEnabled(DoesTexExist);
		fdvipdf->SetEnabled(DoesDviExist);
		fdvips->SetEnabled(DoesDviExist);
		fpspdf->SetEnabled(DoesPsExist);
		ftexpdf->SetEnabled(DoesTexExist);
		ftexhtml->SetEnabled(DoesTexExist);

		fpostscript->SetEnabled(DoesPsExist);
		fpdf->SetEnabled(DoesPdfExist);
		fhtml->SetEnabled(DoesHtmlExist);


		// Setup some node monitoring......
		if (TexPath.InitCheck() == B_OK && DoesTexExist) {
			bool IsAlreadyWatched = false;
			for (uint i = 0; i < watchedPaths.size(); i++) {
				if (parent == watchedPaths[i]) {
					IsAlreadyWatched = true;
					break;
				}
			}


			if (!IsAlreadyWatched) {
				watchedPaths.push_back(parent);

				node_ref nref;
				BEntry parent_entry(parent.Path());
				if (parent_entry.InitCheck() == B_OK) {
					parent_entry.GetNodeRef(&nref);
					BMessenger msgr(this);

					if (watch_node(&nref, B_WATCH_DIRECTORY, msgr) != B_OK)
						cout << "err watching: " << parent.Path() << endl;
				}
				node_ref nref_latex2htmlfolder;
				BString latex2html_dir;
				latex2html_dir << parent_dir.Path() << "/" << basename;
				BEntry latex2html_entry(latex2html_dir.String());
				if (latex2html_entry.InitCheck() == B_OK) {
					latex2html_entry.GetNodeRef(&nref_latex2htmlfolder);
					BMessenger msgr(this);
					if (watch_node(&nref_latex2htmlfolder, B_WATCH_DIRECTORY, msgr) != B_OK)
						cout << "err watching latex2html folder" << endl;
					//&& watch_node(&nref_latex2htmlfolder,B_WATCH_DIRECTORY,msgr) == B_OK)
				}
			}
		}
	}
}

void
MainWindow::SetShortcuts()
{
	int current = m_projectView->CurrentSelection();
	if (current >= 0) {
		ProjectItem* li = (ProjectItem*)m_projectView->ItemAt(current);
		fcut->SetTarget(li->ChildView());
		fcopy->SetTarget(li->ChildView());
		fpaste->SetTarget(li->ChildView());
		fselall->SetTarget(li->ChildView());
		fundo->SetTarget(li->ChildView());
	}
}

// replace by multiple handlers!!!
void
MainWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case K_MENU_FORMAT_RESET_LAYOUT:
		{
			m_verticalSplit->SetAlignment(B_HORIZONTAL);
			m_verticalSplit->SetEditable(false);
			m_verticalSplit->SetBarPosition(LEFT_BAR_V_POS);
			m_horizontalSplit->SetAlignment(B_VERTICAL);
			m_horizontalSplit->SetEditable(false);
			m_horizontalSplit->SetBarPosition(TLIST_VIEW_WIDTH);
			preferences->splitmsg = m_horizontalSplit->GetState();
			preferences->split_leftmsg = m_verticalSplit->GetState();
		} break;
		case K_MENU_INSERT_DATE:
		{
			TexView* tv = CurrentTexView();
			if (tv != NULL) {
				// Avert your eyes! This isn't nice code :-)
				int MAX_BUF = 100;
				BString cmd = "";
				cmd << "date " << preferences->dateFlags.String();
				FILE* ptr = popen(cmd.String(), "r");
				char buf[MAX_BUF + 1];
				int size;
				memset(buf, '\0', sizeof(buf));
				if (ptr != NULL) {
					size = fread(buf, sizeof(char), MAX_BUF, ptr);
					while (size > 0) {
						buf[size - 1] = '\0';
						size = fread(buf, sizeof(char), MAX_BUF, ptr);
					}
					pclose(ptr);
				}
				InsertText(buf);
			}
		} break;
		case K_MENU_INSERT_FILE_RECEIVED:
		{
			// open file, get contents, insert contents
			TexView* tv = CurrentTexView();
			if (tv != NULL) {
				entry_ref ref;
				if (message->FindRef("refs", &ref) == B_OK) {
					BFile file(&ref, B_READ_ONLY);
					if (file.InitCheck() == B_OK) {
						off_t size;
						if (file.GetSize(&size) == B_OK) {
							char buf[size + 1];
							file.Read(buf, size);
							buf[size] = '\0';
							InsertText(buf);
						}
					}
				}
			}
		} break;
		case K_MENU_INSERT_FILE:
		{
			insertfilePanel->Show();

		} break;
		case K_MENU_INSERT_ARRAY:
		{
			// launch color chooser window
			if (dimChooser != NULL) {
				dimChooser->Activate(true);
				break;
			}

			BPoint p;
			uint32 buttons;
			ChildAt(0)->GetMouse(&p, &buttons);
			ChildAt(0)->ConvertToScreen(&p);
			p.x -= 30;
			p.y -= 30;
			float w = 150, h = 115;
			dimChooser = new DimensionWindow(BRect(p.x, p.y, p.x + w, p.y + h),
				new BMessenger(this), "10", "10", MenuConstants::K_MENU_INSERT_ARRAY_WITHDIM);
			dimChooser->Show();
			dimChooser->CenterOnScreen();
		} break;
		case K_MENU_INSERT_ARRAY_WITHDIM:
		{
			int32 rows = atoi(message->GetString(K_ROWS, "10"));
			int32 cols = atoi(message->GetString(K_COLS, "10"));

			BString insert = "\\begin{array}{";
			int i;
			int j;
			for (i = 0; i < cols; i++) {
				insert << "c";
			}
			insert << "}\n";
			for (j = 0; j < rows; j++) {
				for (i = 0; i < cols; i++) {
					if (i + 1 != cols)
						insert << "* & ";
					else
						insert << "* \\\\\n";
				}
			}
			insert << "\\end{array}\n";
			InsertText(insert.String());
		} break;
		case K_MENU_INSERT_MATRIX:
		{
			// launch color chooser window
			if (dimChooser != NULL) {
				dimChooser->Activate(true);
				break;
			}
			BPoint p;
			uint32 buttons;
			ChildAt(0)->GetMouse(&p, &buttons);
			ChildAt(0)->ConvertToScreen(&p);
			p.x -= 30;
			p.y -= 30;
			float w = 150, h = 115;
			dimChooser = new DimensionWindow(BRect(p.x, p.y, p.x + w, p.y + h),
				new BMessenger(this), "10", "10", MenuConstants::K_MENU_INSERT_MATRIX_WITHDIM);
			dimChooser->Show();
			dimChooser->CenterOnScreen();
		} break;
		case K_MENU_INSERT_MATRIX_WITHDIM:
		{
			int32 rows = atoi(message->GetString(K_ROWS, "10"));
			int32 cols = atoi(message->GetString(K_COLS, "10"));

			BString insert = "\\left(\n\\begin{array}{";
			int i;
			int j;
			for (i = 0; i < cols; i++) {
				insert << "c";
			}
			insert << "}\n";
			for (j = 0; j < rows; j++) {
				for (i = 0; i < cols; i++) {
					if (i + 1 != cols)
						insert << "* & ";
					else
						insert << "* \\\\\n";
				}
			}
			insert << "\\end{array}\n\\right)\n";
			InsertText(insert.String());
		} break;
		case K_MENU_INSERT_TABULAR:
		{
			// launch color chooser window
			if (dimChooser != NULL) {
				dimChooser->Activate(true);
				break;
			}

			BPoint p;
			uint32 buttons;
			ChildAt(0)->GetMouse(&p, &buttons);
			ChildAt(0)->ConvertToScreen(&p);
			p.x -= 30;
			p.y -= 30;
			float w = 150, h = 115;
			dimChooser = new DimensionWindow(BRect(p.x, p.y, p.x + w, p.y + h),
				new BMessenger(this), "10", "10", MenuConstants::K_MENU_INSERT_TABULAR_WITHDIM);

			dimChooser->Show();
			dimChooser->CenterOnScreen();
		} break;
		case K_MENU_INSERT_TABULAR_WITHDIM:
		{
			int32 rows = atoi(message->GetString(K_ROWS, "10"));
			int32 cols = atoi(message->GetString(K_COLS, "10"));

			BString insert = "\\begin{tabular}{|";
			int i;
			int j;
			for (i = 0; i < cols; i++) {
				insert << "c|";
			}
			insert << "}\n\\hline\n";
			for (j = 0; j < rows; j++) {
				if (j == 1)
					insert << "\\hline\n";
				for (i = 0; i < cols; i++) {
					if (i + 1 != cols)
						insert << "* & ";
					else
						insert << "* \\\\\n";
				}
			}
			insert << "\\hline\n\\end{tabular}\n";
			InsertText(insert.String());
		} break;
		case K_DIM_WINDOW_QUIT:
		{
			dimChooser = NULL;

		} break;
		case K_MENU_INSERT_EQNARRAY:
		{
			InsertText("\\begin{eqnarray}\n\t* & = & * \\\\\n\t* & = & * \n\\end{eqnarray}\n");
		} break;
		case K_MENU_INSERT_EQNARRAY_STAR:
		{
			InsertText("\\begin{eqnarray*}\n\t* & = & * \\\\\n\t* & = & * \n\\end{eqnarray*}\n");
		} break;
		case K_MENU_INSERT_VERBATIM:
		{
			InsertText("\\begin{verbatim}\n\n\\end{verbatim}\n");
		} break;
		case K_MENU_INSERT_QUOTE:
		{
			InsertText("\\begin{quote}\n\n\\end{quote}\n");
		} break;
		case K_MENU_INSERT_TABLE:
		{
			//%Insert Tabular Environment Here\n\t
			InsertText(
				"\\begin{table}\n\t\\centering\n\t\\caption{*}\n\t\\label{*}\n\\end{table}\n");
		} break;
		case K_MENU_INSERT_FIGURE:
		{
			InsertText(
				"\\begin{figure}\n\t\\includegraphics[*]{*}\n\t\\caption{*}\n\t\\label{*}\n\\end{"
				"figure}\n");

		} break;
		case K_MENU_INSERT_CENTER:
		{
			InsertText("\\begin{center}\n\n\\end{center}\n");

		} break;
		case K_MENU_INSERT_FLUSHLEFT:
		{
			InsertText("\\begin{flushleft}\n\n\\end{flushleft}\n");

		} break;
		case K_MENU_INSERT_FLUSHRIGHT:
		{
			InsertText("\\begin{flushright}\n\n\\end{flushright}\n");

		} break;
		case K_MENU_INSERT_ITEMIZE:
		{
			//\begin{itemize}
			//  \item *
			//  \item *
			//\end{itemize}

			InsertText("\\begin{itemize}\n\t\\item *\n\t\\item *\n\\end{itemize}\n");
		} break;
		case K_MENU_INSERT_DESCRIPTION:
		{
			/*\begin{description}
			  \item[*] *
			  \item[*] *
			  \end{description}
			  */
			InsertText("\\begin{description}\n\t\\item[*] *\n\t\\item[*] *\n\\end{description}\n");
		} break;
		case K_MENU_INSERT_ENUMERATE:
		{
			//\begin{itemize}
			//  \item *
			//  \item *
			//\end{itemize}
			InsertText("\\begin{enumerate}\n\t\\item *\n\t\\item *\n\\end{enumerate}\n");
		} break;
		case K_MENU_INSERT_EQUATION:
		{
			InsertText("\\begin{equation}\\label{*}\n\t*\n\\end{equation}\n");
		} break;
		case K_MENU_INSERT_COLORED_TEXT:
		{
			if (CurrentTListItem() != NULL) {
				// launch color chooser window
				if (rgbTxtChooser != NULL) {
					rgbTxtChooser->Activate(true);
					break;
				}

				BPoint p;
				uint32 buttons;
				ChildAt(0)->GetMouse(&p, &buttons);
				ChildAt(0)->ConvertToScreen(&p);
				p.x -= 30;
				p.y -= 30;
				float w = 350, h = 115;
				rgbTxtChooser = new ColorWindow(BRect(p.x, p.y, p.x + w, p.y + h),
					new BMessenger(this), preferences->RGBText_color);

				rgbTxtChooser->Show();
			}
		} break;
		case K_COLOR_WINDOW_QUIT:
		{
			rgbTxtChooser = NULL;

		} break;
		case K_MENU_FORMAT_SHIFT_RIGHT:
		{
			TexView* tv = CurrentTexView();
			if (tv != NULL) {
				tv->ShiftRight();
			}

		} break;
		case K_MENU_FORMAT_SHIFT_LEFT:
		{
			TexView* tv = CurrentTexView();
			if (tv != NULL) {
				tv->ShiftLeft();
			}

		} break;
		case B_NODE_MONITOR:
		{
			ResetPermissions();
		} break;
		case K_MENU_FORMAT_BOLD:
		{
			InsertText("\\textbf{}");
		} break;
		case K_MENU_FORMAT_EMPH:
		{
			InsertText("\\emph{}");
		} break;
		case K_MENU_FORMAT_ITAL:
		{
			InsertText("\\textit{}");
		} break;
		case K_CMD_DELETE_TEMP_FILES:
		{
			ProjectItem* item = CurrentTListItem();
			if (item != NULL) {
				entry_ref ref = item->GetRef();
				BPath path(&ref);
				if (path.InitCheck() == B_OK) {
					BPath parent;
					path.GetParent(&parent);
					const char* extensions[] = {".log"};
					int num_ext = 1;
					BString basename(path.Leaf());
					basename.Replace(".tex", "", 1);
					for (int i = 0; i < num_ext; i++) {
						BString del_me;
						del_me << parent.Path() << "/" << basename.String() << extensions[i];
						BEntry entry(del_me.String());
						if (entry.InitCheck() == B_OK) {
							entry.Remove();
						}
					}
				}
			}


		} break;
		case K_CMD_VIEW_LOG_FILE:
		{
			ProjectItem* item = CurrentTListItem();
			if (item != NULL) {
				entry_ref ref = item->GetRef();
				BPath path(&ref);
				if (path.InitCheck() == B_OK) {
					BString logpath;
					logpath << path.Path();
					logpath.Replace(".tex", ".log", 1);

					char* arg = (char*)logpath.String();
					be_roster->Launch("application/x-vnd.Be-STEE", 1, &arg);
				}
			}

		} break;
		case K_UPDATE_CLIPBOARD_MENU_STATUS:
		{
			ResetPermissions();

		} break;
		case K_MENU_FILE_PAGE_SETUP:
		{
			ProjectItem* it = CurrentTListItem();
			if (it != NULL)
				PageSetup(it->Label());
		} break;
		case K_MENU_FILE_PRINT:
		{
			Print();
		} break;
		case K_MENU_FORMAT_COMMENTLINE:
		{
			// still one line too many commented/uncommented if newline is included at end of
			// selection
			int32 start, finish;
			TexView* tv = CurrentTexView();
			if (tv != NULL) {
				tv->GetSelection(&start, &finish);
				if (start != finish) {
					vector<int> sols;
					vector<int> eols;
					tv->FillSolEol(sols, eols, start, finish);
					// int32 startline=-1;
					// int32 endline=-1;

					/*for(int i=0;i<sols.size();i++)
					  {
					  if(start >= sols[i] && start <= eols[i])
					  startline = i;
					  if(finish >= sols[i] && finish <= eols[i])
					  endline = i;
					  if(startline >= 0 && endline >= 0)
					  break;
					  }*/
					int offset = 0;
					for (uint i = 0; i < sols.size(); i++) {
						tv->Insert(sols[i] + offset, "%", 1);
						offset++;  // we have to do this because with each insert the sols[i] offset
								   // is invalid (a shift occurs)
					}
				}
			}

		} break;
		case K_MENU_FORMAT_UNCOMMENTLINE:
		{
			int32 start, finish;
			TexView* tv = CurrentTexView();
			if (tv != NULL) {
				tv->GetSelection(&start, &finish);
				if (start != finish) {
					vector<int> sols;
					vector<int> eols;
					tv->FillSolEol(sols, eols, start, finish);
					// int32 startline=-1;
					// int32 endline=-1;

					/*for(int i=0;i<sols.size();i++)
					  {
					  if(start >= sols[i] && start <= eols[i])
					  startline = i;
					  if(finish >= sols[i] && finish < eols[i])
					  endline = i;
					  if(startline >= 0 && endline >= 0)
					  break;
					  }*/
					// if(startline < 0 || endline < 0)
					//	break;
					BFont font(be_fixed_font);
					font.SetSize(12);
					int offset = 0;
					for (uint i = 0; i < sols.size(); i++) {
						const char* text = tv->Text();
						if (text[sols[i] - offset] == '%')	// we don't want to delete a non-comment
						{
							tv->Delete(sols[i] - offset, sols[i] - offset + 1);
							offset++;  // we have to do this because with each delete the sols[i]
									   // offset is invalid (a shift occurs)
						}
					}
				}
			}

		} break;
		case K_FILE_CONTENTS_CHANGED:
		{
			ProjectItem* item = CurrentTListItem();
			if (item != NULL) {
				if (!item->IsSaveNeeded()) {
					item->SetSaveNeeded(true);
					// TODO ?m_projectView->InvalidateItem(m_projectView->CurrentSelection());
				}
			}

		} break;
		case K_SEARCH_WINDOW_FIND:
		{
			const char* ftext;
			int32 err;	// = B_ERROR;
			int32 length;

			if (message->FindString("ftext", &ftext) == B_OK) {
				bool IsCaseSensitive;
				bool IsWrapAround;
				bool IsSearchBackwards;
				bool IsAllDocs;

				TexView* tv = CurrentTexView();
				if (tv != NULL) {
					BString findme;
					findme << ftext;
					length = findme.Length();

					if (length <= 0)
						return;

					BString text(tv->Text());

					int32 start, finish;
					tv->GetSelection(&start, &finish);

					if (preferences->IsSearchBackwards) {
						if (preferences->IsCaseSensitive) {
							err = text.FindLast(findme, start);
						} else {
							err = text.IFindLast(findme, start);
						}
					} else {
						if (preferences->IsCaseSensitive) {
							err = text.FindFirst(findme, finish);
						} else {
							err = text.IFindFirst(findme, finish);
						}
					}

					if (err < 0 && preferences->IsWrapAround) {
						if (preferences->IsSearchBackwards) {
							if (preferences->IsCaseSensitive) {
								err = text.FindLast(findme, text.Length());
							} else {
								err = text.IFindLast(findme, text.Length());
							}
						} else {
							if (preferences->IsCaseSensitive) {
								err = text.FindFirst(findme, 0);
							} else {
								err = text.IFindFirst(findme, 0);
							}
						}
					}
					if (err >= 0) {
						finish = err + length;
						tv->Select(err, finish);
						tv->ScrollToSelection();
						Activate(true);
					}
				}  // end if(tv != NULL)
			}
		} break;
		case K_SEARCH_WINDOW_REPLACE:
		case K_SEARCH_WINDOW_REPLACE_ALL:
		{
			const char* ftext;
			const char* rtext;
			bool all;
			if (message->FindString("ftext", &ftext) == B_OK
				&& message->FindString("rtext", &rtext) == B_OK
				&& message->FindBool("all", &all) == B_OK) {
				TexView* tv = CurrentTexView();
				if (tv != NULL) {
					BString findme;
					findme << ftext;

					BString replacewith;
					replacewith << rtext;

					BString text(tv->Text());


					int32 start, finish;
					int32 offset;
					if (all)
						start = 0;
					else
						tv->GetSelection(&start, &finish);
					// int32 lastoffset=-1;
					// if(replacewith.Length() > 0)
					// while((offset = text.FindFirst(findme.String(),start)) >= 0)
					//{
					//{
					if (!all) {
						offset = text.FindFirst(findme, start);
						tv->Delete(offset, offset + findme.Length());
						tv->Insert(offset, replacewith.String(), replacewith.Length());
						tv->Select(offset, offset + replacewith.Length());
						tv->ScrollToSelection();
						Activate(true);
					} else {
						if (preferences->IsAllDocs) {
							TexView* tv;
							ProjectItem* item;
							start = 0;
							for (int i = 0; i < m_projectView->CountItems(); i++) {
								item = (ProjectItem*)m_projectView->ItemAt(i);
								tv = (TexView*)item->ChildView();
								if (item != NULL && tv != NULL) {
									text = tv->Text();
									// if(preferences->IsCaseSensitive)
									{
										text.ReplaceAll(findme.String(), replacewith.String());
									}
									/*else
									  {
									  text.IReplaceAll(findme.String(),replacewith.String());
									  }*/

									if (text.Compare(tv->Text()) == 0)
										return;	 // the same after replace all

									tv->SetText(text.String(), text.Length());

									/*if(text.Length() < start)
									  start = text.Length();
									  if(text.Length() < finish)
									  finish = text.Length();
									  */
									// if(i == m_projectView->CountItems()-1)
									//{
									// tv->Select(start,finish);
									// tv->ScrollToSelection();
									Activate(true);
									// }
								}
							}

						} else {
							if (preferences->IsCaseSensitive) {
								text.ReplaceAll(findme.String(), replacewith.String());
							} else {
								text.IReplaceAll(findme.String(), replacewith.String());
							}

							if (text.Compare(tv->Text()) == 0)
								return;	 // the same after replace all

							tv->SetText(text.String(), text.Length());

							if (text.Length() < start)
								start = text.Length();
							if (text.Length() < finish)
								finish = text.Length();

							tv->Select(start, finish);
							tv->ScrollToSelection();
							Activate(true);
						}
					}
				}
			}
		} break;
		case K_SEARCH_WINDOW_QUIT:
		{
			searchPanel = NULL;
		} break;
		case K_GTL_WINDOW_QUIT:
			gtlPanel = NULL;
			break;
		case K_ABOUT_WINDOW_QUIT:
			aboutPanel = NULL;
			break;
		case K_PREFS_WINDOW_QUIT:
			prefsPanel = NULL;
			break;
		case K_MENU_FILE_PREFS:
		{
			if (prefsPanel != NULL) {
				prefsPanel->Activate(true);
				break;
			}

			BPoint p;
			uint32 buttons;
			ChildAt(0)->GetMouse(&p, &buttons);
			ChildAt(0)->ConvertToScreen(&p);
			p.x -= 30;
			p.y -= 30;
			float w = 600, h = 400;
			prefsPanel = new PrefsWindow(
				BRect(p.x, p.y, p.x + w, p.y + h), new BMessenger(this), m_texToolBar);

			prefsPanel->StartWatching(this, PrefsConstants::K_PREFS_UPDATE);

			prefsPanel->Show();
		} break;
		case K_MENU_EDIT_SEARCH:
		{
			if (searchPanel != NULL) {
				searchPanel->Activate(true);
				break;
			}

			BPoint p;
			uint32 buttons;
			ChildAt(0)->GetMouse(&p, &buttons);
			ChildAt(0)->ConvertToScreen(&p);
			p.x -= 30;
			p.y -= 30;
			float w = 380, h = 235;
			searchPanel = new SearchWindow(BRect(p.x, p.y, p.x + w, p.y + h), new BMessenger(this));

			searchPanel->Show();
			searchPanel->CenterOnScreen();
		} break;
		case K_MENU_EDIT_GOTOLINE:
		{
			if (gtlPanel != NULL) {
				gtlPanel->Activate(true);
				break;
			}
			BPoint p;
			uint32 buttons;
			ChildAt(0)->GetMouse(&p, &buttons);
			ChildAt(0)->ConvertToScreen(&p);
			p.x -= 15;
			p.y -= 15;
			gtlPanel
				= new GoToLineWindow(BRect(p.x, p.y, p.x + 102, p.y + 40), new BMessenger(this));
			gtlPanel->Show();
		} break;
		case AboutMessages::K_ABOUT_WINDOW_LAUNCH:
		{
			if (aboutPanel != NULL) {
				aboutPanel->Activate(true);
				break;
			}

			BPoint p;
			uint32 buttons;
			ChildAt(0)->GetMouse(&p, &buttons);
			ChildAt(0)->ConvertToScreen(&p);
			p.x -= 15;
			p.y -= 15;
			aboutPanel
				= new AboutWindow(BRect(p.x, p.y, p.x + 400, p.y + 300), new BMessenger(this));
			aboutPanel->Show();
			aboutPanel->CenterOnScreen();
		} break;
		case K_GTL_WINDOW_GO:
		{
			int32 line;
			if (message->FindInt32("line", &line) == B_OK) {
				TexView* tv = CurrentTexView();
				if (tv != NULL) {
					tv->GoToLine(line);
					tv->ScrollToSelection();
					UpdateStatusBar();
				}
			}

		} break;
		case K_UPDATE_STATUSBAR:
		{
			ResetPermissions();
			UpdateStatusBar();
		} break;
		case K_CMD_TBUTTON_INSERT:
		{
			const char* cmd;
			if (message->FindString("cmd", &cmd) == B_OK)
				InsertText(cmd);
		} break;
		case K_UPDATE_TEXTVIEW_SHORTCUTS:
		{
			SetShortcuts();
		} break;
		case K_CMD_LAUNCH_TERMINAL:
		{
			ProjectItem* item = CurrentTListItem();
			if (item != NULL) {
				BString cmd;
				// char* script = "/tmp/cdtodir.sh";
				entry_ref ref = item->GetRef();
				BPath path(&ref);
				if (path.InitCheck() == B_OK) {
					BPath parent;
					path.GetParent(&parent);
					cmd << "cd " << parent.Path() << " && /boot/beos/apps/Terminal &";
					system(cmd.String());
				}
			}
		} break;
		case K_CMD_LAUNCH_TRACKER:
		{
			ProjectItem* item = CurrentTListItem();
			if (item != NULL) {
				entry_ref ref = item->GetRef();
				BPath path(&ref);
				if (path.InitCheck() == B_OK) {
					BPath parent;
					path.GetParent(&parent);
					char* arg = (char*)parent.Path();
					be_roster->Launch("application/x-vnd.Be-TRAK", 1, &arg);
				}
			}
		} break;
		case K_CMD_LAUNCH_PSVIEWER:
		{
			Execute("/tmp/psview.sh", preferences->commands["postscript_cmd"]);
		} break;
		case K_CMD_LAUNCH_HTMLVIEWER:
		{
			ProjectItem* item = CurrentTListItem();
			if (item != NULL) {
				// char* script = "/tmp/cdtodir.sh";
				entry_ref ref = item->GetRef();
				BPath path(&ref);
				if (path.InitCheck() == B_OK) {
					BPath parent;
					path.GetParent(&parent);
					BString fname(path.Leaf());
					fname.Replace(".tex", "", 1);

					BString url;
					url << parent.Path() << "/" << fname.String() << "/" << fname.String()
						<< ".html";
					char* args[] = {(char*)url.String(), NULL};
					be_roster->Launch("text/html", 1, args);
					// application/x-vnd.Be-NPOS
				}
			}
		} break;
		case K_CMD_LAUNCH_BE_PDF:
		{
			if (m_projectView->CountItems() >= 1) {
				int current = m_projectView->CurrentSelection();
				if (current >= 0) {
					ProjectItem* li = (ProjectItem*)m_projectView->ItemAt(current);
					entry_ref ref = li->GetRef();
					BPath path(&ref);
					if (path.InitCheck() == B_OK) {
						BPath parent;
						path.GetParent(&parent);

						BString pdfname(path.Leaf());
						pdfname.Replace(".tex", ".pdf", 1);
						parent.Append(pdfname.String());
						char* args[] = {(char*)parent.Path(), NULL};
						be_roster->Launch("application/x-vnd.mp-xpdf", 1, args);
					}
				}
			}
		} break;
		case K_CMD_COMPILE_TEXDVI:
		{
			Execute("/tmp/textodvi.sh", preferences->commands["latex_cmd"]);
		} break;
		case K_CMD_COMPILE_DVIPDF:
		{
			Execute("/tmp/dvitopdf.sh", preferences->commands["dvipdf_cmd"]);
		} break;
		case K_CMD_COMPILE_DVIPS:
		{
			Execute("/tmp/dvitops.sh", preferences->commands["dvips_cmd"]);
		} break;
		case K_CMD_COMPILE_PSPDF:
		{
			Execute("/tmp/pstopdf.sh", preferences->commands["ps2pdf_cmd"]);
		} break;
		case K_CMD_COMPILE_TEXPDF:
		{
			Execute("/tmp/textopdf.sh", preferences->commands["pdflatex_cmd"]);
		} break;
		case K_CMD_COMPILE_TEXHTML:
		{
			Execute("/tmp/textohtml.sh", preferences->commands["latex2html_cmd"]);
		} break;
		case B_SIMPLE_DATA:
		case K_MENU_FILE_OPEN_RECENT:
		case B_REFS_RECEIVED:
		{
			entry_ref ref;
			status_t err;
			int32 ref_num;
			ref_num = 0;
			while ((err = message->FindRef("refs", ref_num, &ref)) == B_OK) {
				BEntry e(&ref, true);
				BNode node(&e);
				if (node.InitCheck() == B_OK) {
					BNodeInfo ni(&node);
					if (ni.InitCheck() == B_OK) {
						char mime[B_MIME_TYPE_LENGTH];
						if (ni.GetType(mime) == B_OK) {
							BString str(ref.name);

							if (str.FindFirst(".tex") > 0
								&& (strcmp(TEX_FILETYPE, mime) == 0
									|| strcmp("text/plain", mime) == 0)) {
								BScrollView* docScroll = new BScrollView("docScroll",
									new TexView(BRect(0, 0, 0, 0), BRect(0, 0, 0, 0)),
									B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS, false, true);
								ProjectItem* projectItem
									= new ProjectItem(m_horizontalSplit, str, docScroll);
								projectItem->SetRef(&ref);
								m_projectView->AddItem(projectItem);
								be_roster->AddToRecentDocuments(&ref, APP_SIG);
								m_projectView->Select(m_projectView->CountItems() - 1);

								TexView* tv = CurrentTexView();
								if (tv != NULL)
									tv->LoadFile(&ref);
								ResetPermissions();
							}
						}
						if (strcmp(mime, "application/x-vnd.Be-directory") == 0
							|| strcmp(mime, "application/x-vnd.Be-symlink") == 0) {
							entry_ref dir_contains_ref;
							BMessage process_dir(B_SIMPLE_DATA);
							BDirectory dir(&ref);
							if (dir.InitCheck() == B_OK) {
								while (dir.GetNextRef(&dir_contains_ref) == B_OK) {
									if (process_dir.AddRef("refs", &dir_contains_ref) != B_OK)
										return;
								}
								PostMessage(&process_dir, this);
							}
						}
					}
				}
				ref_num++;
			}
			//	OkToRemoveItem = true;
			if (preferences->IsActivationOk && !IsActive())
				Activate();
		} break;
		case K_MENU_FILE_OPEN_TEMPLATE:
		{
			entry_ref ref;
			if (message->FindRef("refs", &ref) == B_OK)
			// No need to verify the mime type : it is already verified when the template is added
			// to the menu
			{
				BNode node(&ref);
				if (node.InitCheck() == B_OK) {
					/*BNodeInfo ni(&node);
					  if(ni.InitCheck() == B_OK)
					  {

					  char mime[B_MIME_TYPE_LENGTH];
					  if(ni.GetType(mime)==B_OK)
					  {
					  BString str(ref.name);
					  if(str.FindFirst(".tex") > 0 && (strcmp(TEX_FILETYPE,mime) == 0 ||
					  strcmp("text/plain",mime) == 0))
					  {
					  */
					BScrollView* docScroll = new BScrollView(ref.name,
						new TexView(BRect(0, 0, 0, 0), BRect(0, 0, 0, 0)), B_FOLLOW_ALL_SIDES,
						B_WILL_DRAW | B_FRAME_EVENTS, false, true);
					ProjectItem* projectItem
						= new ProjectItem(m_horizontalSplit, ref.name, docScroll);
					projectItem->SetRef(&ref);
					m_projectView->AddItem(projectItem);
					be_roster->AddToRecentDocuments(&ref, "application/x-vnd.misza-BeTeX");
					m_projectView->Select(m_projectView->CountItems() - 1);

					// BString fname(ref.name);

					TexView* tv = CurrentTexView();
					if (tv != NULL)
						tv->LoadFile(&ref);
					ResetPermissions();
					/*
					   }
					   }
					   }*/
				}
			}

		} break;
		case B_SAVE_REQUESTED:
		{
			Save(message, m_projectView->CurrentSelection());

			if (RemoveAfterSave) {
				int32 current = RemoveAfterSaveIndex;
				m_projectView->RemoveItem(current);
				int items = m_projectView->CountItems();
				if (items == 0) {
					SetTitle("BeTeX");
					untitled_no = 1;
				} else {
					if (current - 1 >= 0)
						m_projectView->Select(current - 1);
					else if (current < items)
						m_projectView->Select(current);
				}

				UpdateStatusBar();
				RemoveAfterSave = false;
			}

		} break;
		case K_MENU_FILE_NEW:
		{
			NewDocument();

		} break;
		case K_MENU_FILE_OPEN:
			openPanel->Show();
			break;
		case K_MENU_FILE_OPEN_FOLDER:
			openfolderPanel->Show();
			break;
		case K_MENU_FILE_CLOSE:
		{
			Close(m_projectView->CurrentSelection());

		} break;
		case K_MENU_FILE_SAVEAS:
		{
			SaveAsPanel(m_projectView->CurrentSelection());
		} break;
		case K_MENU_FILE_SAVE:
		{
			Save(NULL, m_projectView->CurrentSelection());
		} break;
		case K_MENU_FILE_NEXT_ITEM:
			m_projectView->Next(1);
			break;
		case K_MENU_FILE_PREV_ITEM:
			m_projectView->Prev(1);
			break;
		case K_MENU_FILE_QUIT:
		{
			QuitRequested();
		} break;
		case K_MENU_FILE_ABOUT:
			be_app->PostMessage(B_ABOUT_REQUESTED);
			break;
		case K_RESET_ALL_TEX_VIEW_COLORS:
		{
			TexView* tv;
			ProjectItem* item;
			for (int i = 0; i < m_projectView->CountItems(); i++) {
				item = (ProjectItem*)m_projectView->ItemAt(i);
				tv = (TexView*)item->ChildView();
				if (item != NULL && tv != NULL) {
					tv->UpdateColors();
					tv->SetViewColor(preferences->bg_color);
					tv->Invalidate();
				}
			}
		} break;
		case K_RESET_ALL_TEX_VIEW_FONTS:
		{
			TexView* tv;
			ProjectItem* item;
			for (int i = 0; i < m_projectView->CountItems(); i++) {
				item = (ProjectItem*)m_projectView->ItemAt(i);
				tv = (TexView*)item->ChildView();
				if (item != NULL && tv != NULL) {
					tv->UpdateFontSize();
				}
			}
		} break;
		case B_OBSERVER_NOTICE_CHANGE:
		{
			if (message->GetInt32(B_OBSERVE_ORIGINAL_WHAT, 0)
				== (int32)PrefsConstants::K_PREFS_UPDATE) {
				rgb_color bubble_color = preferences->bubble_color;
				helper->SetColor(bubble_color);
			} else {
				BWindow::MessageReceived(message);
			}
		} break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

void
MainWindow::NewDocument()
{
	BString fname;
	fname << "Untitled" << untitled_no << ".tex";
	BScrollView* docScroll
		= new BScrollView(fname.String(), new TexView(BRect(0, 0, 0, 0), BRect(0, 0, 0, 0)),
			B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS, false, true);
	m_projectView->AddItem(new ProjectItem(m_horizontalSplit, fname,
		docScroll));  // docScroll.Rect() ?
	untitled_no++;
	m_projectView->Select(m_projectView->CountItems() - 1);
	ResetPermissions();
}

void
MainWindow::Save(BMessage* msg, int32 index)
{
	entry_ref ref;
	const char* fname;
	BPath path;
	BEntry entry;

	if (index >= 0) {
		ProjectItem* li = (ProjectItem*)m_projectView->ItemAt(index);

		if (msg && (msg->FindRef("directory", &ref)) == B_OK
			&& (msg->FindString("name", &fname)) == B_OK) {
			if (entry.SetTo(&ref) == B_OK) {
				entry.GetPath(&path);
				BString name_checker = fname;
				if (name_checker.FindFirst(".tex") < 0) {
					name_checker << ".tex";
					fname = name_checker.String();
				}

				path.Append(fname);

				BFile file(path.Path(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
				if (file.InitCheck() == B_OK) {
					file.Write(((BTextView*)li->ChildView())->Text(),
						((BTextView*)li->ChildView())->TextLength());
					file.Unset();
					update_mime_info(path.Path(), 0, 0, 0);

					BString title;
					title << "BeTeX" << " - " << fname;
					SetTitle(title.String());

					BEntry final_entry(path.Path());
					entry_ref final_ref;
					final_entry.GetRef(&final_ref);
					li->SetRef(&final_ref);
					// li->Select();
					li->SetHomely(true);
					li->SetSaveNeeded(false);
					m_projectView->InvalidateItem(index);
				}
			}
		} else	// Save to current file
		{
			if (li->IsHomely())	 // we can save somewhere
			{
				ref = li->GetRef();
				BFile file(&ref, B_READ_WRITE | B_ERASE_FILE);
				if (file.InitCheck() == B_OK) {
					file.Write(li->ChildView()->Text(), li->ChildView()->TextLength());
					li->SetSaveNeeded(false);
					li->SetHomely(true);
					m_projectView->InvalidateItem(index);
				}
			}
			/*
			   else	//need to choose initial name
			   {
			   SaveAsPanel(index);

			   }
			   */
		}
	}
	ResetPermissions();
	UpdateStatusBar();
}

void
MainWindow::SaveAsPanel(int32 index)
{
	if (index >= 0) {
		ProjectItem* li = (ProjectItem*)m_projectView->ItemAt(index);
		savePanel->SetSaveText(li->Label());
		savePanel->Show();
	}
}

void
MainWindow::Close(int32 current)
{
	if (current >= 0) {
		ProjectItem* li = (ProjectItem*)m_projectView->ItemAt(current);
		if (li != NULL && li->IsSaveNeeded()) {
			BString text;
			text << "Save changes to \"" << li->Label() << "\"?";

			BAlert* alert = new BAlert("savealert", text.String(), "Cancel", "Don't Save", "Save",
				B_WIDTH_AS_USUAL, B_WARNING_ALERT);
			alert->SetShortcut(0, B_ESCAPE);
			int32 button_index = alert->Go();
			switch (button_index) {
				case 0:
					break;

				case 1:
					// TODO review block
					{
						// m_projectView->Invalidate();

						// if(!all)
						//{
						m_projectView->RemoveItem(current);
						int items = m_projectView->CountItems();
						if (items == 0) {
							SetTitle("BeTeX");
							untitled_no = 1;
						} else {
							if (current - 1 >= 0)
								m_projectView->Select(current - 1);
							else if (current < items)
								m_projectView->Select(current);
						}


						//}
					}
					break;
				case 2:
					// TODO review block
					{
						// OkToRemoveItem = false;
						RemoveAfterSave = true;
						RemoveAfterSaveIndex = current;
						Save(NULL, current);
						// for(;!OkToRemoveItem;)
						//	;
						//  while(savePanel->IsShowing())
						// m_projectView->Invalidate();
					}
					break;
			}  // end switch
			if (!cbfq.empty()) {
				if (cbfqindex + 1 != cbfq.size()) {
					cbfqindex++;
					Close(cbfq[cbfqindex]);
				} else {
					cbfqindex = 0;
					cbfq.clear();
				}
			}

		}  // end save needed
		else  // if(!all)
		{
			m_projectView->RemoveItem(current);
			// m_projectView->Invalidate();

			int items = m_projectView->CountItems();
			if (items == 0) {
				SetTitle("BeTeX");
				untitled_no = 1;
			} else {
				if (current - 1 >= 0)
					m_projectView->Select(current - 1);
				else if (current < items)
					m_projectView->Select(current);
			}
		}
	}
	// end if(current >=0)
	// if(!all)	//if we only want to close one item we break here
	//	break;
	// }	//end for
	// end if numitems > 0
	// else	I like this but hey :=)
	//	be_app->PostMessage(B_QUIT_REQUESTED);
	UpdateStatusBar();
	ResetPermissions();
}

void
MainWindow::Print()
{
	// printer_settings
	status_t result = B_OK;
	TexView* tv = CurrentTexView();
	ProjectItem* it = CurrentTListItem();

	BRect paper_rect;
	BRect printable_rect;
	BRect old_text_rect;
	if (tv != NULL && it != NULL && tv->TextLength() != 0) {
		old_text_rect = tv->TextRect();
		if (printer_settings == NULL) {
			result = PageSetup(it->Label());  // job.ConfigPage();
			if (result != B_OK) {
				return;
			}
		}
		BPrintJob job(it->Label());
		// setup the driver with user settings
		job.SetSettings(new BMessage(*printer_settings));

		result = job.ConfigJob();
		if (result != B_OK)
			return;


		// printer_settings = job.Settings();
		// use new settings for internal use
		// may have changed
		paper_rect = job.PaperRect();
		printable_rect = job.PrintableRect();

		BRect resize_to_me(printable_rect);
		resize_to_me.top -= 20;
		resize_to_me.right -= 10;
		tv->SetTextRect(resize_to_me);

		// Time to get num of pages...
		// pages are zero-based
		int32 firstPage = job.FirstPage();
		int32 lastPage = job.LastPage();

		int32 firstLine = 0;
		int32 lastLine = tv->CountPhysicalLines();

		int32 pagesInDocument = 1;
		int32 linesInDocument = tv->CountPhysicalLines();

		int32 currentLine = 0;
		while (currentLine < linesInDocument) {
			float currentHeight = 0;
			while ((currentHeight < printable_rect.Height()) && (currentLine < linesInDocument)) {
				currentHeight += tv->LineHeight(currentLine);
				if (currentHeight < printable_rect.Height())
					currentLine++;
			}

			if (pagesInDocument == lastPage)
				lastLine = currentLine;
			if (currentHeight >= printable_rect.Height()) {
				pagesInDocument++;
				if (pagesInDocument == firstPage)
					firstLine = currentLine;
			}
		}

		if (lastPage > pagesInDocument - 1) {
			lastPage = pagesInDocument - 1;
			lastLine = currentLine - 1;
		}


		// Verify that the range is correct
		//  0 ... LONG_MAX -> Print all the document
		//  n ... LONG_MAX -> Print from page n to the end
		//  n ... m -> Print from page n to page m


		int32 numPages = lastPage - firstPage + 1;

		// Verify range is correct
		if (numPages <= 0)
			return;

		// Now we can print the page
		job.BeginJob();

		// Print all pages
		// TODO ? bool can_continue = job.CanContinue();

		int32 printLine = firstLine;
		while (printLine < lastLine) {
			float currentHeight = 0;
			int32 firstLineOnPage = printLine;
			while ((currentHeight < printable_rect.Height()) && (printLine < lastLine)) {
				currentHeight += tv->LineHeight(printLine);
				if (currentHeight < printable_rect.Height())
					printLine++;
			}
			float top = 0;
			if (firstLineOnPage != 0)
				top = tv->TextHeight(0, firstLineOnPage - 1);
			int TEXT_INSET = 0;
			float bottom = tv->TextHeight(0, printLine - 1);
			BRect textRect(0.0, top + TEXT_INSET, printable_rect.Width(), bottom + TEXT_INSET);
			job.DrawView(tv, textRect, BPoint(0.0, 0.0));
			job.SpoolPage();
		}
		job.CommitJob();
		tv->SetTextRect(old_text_rect);
	}
}

status_t
MainWindow::PageSetup(const char* fname)
{
	status_t result = B_OK;

	BPrintJob job(fname);

	if (printer_settings != NULL)
		job.SetSettings(new BMessage(*printer_settings));

	result = job.ConfigPage();

	if (result == B_NO_ERROR) {
		delete printer_settings;
		printer_settings = job.Settings();
	}
	return result;
}

void
MainWindow::MenusBeginning()
{
	for (int i = opensubmenu->CountItems() - 1; i >= 0; i--) {
		delete opensubmenu->RemoveItem(i);
	}

	BMessage recentrefs;
	be_roster->GetRecentDocuments(
		&recentrefs, atoi(preferences->NumRecentDocs.String()), TEX_FILETYPE, APP_SIG);

	entry_ref ref;
	int32 ref_num;
	ref_num = 0;
	while (recentrefs.FindRef("refs", ref_num, &ref) == B_OK) {
		BMessage* openmsg = new BMessage(K_MENU_FILE_OPEN_RECENT);
		BEntry entry(&ref);
		if (entry.InitCheck() == B_OK && entry.Exists() && openmsg->AddRef("refs", &ref) == B_OK) {
			BPath temp_path(&ref);
			bool isPathShown = preferences->IsRecentDocsPathShown;
			const char* label = isPathShown ? temp_path.Path() : temp_path.Leaf();
			opensubmenu->AddItem(new BMenuItem(label, openmsg));
		}

		ref_num++;
	}

	for (int j = fopentemplatesubmenu->CountItems() - 1; j >= 0; j--) {
		delete fopentemplatesubmenu->RemoveItem(j);
	}

	if (TemplateDir.Length() > 0) {
		BEntry entryTemplate(TemplateDir.String());
		if (!entryTemplate.InitCheck() || !entryTemplate.Exists()) {
			if (create_directory(TemplateDir.String(), 0777) != B_OK) {
				return;
			}
		}
		BDirectory dir(TemplateDir.String());
		if (dir.InitCheck() == B_OK) {
			while (dir.GetNextRef(&ref) == B_OK) {
				BMessage* openmsg = new BMessage(K_MENU_FILE_OPEN_TEMPLATE);
				BEntry entry(&ref, true);
				if (entry.InitCheck() == B_OK && entry.Exists()
					&& openmsg->AddRef("refs", &ref) == B_OK) {
					BNode node(&entry);
					if (node.InitCheck() == B_OK) {
						BNodeInfo ni(&node);
						if (ni.InitCheck() == B_OK) {
							char mime[B_MIME_TYPE_LENGTH];
							if (ni.GetType(mime) == B_OK) {
								BString str(ref.name);
								if (str.FindFirst(".tex") > 0
									&& (strcmp(TEX_FILETYPE, mime) == 0
										|| strcmp("text/plain", mime) == 0)) {
									fopentemplatesubmenu->AddItem(new BMenuItem(ref.name, openmsg));
								}
							}
						}
					}
				}
			}
		}
	}
}

BView*
MainWindow::CreateToolBar(BRect toolBarFrame)
{
	WToolbar* toolbar = new WToolbar(toolBarFrame, "toolBar");

	BBitmap* newDocBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	newDocBitmap->SetBits(IconNewDoc, 3072, 0, B_CMAP8);
	toolbar->AddItem(
		new WToolbarButton("NewButton", "New", newDocBitmap, new BMessage(K_MENU_FILE_NEW)));

	BBitmap* openBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	openBitmap->SetBits(IconOpen, 3072, 0, B_CMAP8);
	toolbar->AddItem(
		new WToolbarButton("openButton", "Open", openBitmap, new BMessage(K_MENU_FILE_OPEN)));

	BBitmap* saveBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	saveBitmap->SetBits(IconSave, 3072, 0, B_CMAP8);
	toolbar->AddItem(
		new WToolbarButton("saveButton", "Save", saveBitmap, new BMessage(K_MENU_FILE_SAVE)));

	BBitmap* printerBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	printerBitmap->SetBits(IconPrintMe, 3072, 0, B_CMAP8);
	toolbar->AddItem(
		new WToolbarButton("printButton", "Print", printerBitmap, new BMessage(K_MENU_FILE_PRINT)));

	BBitmap* viewLogBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	viewLogBitmap->SetBits(IconViewLog, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton(
		"viewLogButton", "View Logfile", viewLogBitmap, new BMessage(K_CMD_VIEW_LOG_FILE)));

	BBitmap* trackerBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	trackerBitmap->SetBits(IconOpenCWDTracker, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton("openParentFolderButton", "Open Parent Folder",
		trackerBitmap, new BMessage(K_CMD_LAUNCH_TRACKER)));

	BBitmap* terminalBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	terminalBitmap->SetBits(IconOpenCWDTerminal, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton("openTerminalButton", "Open Terminal in Parent Folder",
		terminalBitmap, new BMessage(K_CMD_LAUNCH_TERMINAL)));

	BBitmap* texDviBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	texDviBitmap->SetBits(IconTexToDVI, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton(
		"texToDVIButton", "Tex To DVI", texDviBitmap, new BMessage(K_CMD_COMPILE_TEXDVI)));

	BBitmap* dviPdfBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	dviPdfBitmap->SetBits(IconDVIToPDF, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton(
		"dviToPDFButton", "Convert DVI to PDF", dviPdfBitmap, new BMessage(K_CMD_COMPILE_DVIPDF)));

	BBitmap* dviPsBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	dviPsBitmap->SetBits(IconDVIToPS, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton(
		"dviToPSButton", "Convert DVI to PS", dviPsBitmap, new BMessage(K_CMD_COMPILE_DVIPS)));

	BBitmap* psPdfBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	psPdfBitmap->SetBits(IconPSToPDF, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton(
		"psToPDFButton", "Convert PS to PDF", psPdfBitmap, new BMessage(K_CMD_COMPILE_PSPDF)));

	BBitmap* texPdfBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	texPdfBitmap->SetBits(IconTexToPDF, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton(
		"texToPDFButton", "Convert TeX to PDF", texPdfBitmap, new BMessage(K_CMD_COMPILE_TEXPDF)));

	BBitmap* texHtmlBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	texHtmlBitmap->SetBits(IconTexToHTML, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton("texToHTMLButton", "Convert TeX to HTML", texHtmlBitmap,
		new BMessage(K_CMD_COMPILE_TEXHTML)));

	BBitmap* prevPSBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	prevPSBitmap->SetBits(IconPrevPS, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton(
		"viewPSButton", "View PS Document", prevPSBitmap, new BMessage(K_CMD_LAUNCH_PSVIEWER)));

	BBitmap* prevPDFBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	prevPDFBitmap->SetBits(IconPrevPDF, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton(
		"viewPDFButton", "View PDF Document", prevPDFBitmap, new BMessage(K_CMD_LAUNCH_BE_PDF)));

	BBitmap* prevHTMLBitmap = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	prevHTMLBitmap->SetBits(IconPrevHTML, 3072, 0, B_CMAP8);
	toolbar->AddItem(new WToolbarButton("viewHTMLButton", "View HTML Document", prevHTMLBitmap,
		new BMessage(K_CMD_LAUNCH_HTMLVIEWER)));

	return toolbar;
}

bool
MainWindow::PromptToQuit()
{
	for (int32 i = 0; i < m_projectView->CountItems(); i++) {
		ProjectItem* projectItem = static_cast<ProjectItem*>(m_projectView->ItemAt(i));
		if (projectItem->IsSaveNeeded())
			return true;
	}
	return false;
}

bool
MainWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return !PromptToQuit();
}
