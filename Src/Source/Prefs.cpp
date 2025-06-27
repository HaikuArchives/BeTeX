#include "Prefs.h"
#include "MainWindow.h"

Prefs::Prefs()
	: BArchivable()
{
	ResetToDefaults();
}

Prefs::Prefs(BMessage* archive)
{
	rgb_color* rc_tmp;
	bool b_tmp;
	BString str_tmp;
	int32 i_tmp;
	ssize_t size;

	if (archive->FindData("fg_color", B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size) == B_OK)
		fg_color = *rc_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindData("bg_color", B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size) == B_OK)
		bg_color = *rc_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindData("generic_cmd_color", B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size)
		== B_OK)
		generic_cmd_color = *rc_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindData("format_cmd_color", B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size)
		== B_OK)
		format_cmd_color = *rc_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindData("special_cmd_color", B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size)
		== B_OK)
		special_cmd_color = *rc_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindData("math_mode_color", B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size)
		== B_OK)
		math_mode_color = *rc_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindData("comma_color", B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size) == B_OK)
		comma_color = *rc_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindData("comment_color", B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size) == B_OK)
		comment_color = *rc_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindData("punc_symbol_color", B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size)
		== B_OK)
		punc_symbol_color = *rc_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindData("html_color", B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size) == B_OK)
		html_color = *rc_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindData(K_BUBBLE_COLOR, B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size) == B_OK)
		bubble_color = *rc_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindData(K_RGBTEXT_COLOR, B_RGB_COLOR_TYPE, (const void**)&rc_tmp, &size) == B_OK)
		RGBText_color = *rc_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("AutoIndent", &b_tmp) == B_OK)
		AutoIndent = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsSyntax", &b_tmp) == B_OK)
		IsSyntax = b_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindBool("IsCaseSensitive", &b_tmp) == B_OK)
		IsCaseSensitive = b_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindBool("IsWrapAround", &b_tmp) == B_OK)
		IsWrapAround = b_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindBool("IsSearchBackwards", &b_tmp) == B_OK)
		IsSearchBackwards = b_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindBool("IsAllDocs", &b_tmp) == B_OK)
		IsAllDocs = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindString("latex_cmd", &str_tmp) == B_OK)
		commands["latex_cmd"] = str_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindString("dvipdf_cmd", &str_tmp) == B_OK)
		commands["dvipdf_cmd"] = str_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindString("dvips_cmd", &str_tmp) == B_OK)
		commands["dvips_cmd"] = str_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindString("ps2pdf_cmd", &str_tmp) == B_OK)
		commands["ps2pdf_cmd"] = str_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindString("pdflatex_cmd", &str_tmp) == B_OK)
		commands["pdflatex_cmd"] = str_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindString("latex2html_cmd", &str_tmp) == B_OK)
		commands["latex2html_cmd"] = str_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindString("postscript_cmd", &str_tmp) == B_OK)
		commands["postscript_cmd"] = str_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindInt32("FontSize", &i_tmp) == B_OK)
		FontSize = i_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindBool("IsTexBarInverted", &b_tmp) == B_OK)
		IsTexBarInverted = b_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindBool("IsGreekHidden", &b_tmp) == B_OK)
		IsGreekHidden = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsBigHidden", &b_tmp) == B_OK)
		IsBigHidden = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsBinaryHidden", &b_tmp) == B_OK)
		IsBinaryHidden = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsMiscHidden", &b_tmp) == B_OK)
		IsMiscHidden = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsBinRelHidden", &b_tmp) == B_OK)
		IsBinRelHidden = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsMMAHidden", &b_tmp) == B_OK)
		IsMMAHidden = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsIntlHidden", &b_tmp) == B_OK)
		IsIntlHidden = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsTypeFaceHidden", &b_tmp) == B_OK)
		IsTypeFaceHidden = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsFunctionHidden", &b_tmp) == B_OK)
		IsFunctionHidden = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsGreekAbsent", &b_tmp) == B_OK)
		IsGreekAbsent = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsBigAbsent", &b_tmp) == B_OK)
		IsBigAbsent = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsBinaryAbsent", &b_tmp) == B_OK)
		IsBinaryAbsent = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsMiscAbsent", &b_tmp) == B_OK)
		IsMiscAbsent = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsBinRelAbsent", &b_tmp) == B_OK)
		IsBinRelAbsent = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsMMAAbsent", &b_tmp) == B_OK)
		IsMMAAbsent = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsIntlAbsent", &b_tmp) == B_OK)
		IsIntlAbsent = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsTypeFaceAbsent", &b_tmp) == B_OK)
		IsTypeFaceAbsent = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindBool("IsFunctionAbsent", &b_tmp) == B_OK)
		IsFunctionAbsent = b_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindBool("IsRecentDocsPathShown", &b_tmp) == B_OK)
		IsRecentDocsPathShown = b_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindBool("IsActivationOk", &b_tmp) == B_OK)
		IsActivationOk = b_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindBool("IsLatinOk", &b_tmp) == B_OK)
		IsLatinOk = b_tmp;
	else {
		ResetToDefaults();
	}

	if (archive->FindString("NumRecentDocs", &str_tmp) == B_OK)
		NumRecentDocs = str_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindString("Rows", &str_tmp) == B_OK)
		Rows = str_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindString("Cols", &str_tmp) == B_OK)
		Cols = str_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindString("dateFlags", &str_tmp) == B_OK)
		dateFlags = str_tmp;
	else {
		ResetToDefaults();
	}
	if (archive->FindRect("main_window_rect", &main_window_rect) != B_OK) {
		ResetToDefaults();
	}
	BMessage tmp_msg;
	if (archive->FindMessage("splitmsg", &tmp_msg) == B_OK) {
		splitmsg = new BMessage(tmp_msg);
	} else {
		ResetToDefaults();
	}

	tmp_msg.MakeEmpty();

	if (archive->FindMessage("split_leftmsg", &tmp_msg) == B_OK) {
		split_leftmsg = new BMessage(tmp_msg);
	} else {
		ResetToDefaults();
	}
}

