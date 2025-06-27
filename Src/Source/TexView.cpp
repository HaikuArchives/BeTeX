#ifndef TEX_VIEW_H
#include "TexView.h"
#endif

#include <StopWatch.h>
#include "Constants.h"

TexView::TexView(BRect frame, BRect textRect)
	: BTextView(frame, "texview", textRect, B_FOLLOW_ALL_SIDES,
		  B_WILL_DRAW | B_PULSE_NEEDED /*|B_NAVIGABLE*/)
{
	IsColSel = false;
	DontSel = false;
	BFont fontie(be_fixed_font);
	f = fontie;
	f.SetSize(preferences->FontSize);
	SetFontAndColor(0, 1, &f, B_FONT_ALL, &preferences->fg_color);
	SetStylable(preferences->IsSyntax);
	SetAlignment(B_ALIGN_LEFT);
	SetDoesUndo(true);
	SetAutoindent(preferences->AutoIndent);
	// IsUndo = false;

	IsNormalFocus = false;
	HasAttachedFocus = false;
	// TextStyles
	green_matches.push_back("textbf");
	green_matches.push_back("textit");
	green_matches.push_back("textmd");
	green_matches.push_back("textrm");
	green_matches.push_back("textsc");
	green_matches.push_back("textsf");
	green_matches.push_back("textsl");
	green_matches.push_back("texttt");
	green_matches.push_back("textup");
	green_matches.push_back("emph");

	green_matches.push_back("bf");
	green_matches.push_back("it");
	green_matches.push_back("em");
	green_matches.push_back("rm");
	green_matches.push_back("sc");
	green_matches.push_back("sf");
	green_matches.push_back("sl");
	green_matches.push_back("tt");
	green_matches.push_back("up");

	// TextSizes
	green_matches.push_back("tiny");
	green_matches.push_back("scriptsize");
	green_matches.push_back("footnotesize");
	green_matches.push_back("small");
	green_matches.push_back("normalsize");
	green_matches.push_back("large");
	green_matches.push_back("Large");
	green_matches.push_back("LARGE");
	green_matches.push_back("huge");
	green_matches.push_back("Huge");

	green_matches.push_back("mathbb");
	green_matches.push_back("mathbf");
	green_matches.push_back("mathcal");
	green_matches.push_back("mathfrak");
	green_matches.push_back("mathit");
	green_matches.push_back("mathnormal");
	green_matches.push_back("mathrm");
	green_matches.push_back("mathsf");
	green_matches.push_back("mathtt");

	green_matches.push_back("rmfamily");
	green_matches.push_back("sffamily");
	green_matches.push_back("ttfamily");

	green_matches.push_back("itshape");
	green_matches.push_back("scshape");
	green_matches.push_back("slshape");
	green_matches.push_back("upshape");

	green_matches.push_back("bfseries");
	green_matches.push_back("mdseries");

	purple_matches.push_back("author");
	purple_matches.push_back("title");
	purple_matches.push_back("section");
	purple_matches.push_back("subsection");
	purple_matches.push_back("subsubsection");
	purple_matches.push_back("subsubsubsection");
	purple_matches.push_back("verb");

	IsShifting = false;
}

TexView::~TexView() {}

void
TexView::Undo(BClipboard* clip)
{
	BTextView::Undo(clip);
	DontSel = false;
	IsShifting = false;
}

