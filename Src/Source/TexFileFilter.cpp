/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef TEX_FILE_FILTER_H
#include "TexFileFilter.h"
#endif

#include <be/support/String.h>
#include <stdio.h>
#include "Constants.h"

TexFileFilter::TexFileFilter()
	: BRefFilter()
{
}

bool
TexFileFilter::Filter(const entry_ref* ref, BNode* node, struct stat_beos* st, const char* filetype)
{
	bool isOk = false;
	char type[256];
	BNodeInfo nodeInfo(node);

	if (node->IsDirectory()) {
		isOk = true;
	} else {
		nodeInfo.GetType(type);
		BString refStr(ref->name);
		// allow all files with type TEX_FILETYPE
		isOk = (refStr.FindFirst(".tex") > 0
				|| (strcmp(TEX_FILETYPE, type) == 0 || strcmp("text/plain", type) == 0));
	}

	return isOk;
}
