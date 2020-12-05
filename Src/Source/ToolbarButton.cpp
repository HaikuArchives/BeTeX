#ifndef MAIN_WINDOW_H
#include "MainWindow.h"
#endif

#include <be/app/Application.h>
#include <be/app/MessageRunner.h>
#include <be/app/Roster.h>
#include <be/app/Clipboard.h>
#include <be/interface/Alert.h>
#include <be/interface/Alignment.h>
#include <be/interface/Menu.h>
#include <be/interface/ScrollBar.h>
#include <be/interface/PrintJob.h>
#include <be/interface/LayoutBuilder.h>
#include <be/storage/Entry.h>
#include <be/storage/File.h>
#include <be/storage/Node.h>
#include <be/storage/NodeInfo.h>
#include <be/storage/Directory.h>
#include <be/storage/Mime.h>

#include <unistd.h>
//#include <stdio.h>


#include "Constants.h"
#include "Toolbar.h"
#include "ToolbarButton.h"
#include "TexFileFilter.h"
#include "DocView.h"
#include "Preferences.h"
#include "MessageFields.h"
#include "ConverterIcons.h"
#include "MainTBar.h"

using namespace MenuConstants;
using namespace PrefsConstants;
using namespace ColorPrefsConstants;
using namespace SearchWindowConstants;
using namespace InterfaceConstants;
using namespace ToolbarConstants;
using namespace AboutMessages;

