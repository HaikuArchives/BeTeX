#include "Prefs.h"

Prefs::Prefs()
{
	settings_path = "/boot/home/config/settings/settings.betex";
	Load();
}

Prefs::~Prefs()
{
	//Save();
}

void Prefs::Load()
{
	//cout << "Load()" << endl;
	//cout << "load" << endl;
	BFile file(settings_path.String(),B_READ_ONLY);
	if(file.InitCheck() == B_OK)
	{	
		//cout << "loading..." << endl;
		BMessage msg;
		msg.Unflatten(&file);
		//Load the settings
		
		rgb_color* rc_tmp;
		bool b_tmp;
		BString str_tmp;
		int32 i_tmp;
		ssize_t size;

		if(msg.FindData("fg_color", B_RGB_COLOR_TYPE, (const void **)&rc_tmp, &size) == B_OK)
			fg_color = *rc_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindData("bg_color", B_RGB_COLOR_TYPE, (const void **)&rc_tmp, &size) == B_OK)
			bg_color = *rc_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindData("generic_cmd_color", B_RGB_COLOR_TYPE, (const void **)&rc_tmp, &size) == B_OK)
			generic_cmd_color = *rc_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindData("format_cmd_color", B_RGB_COLOR_TYPE, (const void **)&rc_tmp, &size) == B_OK)
			format_cmd_color = *rc_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindData("special_cmd_color", B_RGB_COLOR_TYPE, (const void **)&rc_tmp, &size) == B_OK)
			special_cmd_color = *rc_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindData("math_mode_color", B_RGB_COLOR_TYPE, (const void **)&rc_tmp, &size) == B_OK)
			math_mode_color = *rc_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindData("comma_color", B_RGB_COLOR_TYPE, (const void **)&rc_tmp, &size) == B_OK)
			comma_color = *rc_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindData("comment_color", B_RGB_COLOR_TYPE, (const void **)&rc_tmp, &size) == B_OK)
			comment_color = *rc_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindData("punc_symbol_color", B_RGB_COLOR_TYPE, (const void **)&rc_tmp, &size) == B_OK)
			punc_symbol_color = *rc_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		if(msg.FindData("html_color", B_RGB_COLOR_TYPE, (const void **)&rc_tmp, &size) == B_OK)
			html_color = *rc_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
	
		if(msg.FindBool("AutoIndent",&b_tmp) == B_OK)
			AutoIndent = b_tmp;
		else
		{
			ResetToDefaults();
			return;
		}

		if(msg.FindBool("IsSyntax",&b_tmp) == B_OK)
			IsSyntax = b_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		if(msg.FindBool("IsCaseSensitive",&b_tmp) == B_OK)
			IsCaseSensitive = b_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		if(msg.FindBool("IsWrapAround",&b_tmp) == B_OK)
			IsWrapAround = b_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		if(msg.FindBool("IsSearchBackwards",&b_tmp) == B_OK)
			IsSearchBackwards = b_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		if(msg.FindBool("IsAllDocs",&b_tmp) == B_OK)
			IsAllDocs = b_tmp;
		else
		{
			ResetToDefaults();
			return;
		}

		if(msg.FindString("latex_cmd",&str_tmp) == B_OK)
			latex_cmd = str_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindString("dvipdf_cmd",&str_tmp) == B_OK)
			dvipdf_cmd = str_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindString("dvips_cmd",&str_tmp) == B_OK)
			dvips_cmd = str_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindString("ps2pdf_cmd",&str_tmp) == B_OK)
			ps2pdf_cmd = str_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindString("pdflatex_cmd",&str_tmp) == B_OK)
			pdflatex_cmd = str_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindString("latex2html_cmd",&str_tmp) == B_OK)
			latex2html_cmd = str_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		if(msg.FindString("postscript_cmd",&str_tmp) == B_OK)
			postscript_cmd = str_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		
		if(msg.FindInt32("FontSize",&i_tmp) == B_OK)
			FontSize = i_tmp;
		else
		{
			ResetToDefaults();
			return;
		}
		//cout << "loaded" << endl;
	}	
	else
	{
		//cout << "resetting defaults" << endl;
		ResetToDefaults();
	}
	file.Unset();
}

