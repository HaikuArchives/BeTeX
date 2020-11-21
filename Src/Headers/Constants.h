/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <be/interface/View.h>

static bool CompareColors(const rgb_color a, const rgb_color b)
{
	return a.red == b.red
		&& a.green == b.green
		&& a.blue == b.blue
		&& a.alpha == b.alpha;
}

inline bool operator==(const rgb_color &a, const rgb_color &b)
{
	return CompareColors(a, b);
}

inline bool operator!=(const rgb_color &a, const rgb_color &b)
{
	return !CompareColors(a, b);
}

#define TEX_FILETYPE					"text/x-tex"
#define APP_SIG							"application/x-vnd.mik-BeTeX"
#define GREEK_SYMBOL_FONT_FAMILY		"rtxmi"
#define GREEK_SYMBOL_FONT_STYLE			"Medium"

//lower case letters
namespace GreekSymbolConstants
{
	const uint32 
			K_GREEK_SYMBOL_ALPHA =			11,
			K_GREEK_SYMBOL_BETA =			12,
			K_GREEK_SYMBOL_GAMMA =			13,
			K_GREEK_SYMBOL_DELTA =			14,
			K_GREEK_SYMBOL_EPSILON =		15,
			K_GREEK_SYMBOL_ZETA =			16,
			K_GREEK_SYMBOL_ETA =			17,
			K_GREEK_SYMBOL_THETA =			18,
			K_GREEK_SYMBOL_IOTA =			19,
			K_GREEK_SYMBOL_KAPPA =			20,
			K_GREEK_SYMBOL_LAMBDA =			21,
			K_GREEK_SYMBOL_MU =				22,
			K_GREEK_SYMBOL_NU =				23,
			K_GREEK_SYMBOL_XI =				24,
			K_GREEK_SYMBOL_PI =				25,
			K_GREEK_SYMBOL_RHO =			26,
			K_GREEK_SYMBOL_SIGMA =			27,
			K_GREEK_SYMBOL_TAU =			28,
			K_GREEK_SYMBOL_UPSILON =		29,
			K_GREEK_SYMBOL_PHI =			30,
			K_GREEK_SYMBOL_CHI =			31,
			K_GREEK_SYMBOL_PSI =			32,
			K_GREEK_SYMBOL_OMEGA =			33,
			K_GREEK_SYMBOL_SPACE =			65
			;
};

namespace ColorConstants
{
	const rgb_color	
			K_BLACK =						{0,0,0,255},
			K_WHITE = 						{255,255,255,255},
			K_RED = 						{255,0,0,255},
			K_BLUE = 						{0,0,255,255},
			K_MAGENTA = 					{255,0,255,255},
			K_YELLOW = 						{254,202,0,255},
			K_GREEN = 						{0,150,113,255},
			K_PURPLE = 						{136,0,156,255},
			K_NAVY_BLUE = 					{50,0,150,255},
			K_ORANGE = 						{255,174,0,255},
			K_BUBBLE_DEFAULT = 				{255,255,200,255},
			/*BeOS Colors*/
			K_BE_BLUE_BACKGROUND =			{51,102,152,255},
			K_BE_BLUE_STATUS_BAR =			{51,152,255,255},
			K_BE_BLUE_FOCUS =				{0,0,229,255},
			K_BE_YELLOW_TAB =				{255,203,0,255},
			K_BE_GREY_PANEL =				{216,216,216,255},
			/*Normal*/
			K_ORANGE_2 =					{255,128,0,255},
			K_YELLOW_2 =					{255,255,0,255},
			K_GREEN_2  =					{0,255,0,255},
			K_BLUE_2   =					{0,0,255,255},			
			/*Custom Colors*/			
			//Spectrum
			K_SPECTRUM_RED    =				{235,0,0,255},
			K_SPECTRUM_ORANGE =				{235,122,0,255},
			K_SPECTRUM_YELLOW =				{244,241,0,255},
			K_SPECTRUM_GREEN  =				{50,212,0,255},
			K_SPECTRUM_BLUE   =				{0,126,255,255},
			K_SPECTRUM_INDIGO =				{25,0,60,255},
			K_SPECTRUM_PURPLE =				{105,0,84,255},			
			//Metal
			K_GOLD   =						{234,209,0,255},
			K_SILVER =						{226,226,226,255},
			K_COPPER =						{213,100,0,255},
			K_ORICHALCHUM =					{255,174,0},
			