void
TexView::ShiftLeft()
{
	int32 start, finish;
	GetSelection(&start, &finish);
	vector<int> sols;
	vector<int> eols;
	FillSolEol(sols, eols, 0, TextLength() - 1);  // start,finish);
	int32 startline = -1;
	int32 endline = -1;
	// int TotalLines = CountLines();
	for (uint i = 0; i < sols.size(); i++) {
		if (start >= sols[i] && start <= eols[i])
			startline = i;
		if (finish > sols[i] && finish <= eols[i])
			endline = i;
		else if (finish >= sols[i] && finish <= eols[i]) {
			endline = i - 1;
			if (start == sols[i])
				endline++;
		}
		if (startline >= 0 && endline >= 0)
			break;
	}
	if (startline < 0 || endline < 0)
		return;

	// Select(0,0);//,start);
	// Highlight(sols[startline],eols[endline]);
	IsShifting = true;
	Select(sols[startline], sols[startline]);
	int tab_width = 3;
	int offset_total = 0;
	int first_offset = 0;
	bool GotFirstOffset = false;
	for (int i = startline; i <= endline; i++) {
		//	sols[i]-= first_offset;
		//	eols[i]-= first_offset;
		int sol = sols[i] - offset_total;
		int eol = eols[i] - offset_total;
		if (  // sol+2 < eol &&
			ByteAt(sol) == ' ' && ByteAt(sol + 1) == ' ' && ByteAt(sol + 2) == ' ') {
			Delete(sol, sol + 3);
			offset_total += 3;
			// if(!GotFirstOffset)
			///{
			//	GotFirstOffset = true;
			first_offset = 3;
			//}
			// if(startline == endline)
			//	Select(sol,eol-2);
		} else if (	 // sol+1 < eol &&
			ByteAt(sol) == ' ' && ByteAt(sol + 1) == ' ') {
			Delete(sol, sol + 2);
			offset_total += 2;
			// if(!GotFirstOffset)
			//{
			//	GotFirstOffset = true;
			first_offset = 2;
			//}
			// if(startline == endline)
			//	Select(sol,eol-1);
		} else if (	 // sol < eol &&
			ByteAt(sol) == ' ') {
			Delete(sol, sol + 1);
			offset_total += 1;
			// if(!GotFirstOffset)
			//{
			//	GotFirstOffset = true;
			first_offset = 1;
			//}
			// if(startline == endline)
			//	Select(sol,eol);
		} else if (	 // sol < eol &&
			ByteAt(sol) == '\t') {
			Delete(sol, sol + 1);
			offset_total += 1;
			// if(!GotFirstOffset)
			//{
			//	GotFirstOffset = true;
			first_offset = 1;
			//}
			// if(startline == endline)
			//	Select(sol,eol);
		}
	}
	// if(startline != endline)
	Select(sols[startline], eols[endline] - offset_total + 1);
	IsShifting = false;
}

void
TexView::ShiftRight()
{
	int32 start, finish;
	GetSelection(&start, &finish);
	vector<int> sols;
	vector<int> eols;
	FillSolEol(sols, eols, 0, TextLength() - 1);
	int32 startline = -1;
	int32 endline = -1;

	for (uint i = 0; i < sols.size(); i++) {
		if (start >= sols[i] && start <= eols[i])
			startline = i;
		if (finish > sols[i] && finish <= eols[i])
			endline = i;
		else if (finish >= sols[i] && finish <= eols[i]) {
			endline = i - 1;
			if (start == sols[i])
				endline++;
		}
		if (startline >= 0 && endline >= 0)
			break;
	}

	if (startline < 0 || endline < 0)
		return;

	IsShifting = true;
	int selection_offset = 0;
	int tab_width = 3;


	int sel_start, sel_finish;
	int offset_total = 0;
	Select(sols[startline], sols[startline]);
	for (int i = startline; i <= endline; i++) {
		int sol = sols[i] + offset_total;
		int eol = eols[i] + offset_total;

		Insert(sol, "   ", tab_width);
		offset_total += tab_width;
	}
	Select(sols[startline], eols[endline] + offset_total + 1);
	IsShifting = false;
}

void
TexView::FrameResized(float w, float h)
{
	// nice resizable text
	if (w > 250 && h > 250) {
		BRect r = BRect(0, 0, w, h);
		r.InsetBy(5, 5);
		SetTextRect(r);
	}
}

bool
TexView::IsBrace(char c)
{
	switch (c) {
		case '{':
		case '[':
		case '}':
		case ']':
			return true;
	}
	return false;
}

void
TexView::AttachedToWindow()
{
	SetViewColor(preferences->bg_color);
	SetColorSpace(B_RGB32);
	BTextView::AttachedToWindow();
	MakeFocus(true);
}

void
TexView::Select(int32 start, int32 finish)
{
	BTextView::Select(start, finish);
	if (!IsShifting) {
		BMessenger msgr(Window());
		msgr.SendMessage(new BMessage(InterfaceConstants::K_UPDATE_STATUSBAR));
	}
}

void
TexView::SetNormalFocus()
{
	IsNormalFocus = true;
}

void
TexView::SetAttachedFocus()
{
	HasAttachedFocus = true;
}

void
TexView::MouseUp(BPoint point)
{
	IsColSel = false;
	BTextView::MouseUp(point);
}

void
TexView::Draw(BRect r)
{
	BTextView::Draw(r);
}

void
TexView::MouseMoved(BPoint point, uint32 transit, const BMessage* msg)
{
	if (!IsNormalFocus) {
		switch (transit) {
			case B_ENTERED_VIEW:
			{
				MakeFocus(true);
			} break;
			case B_EXITED_VIEW:
			{
				MakeFocus(false);
			} break;
		}
	}
	BTextView::MouseMoved(point, transit, msg);
}

