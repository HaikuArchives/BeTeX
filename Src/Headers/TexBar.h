#ifndef _TEXBAR_H_
#define _TEXBAR_H_

#include <View.h>
#include <File.h>
#include <Resources.h>
#include <Bitmap.h>
#include <Roster.h>
#include <Mime.h>
#include <String.h>
#include <ScrollBar.h>
#include <InterfaceDefs.h>
#include <vector>
using std::vector;

#include "TButton.h"
//#include "TexBarItem.h"
#include "Constants.h"
#include "Preferences.h"
#include "BubbleHelper.h"
//#include "CharRenderer.h"
#include "HeaderItem.h"

struct sextet
{
	const char* s;
	int n;
	float w;
	float h;
	const unsigned char* p;
	const char* cmd;
};

#include "Greek.h"
#include "Big.h"
#include "Binary.h"
#include "Misc.h"
#include "BinRel.h"
#include "MathModeAccents.h"
#include "International.h"
#include "TButton.h"
#include "BubbleHelper.h"
#include "MessageFields.h"

class TexBar : public BView
{
	public:
		TexBar(BRect r,BubbleHelper* h);
		//virtual void Draw(BRect r);
		virtual void AttachedToWindow();
		virtual void MessageReceived(BMessage* msg);
		void ScrollBy(float h,float v);
		virtual void ScrollTo(BPoint where);
		void AdjustScrollBar();
		void RefreshColors();
		virtual void FrameResized(float w, float h);
		virtual void MouseMoved(BPoint point,uint32 transit,const BMessage* msg);
		void Tile(BView* v,bool IsOnlyResizing);

		HeaderItem* GetGreekHeader();

		HeaderItem* GetBigHeader();

		HeaderItem* GetBinaryHeader();

		HeaderItem* GetMiscHeader();

		HeaderItem* GetBinRelHeader();

		HeaderItem* GetMMAHeader();

		HeaderItem* GetIntlHeader();

		HeaderItem* GetTypeFaceHeader();

		HeaderItem* GetFunctionHeader();

	private:
		void UpdateHeaders();
		Prefs* prefs;
		BubbleHelper* helper;
		/*TexBarItem* Greek;
		TexBarItem* BigOp;
		TexBarItem* BinaryOp;
		TexBarItem* MiscSym;
		*/

		TButton* temp_button;

		HeaderItem* GreekHeader;
		bool IsGreekHidden;

		HeaderItem* BigHeader;
		bool IsBigHidden;

		HeaderItem* BinaryHeader;
		bool IsBinaryHidden;

		HeaderItem* MiscHeader;
		bool IsMiscHidden;

		HeaderItem* BinRelHeader;
		bool IsBinRelHidden;

		HeaderItem* MMAHeader;
		bool IsMMAHidden;

		HeaderItem* IntlHeader;
		bool IsIntlHidden;

		HeaderItem* TypeFaceHeader;
		bool IsTypeFaceHidden;

		HeaderItem* FunctionHeader;
		bool IsFunctionHidden;

		//if these are  true, we don't even create their header objects
		bool IsGreekAbsent;
		bool IsBigAbsent;
		bool IsBinaryAbsent;
		bool IsMiscAbsent;
		bool IsBinRelAbsent;
		bool IsMMAAbsent;
		bool IsIntlAbsent;
		bool IsTypeFaceAbsent;
		bool IsFunctionAbsent;

		//BubbleHelper* helper;
		vector<BBitmap*> greekbitvec;
		vector<BBitmap*> bigbitvec;
		vector<BBitmap*> binarybitvec;
		vector<BBitmap*> miscbitvec;
		vector<BBitmap*> bin_relbitvec;
		vector<BBitmap*> mmaccbitvec;
		vector<BBitmap*> intlbitvec;
		bool IsResizing;
		float TotalHeight;

};

#endif