			//UI color constants			
			K_UNSAVED_TEXT_COLOR =			{0,0,255}
			;
};

namespace MenuConstants
{
	//File Menu
	const uint32 
			K_MENU_FILE_NEW =				'mfnw',
			K_MENU_FILE_OPEN =				'mfop',
			K_MENU_FILE_OPEN_RECENT =		'mfor',
			K_MENU_FILE_OPEN_TEMPLATE =		'mfot',
			K_MENU_FILE_OPEN_FOLDER =		'mfof',
			K_UPDATE_RECENT_DOCS =			'udrd',
			K_MENU_FILE_CLOSE =				'mfcl',
			K_MENU_FILE_SAVE =				'mfsv',
			K_MENU_FILE_SAVEAS =			'mfsa',
			K_MENU_FILE_NEXT_ITEM =			'mfni',
			K_MENU_FILE_PREV_ITEM =			'mfpi',
			K_MENU_FILE_PAGE_SETUP =		'mfps',
			K_MENU_FILE_PRINT =				'mfpr',
			K_MENU_FILE_PREFS =				'mfpf',
			K_MENU_FILE_ABOUT =				'mfab',
			K_MENU_FILE_QUIT =				'mfqt'
			;

	//Edit Menu
	const uint32 
			K_MENU_EDIT_UNDO =				'meun',
			K_MENU_EDIT_REDO =				'mere',
			K_MENU_EDIT_SEARCH =			'mesh',
			K_MENU_EDIT_GOTOLINE =			'megl'
			;

	//Format Menu
	const uint32 
			K_MENU_FORMAT_BOLD =			'ftbd',
			K_MENU_FORMAT_EMPH =			'fteh',
			K_MENU_FORMAT_ITAL =			'ftil',
			K_MENU_FORMAT_SHIFT_LEFT =		'ftsl',
			K_MENU_FORMAT_SHIFT_RIGHT =		'ftsr',
			K_MENU_FORMAT_COMMENTLINE =		'ftcl',
			K_MENU_FORMAT_UNCOMMENTLINE =	'ftul',
			K_MENU_FORMAT_RESET_LAYOUT =	'rlmf'
			;
			
	//Insert Menu
	const uint32
			K_MENU_INSERT_FILE =			'mife',
			K_MENU_INSERT_FILE_RECEIVED =	'frim',
			K_MENU_INSERT_DATE =			'mide',
			K_MENU_INSERT_ARRAY =			'miay',
			K_MENU_INSERT_ARRAY_WITHDIM =	'aywd',
			K_MENU_INSERT_MATRIX =			'mimx',
			K_MENU_INSERT_MATRIX_WITHDIM =	'mxwd',
			K_MENU_INSERT_TABULAR =			'mitr',
			K_MENU_INSERT_TABULAR_WITHDIM =	'trwd',
			K_MENU_INSERT_EQUATION =		'mien', 
			K_MENU_INSERT_COLORED_TEXT =	'mict',
			K_MENU_INSERT_ITEMIZE =			'miit',
			K_MENU_INSERT_DESCRIPTION =		'midn',
			K_MENU_INSERT_ENUMERATE =		'miee',
			K_MENU_INSERT_CENTER =			'micr',
			K_MENU_INSERT_FLUSHLEFT =		'mifl',
			K_MENU_INSERT_FLUSHRIGHT =		'mifr',
			K_MENU_INSERT_FIGURE =			'mifg',
			K_MENU_INSERT_TABLE =			'mitb',
			K_MENU_INSERT_EQNARRAY =		'miea',
			K_MENU_INSERT_EQNARRAY_STAR =	'mies',
			K_MENU_INSERT_VERBATIM =		'mivb',
			K_MENU_INSERT_QUOTE =			'miqt'
			;

};