/*bool TexView::IsBadUrlChar(int32 h)
{
	if(IsBrace(ByteAt(h)))
		return true;
	if(ByteAt(h) == ' ' || ByteAt(h) == '(' || ByteAt(h) == ')' || ByteAt(h) == '|'
	|| ByteAt(h) == '\\'|| ByteAt(h) == '\n')
		return true;
	return false;
}
bool TexView::IsUrlCommented(int32 start,int32 finish)
{
	text_run_array* tra = RunArray(start,finish);
	if(tra == NULL)
		return false;
	return (tra->count == 1 && tra->runs[0].color == preferences->comment_color);
}
BString TexView::FindUrl(int32 offset)
{
	BString url="";
	if(IsBadUrlChar(offset))
		return url;
	int text_length = TextLength();
	vector<int> sols;
	vector<int> eols;
	FillSolEol(sols,eols,offset,offset+1);

	if(sols.size() != eols.size())
		return url;
	int sol = sols[0];
	int eol = eols[0];


	int i;
	int start=-1;
	//try looking left
	bool DoneSearch=false;
	for(i=offset;i>=sol;i--)
	{
		if(IsBadUrlChar(i))
		{
			start = i+1;
			DoneSearch = true;
			break;
		}
	}
	if(DoneSearch == false)
		start = sol;

	DoneSearch=false;
	int finish = -1;
	for(i=offset;i<eol;i++)
	{
		if(IsBadUrlChar(i))
		{
			finish = i-1;
			DoneSearch = true;
			break;
		}
	}
	if(DoneSearch == false)
		finish = eol-1;


	if(start < 0 || finish < 0 || IsUrlCommented(start,finish))
		return url;

	if(start +1 < text_length &&
	start +2 < text_length &&
	start +3 < text_length &&
	start +4 < text_length &&
	start +5 < text_length &&
	start +6 < text_length &&
	ByteAt(start) == 'h' &&
	ByteAt(start+1) == 't' &&
	ByteAt(start+2) == 't' &&
	ByteAt(start+3) == 'p' &&
	ByteAt(start+4) == ':' &&
	ByteAt(start+5) == '/' &&
	ByteAt(start+6) == '/')
	{
		for(i=start;i<=finish;i++)
		{
			//if(i==finish)
				url <<(char)ByteAt(i);//= strcat(url,(const char*)ByteAt(i));
			//else
			//	strcat(url,(const char*)ByteAt(i));
		}
	}
	return url;

//	return NULL;
	//try looking right



}*/

int32
TexView::CountPhysicalLines()
{
	return BTextView::CountLines();
}

void
TexView::MouseDown(BPoint point)
{
	BTextView::MouseDown(point);
}
void
TexView::KeyDown(const char* bytes, int32 numBytes)
{
	switch (bytes[0]) {
		case B_ENTER:
		{
			// update previous line on enter
			BMessage msg(InterfaceConstants::K_PARSE_LINE);
			int32 start, finish;
			GetSelection(&start, &finish);
			if (msg.AddInt32("start", start) == B_OK && msg.AddInt32("finish", finish) == B_OK)
				Window()->PostMessage(&msg, this);
		} break;
		case B_TAB:
		{
			if (IsNormalFocus) {
				Parent()->NextSibling()->MakeFocus();
				return;
			}
		} break;
	}
	BTextView::KeyDown(bytes, numBytes);
	BMessenger msgr(Window());
	msgr.SendMessage(new BMessage(InterfaceConstants::K_UPDATE_STATUSBAR));
}

int
TexView::FindFirstOnLine(char c, int offset, int eol)
{
	for (int i = offset; i < eol; i++) {
		if (ByteAt(i) == c)
			return i;
	}
	return -1;
}

int32
TexView::OffsetAtIndex(int32 index)
{
	// int text_length = TextLength();
	int i;
	int count = 0;
	int last = 0;
	for (i = 0; count < index; i++) {
		if (ByteAt(i) == '\n') {
			count++;
			last = i + 1;
		}
	}
	return last;
}

int32
TexView::OffsetAt(BPoint point)
{
	return BTextView::OffsetAt(point);
}

int32
TexView::LineAt(int32 offset)
{
	vector<int> sols;
	vector<int> eols;
	FillSolEol(sols, eols, 0, TextLength() - 1);

	for (uint i = 0; i < sols.size(); i++) {
		if (offset >= sols[i] && offset <= eols[i])
			return i;
	}
	return -1;
}