MainWindow::MainWindow(BRect frame) 
				:	BWindow(frame, "BeTeX", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	//Setup our MenuBar
	BMenuBar* menubar = new BMenuBar(frame,"menu_bar");
 
	CreateMenuBar(menubar);

	float statusBarHeight = 14.0f;
	//Create bubblehelper
	helper = new BubbleHelper(preferences->bubble_color);

	//Create Toolbar
	float menuBarHeight = (menubar->Bounds()).Height();



	BRect toolBarFrame(0.0f, menuBarHeight + 1.0f, frame.Width(), menuBarHeight + 37.0f);
	m_toolBar = new MainTBar(toolBarFrame, helper);

	TLIST_VIEW_WIDTH = 178;
	BRect Trect(0.0f, 0.0f, frame.Width(), frame.Height() - statusBarHeight);

	BRect Textrect = Trect;
	Textrect.InsetBy(10,10);

	m_texToolBar = new TexBar(BRect(0,(frame.Height()/2)-99, TLIST_VIEW_WIDTH, frame.Height()- statusBarHeight), helper);
	BScrollView* toolbarScroll = new BScrollView("tbscroller",m_texToolBar,B_FOLLOW_ALL_SIDES,B_WILL_DRAW|B_FRAME_EVENTS,false,true);

	DocView* docView = new DocView(Trect);
	docScroll = new BScrollView("docScroll",docView,B_FOLLOW_ALL_SIDES,B_WILL_DRAW|B_FRAME_EVENTS,false,true);

	m_projectView = new ProjectView(BRect(0,0,TLIST_VIEW_WIDTH,frame.Height()/2-100),docScroll);
	BScrollView* projectScroll = new BScrollView("ScrollView",m_projectView,B_FOLLOW_ALL_SIDES,B_WILL_DRAW|B_FRAME_EVENTS,false,true);

	m_verticalSplit = new SplitPane(BRect(0,frame.top,TLIST_VIEW_WIDTH,frame.Height()), "vertical", projectScroll, toolbarScroll, B_FOLLOW_ALL_SIDES);

	m_verticalSplit->SetAlignment(B_HORIZONTAL);
	m_verticalSplit->SetEditable(false);
	LEFT_BAR_V_POS = frame.Height()/4;
	m_verticalSplit->SetBarPosition(LEFT_BAR_V_POS);
	
	m_horizontalSplit = new SplitPane(frame, "horizontal", m_verticalSplit, docScroll,B_FOLLOW_ALL_SIDES);
	m_projectView->SetSplitPane(m_horizontalSplit);

	m_horizontalSplit->SetAlignment(B_VERTICAL);
	m_horizontalSplit->SetEditable(false);
	m_horizontalSplit->SetBarPosition(TLIST_VIEW_WIDTH);
		
	BRect statusBarFrame(0.0f, frame.bottom - statusBarHeight, frame.Width(), frame.bottom);
	m_statusBar = new StatusBar(statusBarFrame);

	untitled_no = 1;
	//const int UPDATE_TIME = 600000000/2;	// 10/2=5 minutes
	printer_settings = NULL;

	BPath templatePath;
	if (find_directory(B_SYSTEM_DATA_DIRECTORY, &templatePath) == B_OK)
	{	templatePath.Append("BeTeX");
		templatePath.Append("Templates"); 
		TemplateDir = templatePath.Path();
	}
	searchPanel = NULL;//new SearchWindow(BRect(100,100,250,250),this);
	//gtlPanel = NULL;
	aboutPanel = NULL;
	prefsPanel = NULL;
	rgbTxtChooser = NULL;
	dimChooser = NULL;
	RemoveAfterSave = false;

	BMessage recentrefs;
	be_roster->GetRecentDocuments(&recentrefs,20,TEX_FILETYPE,APP_SIG);

	//Setup SaveFilePanel
	savePanel = new BFilePanel(B_SAVE_PANEL, NULL, NULL, B_FILE_NODE, false, NULL, new TexFileFilter()); 
	savePanel->SetTarget(this);

	//Setup OpenFilePanel
	openPanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_FILE_NODE, true, NULL, new TexFileFilter());
	openPanel->SetTarget(this);

	openfolderPanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_DIRECTORY_NODE, true);
	openfolderPanel->SetTarget(this);

	insertfilePanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_FILE_NODE, false, new BMessage(MenuConstants::K_MENU_INSERT_FILE_RECEIVED), new TexFileFilter());
	insertfilePanel->SetTarget(this);

	ResetPermissions();
	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_NO_VERTICAL))
 		.Add(menubar)
		.Add(m_toolBar)
		.Add(m_horizontalSplit)
		.Add(m_statusBar)
		.End()
		;
	//Update split panes according to preferences
	if (preferences->splitmsg != NULL) 
	{
		Lock();
			m_horizontalSplit->SetState(preferences->splitmsg);
		UnlockLooper();
	} else {
		preferences->splitmsg = m_horizontalSplit->GetState();
	}
	if (preferences->split_leftmsg != NULL) 
	{
		Lock();
			m_verticalSplit->SetState(preferences->split_leftmsg);
		Unlock();
	} else {
		preferences->split_leftmsg = m_verticalSplit->GetState();
	}
	
	//Resize
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
	//clean up bubblehelper
	if(helper != NULL)
		delete helper;
}