Prefs::~Prefs() {}

status_t
Prefs::Archive(BMessage* archive, bool deep) const
{
	status_t result = BArchivable::Archive(archive, deep);

	if (result != B_OK)
		return result;

	archive->AddData("fg_color", B_RGB_COLOR_TYPE, &fg_color, sizeof(rgb_color));
	archive->AddData("bg_color", B_RGB_COLOR_TYPE, &bg_color, sizeof(rgb_color));
	archive->AddData("generic_cmd_color", B_RGB_COLOR_TYPE, &generic_cmd_color, sizeof(rgb_color));
	archive->AddData("format_cmd_color", B_RGB_COLOR_TYPE, &format_cmd_color, sizeof(rgb_color));
	archive->AddData("special_cmd_color", B_RGB_COLOR_TYPE, &special_cmd_color, sizeof(rgb_color));
	archive->AddData("math_mode_color", B_RGB_COLOR_TYPE, &math_mode_color, sizeof(rgb_color));
	archive->AddData("comma_color", B_RGB_COLOR_TYPE, &comma_color, sizeof(rgb_color));
	archive->AddData("comment_color", B_RGB_COLOR_TYPE, &comment_color, sizeof(rgb_color));
	archive->AddData("punc_symbol_color", B_RGB_COLOR_TYPE, &punc_symbol_color, sizeof(rgb_color));
	archive->AddData("html_color", B_RGB_COLOR_TYPE, &html_color, sizeof(rgb_color));

	archive->AddData(K_RGBTEXT_COLOR, B_RGB_COLOR_TYPE, &RGBText_color, sizeof(rgb_color));
	archive->AddInt32("FontSize", FontSize);

	archive->AddData(K_BUBBLE_COLOR, B_RGB_COLOR_TYPE, &bubble_color, sizeof(rgb_color));


	archive->AddBool("AutoIndent", AutoIndent);
	archive->AddBool("IsSyntax", IsSyntax);

	archive->AddBool("IsCaseSensitive", IsCaseSensitive);
	archive->AddBool("IsWrapAround", IsWrapAround);
	archive->AddBool("IsSearchBackwards", IsSearchBackwards);
	archive->AddBool("IsAllDocs", IsAllDocs);

	archive->AddString("latex_cmd", commands.find("latex_cmd")->second);
	archive->AddString("dvipdf_cmd", commands.find("dvipdf_cmd")->second);
	archive->AddString("dvips_cmd", commands.find("dvips_cmd")->second);
	archive->AddString("ps2pdf_cmd", commands.find("ps2pdf_cmd")->second);
	archive->AddString("pdflatex_cmd", commands.find("pdflatex_cmd")->second);
	archive->AddString("latex2html_cmd", commands.find("latex2html_cmd")->second);
	archive->AddString("postscript_cmd", commands.find("postscript_cmd")->second);

	archive->AddBool("IsGreekHidden", IsGreekHidden);
	archive->AddBool("IsBigHidden", IsBigHidden);
	archive->AddBool("IsBinaryHidden", IsBinaryHidden);
	archive->AddBool("IsMiscHidden", IsMiscHidden);
	archive->AddBool("IsBinRelHidden", IsBinRelHidden);
	archive->AddBool("IsMMAHidden", IsMMAHidden);
	archive->AddBool("IsIntlHidden", IsIntlHidden);
	archive->AddBool("IsTypeFaceHidden", IsTypeFaceHidden);
	archive->AddBool("IsFunctionHidden", IsFunctionHidden);

	archive->AddBool("IsGreekAbsent", IsGreekAbsent);
	archive->AddBool("IsBigAbsent", IsBigAbsent);
	archive->AddBool("IsBinaryAbsent", IsBinaryAbsent);
	archive->AddBool("IsMiscAbsent", IsMiscAbsent);
	archive->AddBool("IsBinRelAbsent", IsBinRelAbsent);
	archive->AddBool("IsMMAAbsent", IsMMAAbsent);
	archive->AddBool("IsIntlAbsent", IsIntlAbsent);
	archive->AddBool("IsTypeFaceAbsent", IsTypeFaceAbsent);
	archive->AddBool("IsFunctionAbsent", IsFunctionAbsent);

	archive->AddBool("IsTexBarInverted", IsTexBarInverted);

	archive->AddBool("IsRecentDocsPathShown", IsRecentDocsPathShown);
	archive->AddBool("IsActivationOk", IsActivationOk);
	archive->AddBool("IsLatinOk", IsLatinOk);

	archive->AddString("NumRecentDocs", NumRecentDocs);
	archive->AddString("Cols", Cols);
	archive->AddString("Rows", Rows);
	archive->AddString("dateFlags", dateFlags);

	archive->AddRect("main_window_rect", main_window_rect);

	archive->AddMessage("splitmsg", splitmsg);			  // == B_OK &&
	archive->AddMessage("split_leftmsg", split_leftmsg);  // == B_OK)

	return B_OK;
}

