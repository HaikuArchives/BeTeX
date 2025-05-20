#ifndef _MAIN_TBAR_H_
#define _MAIN_TBAR_H_

#include <View.h>
#include <File.h>
#include <Resources.h>
#include <Bitmap.h>
#include <Roster.h>
#include <Mime.h>
#include "TButton.h"

#include "Constants.h"
#include "ConverterIcons.h"
#include "TexView.h"
#include "ProjectItem.h"
#include <NodeMonitor.h>
#include <Node.h>
#include "BubbleHelper.h"
class MainTBar : public BView
{
	public:
		MainTBar(BRect r,BubbleHelper* h);
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
