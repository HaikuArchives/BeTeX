#ifndef TEX_VIEW_H
#define TEX_VIEW_H

#include <be/app/Message.h>
#include <be/interface/Font.h>
#include <be/interface/Point.h>
#include <be/interface/Rect.h>
#include <be/interface/TextView.h>
#include <be/storage/Entry.h>
#include <be/storage/File.h>
#include <be/support/String.h>

#include <Messenger.h>
#include <Region.h>
#include <Roster.h>
#include <ScrollView.h>
#include <View.h>
#include <Window.h>

#include <stdlib.h>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <vector>
using std::max;
using std::min;
using std::vector;

#include "Prefs.h"

class TexView : public BTextView {
public:
	TexView(BRect frame, BRect textRect);
	virtual ~TexView();

	virtual void AttachedToWindow();
	virtual void MessageReceived(BMessage* message);
	virtual void KeyDown(const char* bytes, int32 numBytes);
	virtual void FrameResized(float w, float h);
	virtual bool CanEndLine(int32 offset);
	virtual void MouseMoved(BPoint point, uint32 transit, const BMessage* msg);
	virtual void MouseDown(BPoint point);
	virtual void MouseUp(BPoint point);
	virtual void Draw(BRect r);
	void SetText(const char* text, int32 length, const text_run_array* runs = NULL);
	void SetText(BFile* file, int32 offset, int32 length, const text_run_array* runs = NULL);
	virtual void Select(int32 start, int32 finish);

	void SetNormalFocus();
	void SetAttachedFocus();
	int32 OffsetAtIndex(int32 index);
	int32 OffsetAt(BPoint point);
	int32 LineAt(int32 offset);
	virtual void Undo(BClipboard* clip);

	void FillSolEol(vector<int>& s, vector<int>& e, int start, int finish);
	void TwoColorPlateau(char c, int sol, int eol, rgb_color c1, vector<rgb_color>& colorVec);
	bool Contains(vector<BString>& v, BString s);
	bool IsBrace(char c);
	void ShiftLeft();
	void ShiftRight();
	int IsMathMode(int offset, vector<BString>& v, bool IsStart);
	void SetMathModes(vector<rgb_color>& colorVec);
	int32 GetMathSol(int sol);

	void ParseLine(int sol, int eol, vector<rgb_color>& colorVec);
	void IParseLine(int sol, int eol, vector<rgb_color>& colorVecFullText);
	void ITwoColorPlateau(char c, int sol, int eol, rgb_color c1, vector<rgb_color>& colorVec);
	void ParseAll(int start, int finish, bool IsInteractive);
	void LoadFile(entry_ref* ref);

	int FindFirstOnLine(char c, int offset, int eol);
	void UpdateColors();
	void UpdateFontSize();

	void GoToLine(int32 index);
	int32 CountLines();
	int32 CountPhysicalLines();

private:
	bool IsShifting, DontSel, IsColSel, IsNormalFocus, HasAttachedFocus;

	BPoint ColStart, ColFinish;

	BString fCache, fText;

	vector<BString> green_matches, purple_matches;

	BFont f;

protected:
	virtual void InsertText(
		const char* text, int32 length, int32 offset, const text_run_array* runner);
	virtual void DeleteText(int32 start, int32 finish);
};

#endif