void
TexView::FillSolEol(vector<int>& s, vector<int>& e, int start, int finish)
{
	int i = start;
	int text_length = TextLength();
	for (i = start; i >= 0; i--) {
		if (ByteAt(i) == '\n') {
			break;
		}
	}
	start = i + 1;

	i = finish;
	for (i = finish; i < text_length; i++) {
		if (ByteAt(i) == '\n') {
			break;
		}
	}
	finish = i;

	for (i = start; i <= finish; i++) {
		if (ByteAt(i) == '\n') {
			e.push_back(i);
		}
	}

	e.push_back(i);

	s.push_back(start);
	for (i = 0; i < e.size() - 1; i++) {
		s.push_back(e[i] + 1);
	}
}

void
TexView::GoToLine(int32 index)
{
	if (index < 0 || index > CountLines() || TextLength() <= 0)
		return;

	vector<int> eols;
	vector<int> sols;

	FillSolEol(sols, eols, 0, TextLength() - 1);
	Select(sols[index], eols[index]);
}

int32
TexView::CountLines()
{
	vector<int> eols;
	vector<int> sols;
	FillSolEol(sols, eols, 0, TextLength() - 1);
	return eols.size();
}

void
TexView::ParseAll(int start, int finish, bool IsInteractive)
{
	BFont font(be_fixed_font);
	font.SetSize(preferences->FontSize);

	int text_length = TextLength();
	if (text_length > 0) {
		vector<int> eols;
		vector<int> sols;
		FillSolEol(sols, eols, start, finish);

		int i;
		int size;
		size = text_length;
		vector<rgb_color> colorVec(size, preferences->fg_color);

		SetMathModes(colorVec);

		if (!IsInteractive) {
			for (i = 0; i < sols.size(); i++) {
				ParseLine(sols[i], eols[i], colorVec);
			}

			int offset_size = 1;
			vector<int> offsets;
			offsets.push_back(0);

			for (i = 1; i < size; i++) {
				if (colorVec[i - 1] != colorVec[i]) {
					offsets.push_back(i);
					offset_size++;
				}
			}
			text_run_array* tra = (text_run_array*)malloc(sizeof(text_run_array) * (size));
			tra->count = offset_size;
			for (i = 0; i < offset_size; i++) {
				tra->runs[i].color = colorVec[offsets[i]];
				tra->runs[i].font = font;
				tra->runs[i].offset = offsets[i];
			}
			SetRunArray(0, text_length - 1, tra);
		} else {
			int32 start, finish;
			GetSelection(&start, &finish);
			Select(start, start);
			for (i = 0; i < sols.size(); i++) {
				IParseLine(sols[i], eols[i], colorVec);
			}
			Select(start, finish);
		}
	}
}

void
TexView::IParseLine(int sol, int eol, vector<rgb_color>& colorVecFullText)
{
	int size = eol - sol + 1;
	vector<rgb_color> colorVec(
		colorVecFullText.begin() + sol, colorVecFullText.begin() + sol + size - 1);

	int i;
	int offset = sol;
	int pos;
	int text_length = TextLength();
	// Setup some defaults....
	ITwoColorPlateau('\'', sol, eol, preferences->comma_color, colorVec);
	ITwoColorPlateau('`', sol, eol, preferences->comma_color, colorVec);
	ITwoColorPlateau('\\', sol, eol, preferences->punc_symbol_color, colorVec);

	for (i = sol; i < eol; i++) {
		if (ByteAt(i) == '[' || ByteAt(i) == ']') {
			if (i - 1 >= 0 && ByteAt(i - 1) == '\\') {
				colorVec[i - 1 - sol] = preferences->punc_symbol_color;
			}

			colorVec[i - sol] = preferences->punc_symbol_color;

		} else if (ByteAt(i) == '&' || ByteAt(i) == '{' || ByteAt(i) == '}')  //
		{
			if (i - 1 >= 0 && ByteAt(i - 1) == '\\') {
				colorVec[i - 1 - sol] = preferences->punc_symbol_color;
			}

			colorVec[i - sol] = preferences->punc_symbol_color;

		} else if (ByteAt(i) == '$') {
			if (i - 1 >= 0 && ByteAt(i - 1) == '\\') {
				colorVec[i - 1 - sol] = preferences->fg_color;
				colorVec[i - sol] = preferences->fg_color;
			}
		} else if (ByteAt(i) == '\\' && i + 1 < eol) {
			if (ByteAt(i + 1) == '#') {
				colorVec[i - sol] = preferences->punc_symbol_color;
				colorVec[i + 1 - sol] = preferences->punc_symbol_color;
			} else if (ByteAt(i + 1) == '\'' || ByteAt(i + 1) == '`') {
				colorVec[i - sol] = preferences->fg_color;
				colorVec[i + 1 - sol] = preferences->fg_color;
			}
		}
	}
	offset = sol;

	while ((pos = FindFirstOnLine('%', offset, eol)) >= 0 && offset < eol) {
		if (pos - 1 >= 0 && ByteAt(pos - 1) == '\\') {
			colorVec[pos - 1 - sol] = preferences->punc_symbol_color;
			colorVec[pos - sol] = preferences->punc_symbol_color;
		} else {
			for (i = pos; i < eol; i++)
				colorVec[i - sol] = preferences->comment_color;
			break;
		}
		offset = pos + 1;
	}

	// START COLORING***********************************

	BFont default_font(be_fixed_font);
	default_font.SetSize(preferences->FontSize);

	int plLength = 0;
	int plStart = 0;
	for (i = sol; i < eol; i++) {
		if (i == sol) {
			plLength = 1;
			plStart = i;
		} else if (colorVec[i - sol] != colorVec[i - sol - 1]) {
			// if(colorVec[i-sol-1] != old_colors[i-sol])
			SetFontAndColor(
				plStart, plStart + plLength, &default_font, B_FONT_ALL, &colorVec[i - sol - 1]);

			plLength = 1;
			plStart = i;
		} else {
			plLength++;
		}
	}

	if (plLength > 0)
		SetFontAndColor(
			plStart, plStart + plLength, &default_font, B_FONT_ALL, &colorVec[i - sol - 1]);
}