void Prefs::Save()
{
	//cout << "Save()" << endl;
	BMessage msg;
	msg.AddData("fg_color",B_RGB_COLOR_TYPE,&fg_color,sizeof(rgb_color));
	msg.AddData("bg_color",B_RGB_COLOR_TYPE,&bg_color,sizeof(rgb_color));
	msg.AddData("generic_cmd_color",B_RGB_COLOR_TYPE,&generic_cmd_color,sizeof(rgb_color));
	msg.AddData("format_cmd_color",B_RGB_COLOR_TYPE,&format_cmd_color,sizeof(rgb_color));
	msg.AddData("special_cmd_color",B_RGB_COLOR_TYPE,&special_cmd_color,sizeof(rgb_color));
	msg.AddData("math_mode_color",B_RGB_COLOR_TYPE,&math_mode_color,sizeof(rgb_color));
	msg.AddData("comma_color",B_RGB_COLOR_TYPE,&comma_color,sizeof(rgb_color));
	msg.AddData("comment_color",B_RGB_COLOR_TYPE,&comment_color,sizeof(rgb_color));
	msg.AddData("punc_symbol_color",B_RGB_COLOR_TYPE,&punc_symbol_color,sizeof(rgb_color));
	msg.AddData("html_color",B_RGB_COLOR_TYPE,&html_color,sizeof(rgb_color));
	
	msg.AddInt32("FontSize",FontSize);
	
	msg.AddBool("AutoIndent",AutoIndent);
	msg.AddBool("IsSyntax",IsSyntax);

	msg.AddBool("IsCaseSensitive",IsCaseSensitive);
	msg.AddBool("IsWrapAround",IsWrapAround);
	msg.AddBool("IsSearchBackwards",IsSearchBackwards);
	msg.AddBool("IsAllDocs",IsAllDocs);

	msg.AddString("latex_cmd",latex_cmd);
	msg.AddString("dvipdf_cmd",dvipdf_cmd);
	msg.AddString("dvips_cmd",dvips_cmd);
	msg.AddString("ps2pdf_cmd",ps2pdf_cmd);
	msg.AddString("pdflatex_cmd",pdflatex_cmd);
	msg.AddString("latex2html_cmd",latex2html_cmd);
	msg.AddString("postscript_cmd",postscript_cmd);
		
	BFile file(settings_path.String(),B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	msg.Flatten(&file);
	file.Unset();
}

void Prefs::ResetToDefaults()
{
	fg_color = ColourConstants::K_BLACK;
	bg_color = ColourConstants::K_WHITE;
	generic_cmd_color = ColourConstants::K_RED;
	format_cmd_color = ColourConstants::K_GREEN;
	special_cmd_color = ColourConstants::K_PURPLE;
	math_mode_color = ColourConstants::K_NAVY_BLUE;
	comma_color = ColourConstants::K_RED;
	comment_color = ColourConstants::K_BLUE;
	punc_symbol_color = ColourConstants::K_PURPLE;
	html_color = ColourConstants::K_ORANGE;
	FontSize = 12;
	
	AutoIndent = true;
	IsSyntax = true;
	
	latex_cmd = "latex $.tex";
	dvipdf_cmd = "dvipdf $.dvi";
	dvips_cmd = "dvips -o $.ps $.dvi";
	ps2pdf_cmd = "ps2pdf $.ps";
	pdflatex_cmd = "pdflatex $.tex";
	latex2html_cmd = "latex2html $.tex";
	postscript_cmd = "gs -sDEVICE=bealpha4 $.ps";
	IsCaseSensitive = false;
	IsWrapAround = false;
	IsSearchBackwards = false;
	IsAllDocs = false;	
}