void
Prefs::ResetColors()
{
	fg_color = ColorConstants::K_BLACK;
	bg_color = ColorConstants::K_WHITE;
	generic_cmd_color = ColorConstants::K_RED;
	format_cmd_color = ColorConstants::K_GREEN;
	special_cmd_color = ColorConstants::K_PURPLE;
	math_mode_color = ColorConstants::K_NAVY_BLUE;
	comma_color = ColorConstants::K_RED;
	comment_color = ColorConstants::K_BLUE;
	punc_symbol_color = ColorConstants::K_PURPLE;
	html_color = ColorConstants::K_ORANGE;
	RGBText_color = ColorConstants::K_BLACK;

	FontSize = 12;
}

void
Prefs::ResetCommands()
{
	commands["latex_cmd"] = "latex $.tex";
	commands["dvipdf_cmd"] = "dvipdf $.dvi";
	commands["dvips_cmd"] = "dvips -o $.ps $.dvi";
	commands["ps2pdf_cmd"] = "ps2pdf $.ps";
	commands["pdflatex_cmd"] = "pdflatex $.tex";
	commands["latex2html_cmd"] = "latex2html $.tex";
	commands["postscript_cmd"] = "BGhostview $.ps";
}

void
Prefs::ResetToolbar()
{
	IsGreekHidden = false;
	IsBigHidden = true;
	IsBinaryHidden = true;
	IsMiscHidden = true;
	IsBinRelHidden = true;
	IsMMAHidden = true;
	IsIntlHidden = true;
	IsTypeFaceHidden = true;
	IsFunctionHidden = true;

	IsGreekAbsent = false;
	IsBigAbsent = false;
	IsBinaryAbsent = false;
	IsMiscAbsent = false;
	IsBinRelAbsent = false;
	IsMMAAbsent = false;
	IsIntlAbsent = false;
	IsTypeFaceAbsent = false;
	IsFunctionAbsent = false;

	IsTexBarInverted = true;
}

void
Prefs::ResetGeneral()
{
	bubble_color = ColorConstants::K_BUBBLE_DEFAULT;

	AutoIndent = true;
	IsSyntax = true;

	IsCaseSensitive = false;
	IsWrapAround = false;
	IsSearchBackwards = false;
	IsAllDocs = false;
	IsRecentDocsPathShown = true;
	IsActivationOk = true;

	NumRecentDocs = "20";
	IsLatinOk = false;

	Rows = "4";
	Cols = "4";
	dateFlags = "";
}

void
Prefs::ResetLayout()
{
	BScreen screen;
	if (screen.IsValid()) {
		BRect f = screen.Frame();
		main_window_rect = BRect(f.left + 4, f.top + 24, f.right - 5, f.bottom - 5);
	} else {
		main_window_rect = BRect(0, 0, 500, 500);
	}
	splitmsg = NULL;
	split_leftmsg = NULL;
}

void
Prefs::ResetToDefaults()
{
	ResetColors();
	ResetCommands();
	ResetToolbar();
	ResetGeneral();
	ResetLayout();
}

BArchivable*
Prefs::Instantiate(BMessage* archive)
{
	if (!validate_instantiation(archive, "Prefs"))
		return NULL;
	return new Prefs(archive);
}

Prefs* preferences = new Prefs();