void
TexView::ParseLine(int sol, int eol, vector<rgb_color>& colorVec)
{
	int i;
	int offset = sol;
	int pos;
	int text_length = TextLength();
	// Setup some defaults....
	TwoColorPlateau('\'', sol, eol, preferences->comma_color, colorVec);
	TwoColorPlateau('`', sol, eol, preferences->comma_color, colorVec);
	TwoColorPlateau('\\', sol, eol, preferences->punc_symbol_color, colorVec);

	for (i = sol; i < eol; i++) {
		if (ByteAt(i) == '[' || ByteAt(i) == ']') {
			if (i - 1 >= 0 && ByteAt(i - 1) == '\\') {
				colorVec[i - 1] = preferences->punc_symbol_color;
			}

			colorVec[i] = preferences->punc_symbol_color;

		} else if (ByteAt(i) == '&' || ByteAt(i) == '{' || ByteAt(i) == '}')  //
		{
			if (i - 1 >= 0 && ByteAt(i - 1) == '\\') {
				colorVec[i - 1] = preferences->punc_symbol_color;
			}

			colorVec[i] = preferences->punc_symbol_color;

		} else if (ByteAt(i) == '$') {
			if (i - 1 >= 0 && ByteAt(i - 1) == '\\') {
				colorVec[i - 1] = preferences->fg_color;
				colorVec[i] = preferences->fg_color;
			}
		} else if (ByteAt(i) == '\\' && i + 1 < eol) {
			if (ByteAt(i + 1) == '#') {
				colorVec[i] = preferences->punc_symbol_color;
				colorVec[i + 1] = preferences->punc_symbol_color;
			} else if (ByteAt(i + 1) == '\'' || ByteAt(i + 1) == '`') {
				colorVec[i] = preferences->fg_color;
				colorVec[i + 1] = preferences->fg_color;
			}
		}
	}
	offset = sol;

	while ((pos = FindFirstOnLine('%', offset, eol)) >= 0 && offset < eol) {
		if (pos - 1 >= 0 && ByteAt(pos - 1) == '\\') {
			colorVec[pos - 1] = preferences->punc_symbol_color;
			colorVec[pos] = preferences->punc_symbol_color;
		} else {
			for (i = pos; i < eol; i++)
				colorVec[i] = preferences->comment_color;
			break;
		}
		offset = pos + 1;
	}
}