void MainWindow::CreateMenuBar(BMenuBar *menuBar)
{
	BMenu* fileMenu = new BMenu("File");
	menuBar->AddItem(fileMenu);

	fileMenu->AddItem(fnew = new BMenuItem("New", new BMessage(MenuConstants::K_MENU_FILE_NEW),'N'));
	fileMenu->AddItem(fopen = new BMenuItem(opensubmenu = new BMenu("Open" B_UTF8_ELLIPSIS),new BMessage(MenuConstants::K_MENU_FILE_OPEN)));
	fopen->SetShortcut('O',0);

	fileMenu->AddItem(fopentemplate = new BMenuItem(fopentemplatesubmenu = new BMenu("Open Template" B_UTF8_ELLIPSIS), NULL));
	fileMenu->AddItem(fopenfolder = new BMenuItem("Open Folder" B_UTF8_ELLIPSIS, new BMessage(MenuConstants::K_MENU_FILE_OPEN_FOLDER)));
	fileMenu->AddItem(fclose = new BMenuItem("Close", new BMessage(MenuConstants::K_MENU_FILE_CLOSE),'W'));
	fileMenu->AddItem(fsave = new BMenuItem("Save", new BMessage(MenuConstants::K_MENU_FILE_SAVE),'S'));
	fileMenu->AddItem(fsaveas = new BMenuItem("Save As" B_UTF8_ELLIPSIS, new BMessage(MenuConstants::K_MENU_FILE_SAVEAS)));
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(fpgsetup = new BMenuItem("Page Setup" B_UTF8_ELLIPSIS, new BMessage(MenuConstants::K_MENU_FILE_PAGE_SETUP)));
	fileMenu->AddItem(fprint = new BMenuItem("Print" B_UTF8_ELLIPSIS, new BMessage(MenuConstants::K_MENU_FILE_PRINT),'P'));
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(fprefs = new BMenuItem("Preferences" B_UTF8_ELLIPSIS,new BMessage(MenuConstants::K_MENU_FILE_PREFS)));
	fprefs->SetShortcut('P',B_SHIFT_KEY);
	fileMenu->AddItem(freset_layout = new BMenuItem("Reset Layout", new BMessage(MenuConstants::K_MENU_FORMAT_RESET_LAYOUT)));
	fileMenu->AddItem(fabout = new BMenuItem("About BeTeX" B_UTF8_ELLIPSIS,new BMessage(MenuConstants::K_MENU_FILE_ABOUT)));
	fileMenu->AddItem(fquit = new BMenuItem("Quit",new BMessage(MenuConstants::K_MENU_FILE_QUIT),'Q'));

	BMenu* editMenu = new BMenu("Edit");
	menuBar->AddItem(editMenu);

	editMenu->AddItem(fundo = new BMenuItem("Undo", new BMessage(B_UNDO),'Z'));
	editMenu->AddSeparatorItem();
	editMenu->AddItem(fcut = new BMenuItem("Cut", new BMessage(B_CUT),'X'));
	editMenu->AddItem(fcopy = new BMenuItem("Copy", new BMessage(B_COPY),'C'));
	editMenu->AddItem(fpaste = new BMenuItem("Paste", new BMessage(B_PASTE),'V'));
	editMenu->AddSeparatorItem();
	editMenu->AddItem(fselall = new BMenuItem("Select All", new BMessage(B_SELECT_ALL),'A'));
	editMenu->AddSeparatorItem();
	editMenu->AddItem(fsearchreplace = new BMenuItem("Find" B_UTF8_ELLIPSIS, new BMessage(MenuConstants::K_MENU_EDIT_SEARCH),'F'));
	editMenu->AddItem(fgotoline = new BMenuItem("Go To Line" B_UTF8_ELLIPSIS, new BMessage(MenuConstants::K_MENU_EDIT_GOTOLINE),'G'));

	BMenu* formatMenu = new BMenu("Format");
	menuBar->AddItem(formatMenu);
	formatMenu->AddItem(fbold = new BMenuItem("Bold", new BMessage(MenuConstants::K_MENU_FORMAT_BOLD),'B'));
	formatMenu->AddItem(femph = new BMenuItem("Emphasized", new BMessage(MenuConstants::K_MENU_FORMAT_EMPH),'E'));
	formatMenu->AddItem(fital = new BMenuItem("Italic", new BMessage(MenuConstants::K_MENU_FORMAT_ITAL),'I'));
	formatMenu->AddSeparatorItem();
	formatMenu->AddItem(fshiftleft = new BMenuItem("Shift Left", new BMessage(MenuConstants::K_MENU_FORMAT_SHIFT_LEFT),'['));
	formatMenu->AddItem(fshiftright = new BMenuItem("Shift Right", new BMessage(MenuConstants::K_MENU_FORMAT_SHIFT_RIGHT),']'));
	formatMenu->AddSeparatorItem();
	formatMenu->AddItem(fcomment = new BMenuItem("Comment Selection", new BMessage(MenuConstants::K_MENU_FORMAT_COMMENTLINE)));
	formatMenu->AddItem(funcomment = new BMenuItem("UnComment Selection", new BMessage(MenuConstants::K_MENU_FORMAT_UNCOMMENTLINE)));

	BMenu* insertMenu = new BMenu("Insert");
	menuBar->AddItem(insertMenu);
	insertMenu->AddItem(finsertfile = new BMenuItem("File" B_UTF8_ELLIPSIS,new BMessage(MenuConstants::K_MENU_INSERT_FILE)));
	insertMenu->AddItem(fdate = new BMenuItem("Date",new BMessage(MenuConstants::K_MENU_INSERT_DATE)));
	insertMenu->AddItem(farray = new BMenuItem("Array",new BMessage(MenuConstants::K_MENU_INSERT_ARRAY)));
	insertMenu->AddItem(fmatrix = new BMenuItem("Matrix",new BMessage(MenuConstants::K_MENU_INSERT_MATRIX)));
	insertMenu->AddItem(ftabular = new BMenuItem("Tabular",new BMessage(MenuConstants::K_MENU_INSERT_TABULAR)));
	insertMenu->AddItem(fequation = new BMenuItem("Equation",new BMessage(MenuConstants::K_MENU_INSERT_EQUATION)));   
	insertMenu->AddItem(frgbcolor = new BMenuItem("Colored Text",new BMessage(MenuConstants::K_MENU_INSERT_COLORED_TEXT)));

	insertMenu->AddItem(flists = new BMenuItem(flistsubmenu = new BMenu("Lists"),NULL));
	flistsubmenu->AddItem(fitemize = new BMenuItem("Itemize",new BMessage(MenuConstants::K_MENU_INSERT_ITEMIZE)));
	flistsubmenu->AddItem(fdescription = new BMenuItem("Description",new BMessage(MenuConstants::K_MENU_INSERT_DESCRIPTION)));
	flistsubmenu->AddItem(fenumerate = new BMenuItem("Enumerate",new BMessage(MenuConstants::K_MENU_INSERT_ENUMERATE)));

	insertMenu->AddItem(fenvironments = new BMenuItem(fenvironmentssubmenu = new BMenu("Environments"),NULL));
	fenvironmentssubmenu->AddItem(ffigure = new BMenuItem("Figure",new BMessage(MenuConstants::K_MENU_INSERT_FIGURE)));
	fenvironmentssubmenu->AddItem(ftable = new BMenuItem("Table",new BMessage(MenuConstants::K_MENU_INSERT_TABLE)));
	fenvironmentssubmenu->AddSeparatorItem();
	fenvironmentssubmenu->AddItem(fcenter = new BMenuItem("Center",new BMessage(MenuConstants::K_MENU_INSERT_CENTER)));
	fenvironmentssubmenu->AddItem(fflushleft = new BMenuItem("FlushLeft",new BMessage(MenuConstants::K_MENU_INSERT_FLUSHLEFT)));
	fenvironmentssubmenu->AddItem(fflushright = new BMenuItem("FlushRight",new BMessage(MenuConstants::K_MENU_INSERT_FLUSHRIGHT)));
	fenvironmentssubmenu->AddSeparatorItem();
	fenvironmentssubmenu->AddItem(feqnarray = new BMenuItem("EqnArray",new BMessage(MenuConstants::K_MENU_INSERT_EQNARRAY)));
	fenvironmentssubmenu->AddItem(ffeqnarraystar = new BMenuItem("EqnArray*",new BMessage(MenuConstants::K_MENU_INSERT_EQNARRAY_STAR)));
	fenvironmentssubmenu->AddSeparatorItem();
	fenvironmentssubmenu->AddItem(fverbatim = new BMenuItem("Verbatim",new BMessage(MenuConstants::K_MENU_INSERT_VERBATIM)));
	fenvironmentssubmenu->AddItem(fquote = new BMenuItem("Quote",new BMessage(MenuConstants::K_MENU_INSERT_QUOTE)));

	BMenu *compileMenu = new BMenu("Compile");
	menuBar->AddItem(compileMenu);
	compileMenu->AddItem(ftexdvi = new BMenuItem("tex->dvi", new BMessage(ToolbarConstants::K_CMD_COMPILE_TEXDVI),'1'));
	compileMenu->AddItem(fdvipdf = new BMenuItem("dvi->pdf", new BMessage(ToolbarConstants::K_CMD_COMPILE_DVIPDF),'2'));
	compileMenu->AddItem(fdvips = new BMenuItem("dvi->ps", new BMessage(ToolbarConstants::K_CMD_COMPILE_DVIPS),'3'));
	compileMenu->AddItem(fpspdf = new BMenuItem("ps->pdf", new BMessage(ToolbarConstants::K_CMD_COMPILE_PSPDF),'4'));
	compileMenu->AddItem(ftexpdf = new BMenuItem("tex->pdf", new BMessage(ToolbarConstants::K_CMD_COMPILE_TEXPDF),'5'));
	compileMenu->AddItem(ftexhtml = new BMenuItem("tex->html", new BMessage(ToolbarConstants::K_CMD_COMPILE_TEXHTML),'6'));

	BMenu* previewMenu = new BMenu("Preview");
	menuBar->AddItem(previewMenu);
	previewMenu->AddItem(fpostscript = new BMenuItem("Postscript" B_UTF8_ELLIPSIS, new BMessage(ToolbarConstants::K_CMD_LAUNCH_PSVIEWER),'7'));
	previewMenu->AddItem(fpdf = new BMenuItem("Pdf" B_UTF8_ELLIPSIS, new BMessage(ToolbarConstants::K_CMD_LAUNCH_BE_PDF),'8'));
	previewMenu->AddItem(fhtml = new BMenuItem("Html" B_UTF8_ELLIPSIS, new BMessage(ToolbarConstants::K_CMD_LAUNCH_HTMLVIEWER),'9'));
}

