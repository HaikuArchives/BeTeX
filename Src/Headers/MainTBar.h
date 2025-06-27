#ifndef _MAIN_TBAR_H_
#define _MAIN_TBAR_H_

#include <Bitmap.h>
#include <File.h>
#include <Mime.h>
#include <Resources.h>
#include <Roster.h>
#include <View.h>
#include "TButton.h"

#include <Node.h>
#include <NodeMonitor.h>
#include "BubbleHelper.h"
#include "Constants.h"
#include "ConverterIcons.h"
#include "ProjectItem.h"
#include "TexView.h"
class MainTBar : public BView {
public:
	MainTBar(BRect r, BubbleHelper* h);
	virtual void Draw(BRect r);

	TButton* TBTexDvi;
	TButton* TBDviPdf;
	TButton* TBDviPs;
	TButton* TBPsPdf;
	TButton* TBTexPdf;
	TButton* TBTexHtml;

	TButton* TBPrevDVI;
	TButton* TBPrevPS;
	TButton* TBPrevPDF;
	TButton* TBPrevHTML;
	TButton* TBNewDoc;
	TButton* TBOpen;
	TButton* TBSave;
	TButton* TBPrint;

	TButton* TBDelTmp;
	TButton* TBEmptyTmp;
	TButton* TBViewLog;

	TButton* TBOpenTracker;
	TButton* TBOpenTerminal;

private:
	BubbleHelper* helper;

	BPath parent_dir;
	BPath TexPath;
	BString basename;
};

#endif