void
TexView::TwoColorPlateau(char c, int sol, int eol, rgb_color c1, vector<rgb_color>& colorVec)
{
	int IsPlateau = false;
	int plLength = 0;
	int plStart;
	int i;
	for (i = sol; i < eol; i++) {
		// start a plateau
		if (c == '\\' && !IsPlateau && ByteAt(i) == c) {
			plStart = i;
			plLength++;
			IsPlateau = true;
		} else if (c != '\\' && !IsPlateau && ByteAt(i) == c && i - 1 >= 0
				   && ByteAt(i - 1) != '\\') {
			plStart = i;
			plLength++;
			IsPlateau = true;
		} else if (IsPlateau && ByteAt(i) == c && i - 1 >= 0 && ByteAt(i - 1) == c) {
			plLength++;
		} else if (IsPlateau && ByteAt(i) != c && i - 1 >= 0 && ByteAt(i - 1) == c) {
			if (plLength % 2 == 0) {
				for (int k = plStart; k < plStart + plLength; k++) {
					colorVec[k] = c1;
				}
			} else {
				for (int k = plStart; k < plStart + plLength - 1 && k < eol; k++) {
					colorVec[k] = c1;
				}
				if (c == '\\' && isalpha(ByteAt(i))) {
					BString match = "";

					int j = i;
					for (j = i; j < eol; j++) {
						if (isalpha(ByteAt(j)))
							match << (char)ByteAt(j);
						else
							break;
					}
					if (match.Length() > 0) {
						if (Contains(green_matches, match)) {
							for (int k = i - 1; k < j; k++)
								colorVec[k] = preferences->format_cmd_color;
						} else if (Contains(purple_matches, match)) {
							for (int k = i - 1; k < j; k++)
								colorVec[k] = preferences->special_cmd_color;
						} else {
							for (int k = i - 1; k < j; k++)
								colorVec[k] = preferences->generic_cmd_color;
						}
					}
				} else {
					colorVec[plStart + plLength - 1] = preferences->fg_color;
					colorVec[plStart + plLength] = preferences->fg_color;
				}
			}

			IsPlateau = false;
			plLength = 0;
		}
	}
	if (IsPlateau) {
		if (plLength % 2 == 0) {
			for (int k = plStart; k < plStart + plLength; k++) {
				colorVec[k] = c1;
			}

		} else {
			for (int k = plStart; k < plStart + plLength - 1 && k < eol; k++) {
				colorVec[k] = c1;
			}

			if (c == '\\' && isalpha(ByteAt(i))) {
				BString match = "";

				int j = i;
				for (j = i; j < eol; j++) {
					if (isalpha(ByteAt(j)))
						match << (char)ByteAt(j);
					else
						break;
				}
				if (match.Length() > 0) {
					if (Contains(green_matches, match)) {
						for (int k = i - 1; k < j; k++)
							colorVec[k] = preferences->format_cmd_color;
					} else if (Contains(purple_matches, match)) {
						for (int k = i - 1; k < j; k++)
							colorVec[k] = preferences->special_cmd_color;
					} else {
						for (int k = i - 1; k < j; k++)
							colorVec[k] = preferences->generic_cmd_color;
					}
				}
			} else {
				colorVec[plStart + plLength - 1] = preferences->fg_color;
				colorVec[plStart + plLength] = preferences->fg_color;
			}
		}

		IsPlateau = false;
		plLength = 0;
	}
}
void
TexView::ITwoColorPlateau(char c, int sol, int eol, rgb_color c1, vector<rgb_color>& colorVec)
{
	int IsPlateau = false;
	int plLength = 0;
	int plStart;
	int i;
	for (i = sol; i < eol; i++) {
		// start a plateau
		if (c == '\\' && !IsPlateau && ByteAt(i) == c) {
			plStart = i;
			plLength++;
			IsPlateau = true;
		} else if (c != '\\' && !IsPlateau && ByteAt(i) == c && i - 1 >= 0
				   && ByteAt(i - 1) != '\\') {
			plStart = i;
			plLength++;
			IsPlateau = true;
		} else if (IsPlateau && ByteAt(i) == c && i - 1 >= 0 && ByteAt(i - 1) == c) {
			plLength++;
		} else if (IsPlateau && ByteAt(i) != c && i - 1 >= 0 && ByteAt(i - 1) == c) {
			if (plLength % 2 == 0) {
				for (int k = plStart; k < plStart + plLength; k++) {
					colorVec[k - sol] = c1;
				}
			} else {
				for (int k = plStart; k < plStart + plLength - 1 && k < eol; k++) {
					colorVec[k - sol] = c1;
				}
				colorVec[plStart + plLength - sol] = preferences->fg_color;

				if (c == '\\' && isalpha(ByteAt(i))) {
					BString match = "";

					int j = i;
					for (j = i; j < eol; j++) {
						if (isalpha(ByteAt(j)))
							match << (char)ByteAt(j);
						else
							break;
					}
					if (match.Length() > 0) {
						if (Contains(green_matches, match)) {
							for (int k = i - 1; k < j; k++)
								colorVec[k - sol] = preferences->format_cmd_color;
						} else if (Contains(purple_matches, match)) {
							for (int k = i - 1; k < j; k++)
								colorVec[k - sol] = preferences->special_cmd_color;
						} else {
							for (int k = i - 1; k < j; k++)
								colorVec[k - sol] = preferences->generic_cmd_color;
						}
					}
				} else {
					colorVec[plStart + plLength - 1 - sol] = preferences->fg_color;
					// if(plLength %2 == 0)
					colorVec[plStart + plLength - sol] = preferences->fg_color;
				}
			}

			IsPlateau = false;
			plLength = 0;
		}
	}
	if (IsPlateau) {
		if (plLength % 2 == 0) {
			for (int k = plStart; k < plStart + plLength; k++) {
				colorVec[k - sol] = c1;
			}
		} else {
			for (int k = plStart; k < plStart + plLength - 1 && k < eol; k++) {
				colorVec[k - sol] = c1;
			}
			colorVec[plStart + plLength - sol] = preferences->fg_color;
			if (c == '\\' && isalpha(ByteAt(i))) {
				BString match = "";

				int j = i;
				for (j = i; j < eol; j++) {
					if (isalpha(ByteAt(j)))
						match << (char)ByteAt(j);
					else
						break;
				}
				if (match.Length() > 0) {
					if (Contains(green_matches, match)) {
						for (int k = i - 1; k < j; k++)
							colorVec[k - sol] = preferences->format_cmd_color;
					} else if (Contains(purple_matches, match)) {
						for (int k = i - 1; k < j; k++)
							colorVec[k - sol] = preferences->special_cmd_color;
					} else {
						for (int k = i - 1; k < j; k++)
							colorVec[k - sol] = preferences->generic_cmd_color;
					}
				}
			} else {
				colorVec[plStart + plLength - 1 - sol] = preferences->fg_color;
				//	if(plLength %2 == 0)
				colorVec[plStart + plLength - sol] = preferences->fg_color;
			}
		}


		IsPlateau = false;
		plLength = 0;
	}
}

