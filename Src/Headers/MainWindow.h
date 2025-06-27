#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <be/app/Message.h>
#include <be/interface/MenuBar.h>
#include <be/interface/MenuItem.h>
#include <be/interface/Rect.h>
#include <be/interface/ScrollView.h>
#include <be/interface/View.h>
#include <be/interface/Window.h>
#include <be/storage/FilePanel.h>
#include <be/storage/FindDirectory.h>
#include <be/storage/NodeMonitor.h>
#include <be/storage/Path.h>
#include <be/support/String.h>
#include <vector>
using std::vector;

#include "AboutWindow.h"
#include "BubbleHelper.h"
#include "ColorWindow.h"
#include "DimensionWindow.h"
#include "GoToLineWindow.h"
#include "MainTBar.h"
#include "PrefsWindow.h"
#include "ProjectItem.h"
#include "ProjectView.h"
#include "SearchWindow.h"
#include "SplitPane.h"
#include "StatusBar.h"
#include "TexBar.h"
#include "TexView.h"
#include "iconheaders/ConverterIcons.h"

class MainWindow : public BWindow {
public:
	MainWindow(BRect frame);
	virtual ~MainWindow();


	virtual void MessageReceived(BMessage* message);
	virtual bool QuitRequested();

	void NewDocument();
	void Save(BMessage* message, int32 index);
	void SaveAsPanel(int32 index);
	void Close(int32 current);
	void Print();
	status_t PageSetup(const char* fname);

	void InsertText(const char* text);
	virtual void MenusBeginning();
	void UpdateStatusBar();
	ProjectView* GetProjectView();
	TexView* CurrentTexView();
	ProjectItem* CurrentTListItem();
	void Execute(const char* script, const char* command);

	void SetShortcuts();
	void ResetPermissions();
	bool PromptToQuit();

private:
	BView* CreateToolBar(BRect toolbarFrame);
	void CreateMenuBar(BMenuBar* menuBar);


private:
	BString TemplateDir;
	BString basename;
	// bool OkToRemoveItem;
	TexBar* m_texToolBar;
	BPath parent_dir;
	BPath TexPath;

	vector<BPath> watchedPaths;
	vector<int> cbfq;

	bool RemoveAfterSave;
	bool RemoveAfterSaveIndex;

	uint32 cbfqindex;
	int untitled_no;
	int TLIST_VIEW_WIDTH;
	float LEFT_BAR_V_POS;

	// OpenPanel
	BFilePanel* openPanel;
	BFilePanel* savePanel;
	BFilePanel* openfolderPanel;
	BFilePanel* insertfilePanel;

	PrefsWindow* prefsPanel;
	SearchWindow* searchPanel;
	GoToLineWindow* gtlPanel;
	AboutWindow* aboutPanel;
	ColorWindow* rgbTxtChooser;
	DimensionWindow* dimChooser;

	BMenuItem* fnew;
	BMenuItem* fopen;
	BMenu* opensubmenu;
	BMenuItem* fopenfolder;

	BMenuItem* fopentemplate;
	BMenu* fopentemplatesubmenu;

	BMenuItem* fclose;
	BMenuItem* fsave;
	BMenuItem* fsaveas;
	//_________________
	BMenuItem* fnextitem;
	BMenuItem* fprevitem;
	//_________________
	BMenuItem* fpgsetup;
	BMenuItem* fprint;
	//_________________
	BMenuItem* fprefs;
	BMenuItem* freset_layout;
	BMenuItem* fabout;
	BMenuItem* fquit;


	BMenuItem* fundo;
	BMenuItem* fredo;
	//_________________
	BMenuItem* fcut;
	BMenuItem* fcopy;
	BMenuItem* fpaste;
	//_________________
	BMenuItem* fselall;
	//_________________
	BMenuItem* fsearchreplace;
	BMenuItem* fgotoline;

	BMenuItem* fbold;
	BMenuItem* femph;
	BMenuItem* fital;
	//_________________
	BMenuItem* fshiftleft;
	BMenuItem* fshiftright;
	//_________________
	BMenuItem* fcomment;
	BMenuItem* funcomment;

	BMenuItem* finsertfile;
	BMenuItem* fdate;
	BMenuItem* farray;
	BMenuItem* fmatrix;
	BMenuItem* ftabular;
	BMenuItem* fequation;
	BMenuItem* frgbcolor;
	BMenuItem* flists;
	BMenu* flistsubmenu;
	BMenuItem* fitemize;
	BMenuItem* fdescription;
	BMenuItem* fenumerate;
	BMenuItem* fcases;

	BMenuItem* fenvironments;
	BMenu* fenvironmentssubmenu;
	BMenuItem* ffigure;
	BMenuItem* ftable;
	BMenuItem* fetabular;
	BMenuItem* fetabularstar;
	BMenuItem* ftabbing;
	BMenuItem* fcenter;
	BMenuItem* fflushleft;
	BMenuItem* fflushright;
	BMenuItem* feqnarray;
	BMenuItem* ffeqnarraystar;
	BMenuItem* fverbatim;
	BMenuItem* fquote;
	BMenuItem* fminipage;

	BMenuItem* ftexdvi;
	BMenuItem* fdvipdf;
	BMenuItem* fdvips;
	BMenuItem* fpspdf;
	BMenuItem* ftexpdf;
	BMenuItem* ftexhtml;

	BMenuItem* fpostscript;
	BMenuItem* fpdf;
	BMenuItem* fhtml;

	BubbleHelper* helper;
	BScrollView* scrollView;
	SplitPane* m_horizontalSplit;
	SplitPane* m_verticalSplit;
	ProjectView* m_projectView;
	BScrollView* docScroll;

	/*			BTabView* tabView;
				BTab* tab;
				int TabIndex;
				int TabNumber;
				*/
	TexView* tv;


	MainTBar* m_toolBar;
	StatusBar* m_statusBar;
	BMessage* printer_settings;
};
#endif