//Toolbar Items
namespace ToolbarConstants
{
	const uint32 
			K_CMD_DELETE_TEMP_FILES =		'dtfs',
			K_CMD_VIEW_LOG_FILE =			'vlgf',
			K_CMD_COMPILE_TEXDVI =			'txdv',
			K_CMD_COMPILE_DVIPDF =			'dvpf',
			K_CMD_COMPILE_DVIPS =			'dvps',
			K_CMD_COMPILE_PSPDF =			'pspf',
			K_CMD_COMPILE_TEXPDF =			'txpf',
			K_CMD_COMPILE_TEXHTML =			'txhl',
			K_CMD_LAUNCH_TERMINAL =			'ltrm',
			K_CMD_LAUNCH_TRACKER =			'ltrk',
			K_CMD_LAUNCH_DVIVIEWER =		'ldvv',
			K_CMD_LAUNCH_PSVIEWER =			'lpsv',
			K_CMD_LAUNCH_BE_PDF =			'lbpf',
			K_CMD_LAUNCH_HTMLVIEWER =		'lhlv',
			K_UPDATE_TEXTVIEW_SHORTCUTS =	'udts'
			;
};

namespace AboutMessages
{
	const uint32 K_ABOUT_WINDOW_QUIT =				'abwq',
			K_ABOUT_WINDOW_LAUNCH = 				'abwg',
			K_GOTO_BETEX_HOMEPAGE = 				'gtbh',
			K_GOTO_BETEX_DONATEPAGE = 				'gtbd',
			K_GOTO_BETEX_BEBITSPAGE = 				'gbbb'
			;
};

namespace SearchWindowConstants
{
	const uint32 
			K_SEARCH_WINDOW_FIND =					'swfd',
			K_SEARCH_WINDOW_REPLACE =				'swrp',
			K_SEARCH_WINDOW_REPLACE_ALL =			'swra',
			K_SEARCH_WINDOW_SETTINGS_CHANGED =		'swsc',
			K_SEARCH_WINDOW_QUIT =					'swqt'
			;
};

namespace ColorPrefsConstants
{
	const uint32 
			K_COLOR_WINDOW_QUIT =					'clrq',
			K_COLOR_PREFS_UPDATE =					'cput',
			K_COLOR_PREFS_OK =						'cpok',
			K_COLOR_PREFS_CANCEL =					'cpcl'
			;
};

namespace PrefsConstants
{
	const uint32 
			K_PREFS_UPDATE =						'PVud',
			K_PREFS_VIEW_OK =						'pvok',
			K_PREFS_VIEW_CANCEL =					'pvcl',
			K_PREFS_VIEW_RESET_COLOR_DEFAULTS = 	'pcrd',
			K_PREFS_VIEW_RESET_COMMAND_DEFAULTS =	'pcmd',
			K_PREFS_VIEW_RESET_TOOLBAR_DEFAULTS =	'pvrt',			
			K_PREFS_VIEW_RESET_GENERAL_DEFAULTS =	'pcgd',
			K_PREFS_WINDOW_QUIT =				'pfwq'
			;
			
	const uint32
			K_RESET_TEXVIEW_PREFS =					1,
			K_RESET_COMMAND_PREFS =					2, 
			K_RESET_TOOLBAR_PREFS =					4,
			K_RESET_GENERAL_PREFS =					8
			;
};

namespace InterfaceConstants
{
	const uint32 
			K_CMD_TBUTTON_INSERT =					'ctbi',
			K_HANDLE_HIERARCHY =					'hier',
			K_ANALYSE_TEXVIEW =						'atxv',
			K_PARSE_LINE =							'psln',
			K_UPDATE_CLIPBOARD_MENU_STATUS =		'ucms',
			K_UPDATE_STATUSBAR =					'udsb',
			K_UNDO_HIGHLIGHTING =					'udhl',
			K_FILE_CONTENTS_CHANGED =				'fcch',
			K_GTL_WINDOW_GO =						'glwg',
			K_GTL_WINDOW_QUIT =						'glwq',
			K_RESET_ALL_TEX_VIEW_COLORS =			'ratc',
			K_RESET_ALL_TEX_VIEW_FONTS =			'ratf',
			K_SPAN_WINDOW_LAUNCH =					'spwl',
			K_SPAN_WINDOW_QUIT =					'spwq',
			K_DIM_WINDOW_OK =						'dwok',
			K_DIM_WINDOW_CANCEL =					'dwcl',
			K_DIM_WINDOW_QUIT =						'dwqt'
			;
};
 
#endif