int
TexView::IsMathMode(int offset, vector<BString>& v, bool IsStart)
{
	for (uint i = 0; i < v.size(); i++) {
		// try all possibilities....
		if (ByteAt(offset) == v[i].ByteAt(0)) {
			// should be fixed to incorporate sol -- maybe not!
			if (offset - 1 >= 0 /*!= sol*/ && ByteAt(offset - 1) == '\\')
				return -1;

			int len = v[i].Length();

			bool IsOk = true;
			int j = 1;
			while (IsOk && j < len) {
				IsOk = (ByteAt(offset + j) == v[i].ByteAt(j));
				j++;
			}
			if (IsOk == true) {
				if (len > 2) {
					if (IsStart)
						return offset + j;
					else
						return offset;
				} else {
					if (IsStart)
						return offset;
					else
						return offset + j;
				}
			}
		}
	}
	return -1;
}

void
TexView::SetMathModes(vector<rgb_color>& colorVec)
{
	vector<BString> start;
	vector<BString> end;

	start.push_back("$");
	start.push_back("\\[");
	start.push_back("\\begin{eqnarray*}");
	start.push_back("\\begin{eqnarray}");
	start.push_back("\\begin{equation}");
	start.push_back("\\begin{displaymath}");
	start.push_back("\\begin{math}");
	start.push_back("\\begin{verbatim}");

	end.push_back("$");
	end.push_back("\\]");
	end.push_back("\\end{eqnarray*}");
	end.push_back("\\end{eqnarray}");
	end.push_back("\\end{equation}");
	end.push_back("\\end{displaymath}");
	end.push_back("\\end{math}");
	end.push_back("\\end{verbatim}");

	int i;
	int32 spos;
	int32 fpos = -1;
	int length = TextLength();

	for (i = 0; i < length; i++) {
		if ((spos = IsMathMode(i, start, true)) >= 0) {
			int k = spos + 1;
			for (k = spos + 1; k < length; k++) {
				if ((fpos = IsMathMode(k, end, false)) >= 0)
					break;
			}
			if (fpos == -1)
				fpos = length;

			for (int p = spos; p < fpos; p++) {
				colorVec[p] = preferences->math_mode_color;
			}
			i = k;
		}
	}
}

void
TexView::InsertText(const char* text, int32 length, int32 offset, const text_run_array* runner)
{
	BTextView::InsertText(text, length, offset, NULL);

	if (!IsShifting && text[0] != B_ENTER) {
		BMessage msg(InterfaceConstants::K_ANALYSE_TEXVIEW);
		if (msg.AddInt32("offset", offset) == B_OK && msg.AddInt32("length", length) == B_OK)
			Window()->PostMessage(&msg, this);
	}

	Window()->PostMessage(new BMessage(InterfaceConstants::K_FILE_CONTENTS_CHANGED));
}