void MainWindow::InsertText(const char* text)
{
	TexView* tv = CurrentTexView();
	if(tv != NULL)
	{
		BString cmd(text);
		int lbrk,rbrk;
		lbrk=cmd.FindLast('{');
		rbrk=cmd.FindLast('}');
		bool InsertTextIntoBrk= (lbrk >= 0 && rbrk >= 0 && rbrk-lbrk == 1);
		int32 start,finish;
		tv->GetSelection(&start,&finish);
		if(start != finish)
		{
			if(InsertTextIntoBrk)
			{
				BString leftstr;
				cmd.CopyInto(leftstr,0,lbrk+1);
				BString rightstr;
				cmd.CopyInto(rightstr,rbrk,cmd.Length()-rbrk);

				tv->Insert(start,leftstr.String(),leftstr.Length(),NULL);
				tv->Insert(finish+leftstr.Length(),rightstr.String(),rightstr.Length(),NULL);
				tv->Select(finish+rightstr.Length()+leftstr.Length(),finish+rightstr.Length()+leftstr.Length());
			}
			else
			{
				tv->Delete(start,finish);
				tv->Insert(start,cmd.String(),cmd.Length(),NULL);
				tv->Select(start+cmd.Length(),start+cmd.Length());
			}
		}
		else
		{
			tv->Insert(start,cmd.String(),cmd.Length(),NULL);
			if(InsertTextIntoBrk)
				tv->Select(start+cmd.Length()-1,start+cmd.Length()-1);
			else
				tv->Select(start+cmd.Length(),start+cmd.Length());
		}
		tv->ScrollToSelection();
	}
}

void MainWindow::UpdateStatusBar()
{
	int current = m_projectView->CurrentSelection();
	if(m_projectView->CountItems() == 0 || current < 0)
			m_statusBar->SetText("BeTeX");

	else
	{

		if(current >= 0)
		{
			TexView* tv = CurrentTexView();
			ProjectItem*