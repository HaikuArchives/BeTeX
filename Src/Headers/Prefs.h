#ifndef _PREFS_H_
#define _PREFS_H_

#include <iostream>
#include <View.h>
#include <String.h>
#include <TypeConstants.h>
#include "constants.h"
#include <File.h>
#include <Message.h>


class Prefs
{
	public:
		Prefs();
		~Prefs();
		void Load();
		void Save();
		void ResetToDefaults();
		BString settings_path;
	//private:	//Set variables to public for ease of use
		//Syntax Highlighting
		//Have in prefs panel some sample text - updates live!
		// e.g. %This is a comment
		//	 	\[ Mathematics is COOL \] $Phe\alphar the Calculus$
		//Have Colour well for each and selector like Colors! one.
		rgb_color fg_color;				//black
		rgb_color bg_color;				//white	
		rgb_color generic_cmd_color;	//red
		rgb_color format_cmd_color;		//green
		rgb_color special_cmd_color;	//purple
		rgb_color math_mode_color;		//navyblue
		rgb_color comma_color;			//red
		rgb_color comment_color;		//blue
		rgb_color punc_symbol_color; 	//purple
		rgb_color html_color;			//sea green
		
		//Fonts
		//Have BSlider for live update in size
		int32 FontSize;					//12
		//Font Type is Fixed
		
		//Boolean Values
		bool AutoIndent;				//true
		bool IsSyntax;		//true
		
		//Latex Commands ($ represents filename "basename")
		BString latex_cmd;			//latex $.tex
		BString dvipdf_cmd;			//dvipdf $.dvi
		BString dvips_cmd;			//dvips -o $.ps $.dvi
		BString ps2pdf_cmd;			//ps2pdf $.ps
		BString pdflatex_cmd;		//pdflatex $.tex
		BString latex2html_cmd;		//latex2html $.tex
		BString postscript_cmd;		//gs -sDEVICE=bealpha4 $.ps
		
		
		bool IsCaseSensitive;
		bool IsWrapAround;
		bool IsSearchBackwards;
		bool IsAllDocs;
		bool IsTexBarInverted;
	//private:	
	
		bool IsGreekHidden;
		bool IsBigHidden;
		bool IsBinaryHidden;
		bool IsMiscHidden;
		bool IsBinRelHidden;
		bool IsMMAHidden;
		bool IsIntlHidden;
		bool IsTypeFaceHidden;
		bool IsFunctionHidden;
	
		bool IsGreekAbsent;
		bool IsBigAbsent;
		bool IsBinaryAbsent;
		bool IsMiscAbsent;
		bool IsBinRelAbsent;
		bool IsMMAAbsent;
		bool IsIntlAbsent;
		bool IsTypeFaceAbsent;
		bool IsFunctionAbsent;	
};
#endif //_PREFS_H_