void
TexView::DeleteText(int32 start, int32 finish)
{
	if (!IsShifting) {
		BMessage msg(InterfaceConstants::K_ANALYSE_TEXVIEW);
		if (msg.AddInt32("start", start) == B_OK && msg.AddInt32("finish", finish) == B_OK)
			Window()->PostMessage(&msg, this);
	}
	BTextView::DeleteText(start, finish);
	Window()->PostMessage(new BMessage(InterfaceConstants::K_FILE_CONTENTS_CHANGED));
}


void
TexView::SetText(const char* text, int32 length, const text_run_array* runs)
{
	BTextView::SetText(text, length, runs);
	ParseAll(0, length - 1, false);
}
void
TexView::UpdateColors()
{
	SetViewColor(preferences->bg_color);
	Invalidate();
	SetFontAndColor(0, TextLength(), &f, B_FONT_ALL, &preferences->fg_color);
	ParseAll(0, TextLength() - 1, true);
}

void
TexView::UpdateFontSize()
{
	f.SetSize(preferences->FontSize);
	SetFontAndColor(0, TextLength(), &f, B_FONT_ALL, &preferences->fg_color);
	ParseAll(0, TextLength() - 1, true);
}

void
TexView::SetText(BFile* file, int32 offset, int32 length, const text_run_array* runs)
{
	BTextView::SetText(file, offset, length, runs);
	ParseAll(offset, length - 1, false);
}

bool
TexView::Contains(vector<BString>& v, BString s)
{
	for (int i = 0; i < v.size(); i++) {
		if (v[i] == s)
			return true;
	}
	return false;
}

bool
TexView::CanEndLine(int32 offset)
{
	if (ByteAt(offset) == B_ENTER)
		return true;
	else
		return false;
}

int32
TexView::GetMathSol(int sol)
{
	/*bool IsMultiLine;
	vector<BString> start;
	start.push_back("$");
	start.push_back("\\[");
	start.push_back("\\begin{eqnarray*}");
	start.push_back("\\begin{eqnarray}");
	start.push_back("\\begin{equation}");
	start.push_back("\\begin{displaymath}");
	start.push_back("\\begin{math}");
	const char* text;
	int32 text_length = TextLength();
	int retval=-1;
	for(int i=sol;i>=0;i--)
	{
		if(retval = (IsMathMode(i,i,0,start,true,IsMultiLine) > 0))
			return retval;
	}
	return retval;
	*/
	return sol;
}

// #pragma mark -
void
TexView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case InterfaceConstants::K_UNDO_HIGHLIGHTING:
		{
			int32 start, finish;
			if (msg->FindInt32("start", &start) == B_OK
				&& msg->FindInt32("finish", &finish) == B_OK) {
				Select(start, finish);
			}
		} break;
		case InterfaceConstants::K_PARSE_LINE:
		{
			int32 start, finish;
			if (msg->FindInt32("start", &start) == B_OK
				&& msg->FindInt32("finish", &finish) == B_OK) {
				int32 sel_start, sel_finish;
				GetSelection(&sel_start, &sel_finish);
				ParseAll(min(sel_start, start), max(sel_finish, finish), true);
			}
		} break;
		case InterfaceConstants::K_ANALYSE_TEXVIEW:
		{
			int32 start, finish;
			if (msg->FindInt32("start", &start) == B_OK
				&& msg->FindInt32("finish", &finish) == B_OK) {
				ParseAll(start, finish, true);
			}

			int32 offset, length;
			if (msg->FindInt32("offset", &offset) == B_OK
				&& msg->FindInt32("length", &length) == B_OK) {
				GetSelection(&start, &finish);
				ParseAll(offset, finish, true);
			}
		} break;
		case B_CUT:
		case B_COPY:
		case B_PASTE:
		{
			BMessage msg(InterfaceConstants::K_UPDATE_CLIPBOARD_MENU_STATUS);
			Window()->PostMessage(&msg, this);
		}
		case B_OBSERVER_NOTICE_CHANGE:
		{
			if (msg->GetInt32(B_OBSERVE_ORIGINAL_WHAT, 0)
				== (int32)PrefsConstants::K_PREFS_UPDATE) {
				UpdateColors();
			} else {
				BTextView::MessageReceived(msg);
			}
		} break;
		default:
			BTextView::MessageReceived(msg);
			break;
	}
}

void
TexView::LoadFile(entry_ref* ref)
{
	if (ref == NULL) {
		return;
	}

	BFile file(ref, B_READ_ONLY);
	if (file.InitCheck() != B_OK) {
		return;
	}

	off_t length;
	file.GetSize(&length);
	if (length == 0) {
		return;
	}

	SetText(&file, 0, length);
}
