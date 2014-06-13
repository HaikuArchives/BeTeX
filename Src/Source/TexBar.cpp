#include "TexBar.h"

TexBar::TexBar(BRect r,BubbleHelper* h,Prefs* pr) 
			:	BView(r,"TexBar",B_FOLLOW_ALL_SIDES,B_WILL_DRAW|B_FRAME_EVENTS|B_NAVIGABLE)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));//tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_2_TINT));
	prefs = pr;
	helper = h;
	//helper->= new BubbleHelper();
	r = Bounds();
	IsGreekHidden = prefs->IsGreekHidden;
	IsBigHidden = prefs->IsBigHidden;
	IsBinaryHidden = prefs->IsBinaryHidden;
	IsMiscHidden = prefs->IsMiscHidden;
	IsBinRelHidden = prefs->IsBinRelHidden;
	IsMMAHidden = prefs->IsMMAHidden;
	IsIntlHidden = prefs->IsIntlHidden;
	IsTypeFaceHidden = prefs->IsTypeFaceHidden;
	IsFunctionHidden = prefs->IsFunctionHidden;

	IsGreekAbsent = prefs->IsGreekAbsent;
	IsBigAbsent = prefs->IsBigAbsent;
	IsBinaryAbsent = prefs->IsBinaryAbsent;
	IsMiscAbsent = prefs->IsMiscAbsent;
	IsBinRelAbsent = prefs->IsBinRelAbsent;
	IsMMAAbsent = prefs->IsMMAAbsent;
	IsIntlAbsent = prefs->IsIntlAbsent;
	IsTypeFaceAbsent = prefs->IsTypeFaceAbsent;
	IsFunctionAbsent = prefs->IsFunctionAbsent;

	GreekHeader = NULL;
	BigHeader = NULL;
	BinaryHeader = NULL;
	MiscHeader = NULL;
	BinRelHeader = NULL;
	MMAHeader = NULL;
	IntlHeader = NULL;
	TypeFaceHeader = NULL;
	FunctionHeader = NULL;         
	
	unsigned char* p;
	
	if(!IsGreekAbsent)
	{
		GreekHeader = new HeaderItem(BRect(0,0,r.Width(),20),"Greek Letters",B_FOLLOW_NONE,B_WILL_DRAW|B_FRAME_EVENTS,prefs);
		AddChild(GreekHeader);
		
		
		for(int i=0;i<greek_items;i++)
		{
			greekbitvec.push_back(new BBitmap(BRect(0,0,greek_data[i].w,greek_data[i].h),B_RGB32));
			p = (unsigned char*)greekbitvec[i]->Bits();
			for(int k=0;k<greek_data[i].n;k++)
			{
				p[k] = greek_data[i].p[k];
			}
		}
	}
	//IsGreekHidden = false;
	if(!IsBigAbsent)
	{
		BigHeader = new HeaderItem(BRect(0,0,r.Width(),20),"Big Operators",B_FOLLOW_NONE,B_WILL_DRAW|B_FRAME_EVENTS,prefs);
		AddChild(BigHeader);
		
		for(int i=0;i<big_items;i++)
		{
			bigbitvec.push_back(new BBitmap(BRect(0,0,big_data[i].w,big_data[i].h),B_RGB32));
			p = (unsigned char*)bigbitvec[i]->Bits();
			for(int k=0;k<big_data[i].n;k++)
			{
				p[k] = big_data[i].p[k];
			}
		}
		//IsBigHidden = false;
	}
	
	if(!IsBinaryAbsent)
	{
		BinaryHeader = new HeaderItem(BRect(0,0,r.Width(),20),"Binary Operators",B_FOLLOW_NONE,B_WILL_DRAW|B_FRAME_EVENTS,prefs);
		AddChild(BinaryHeader);
		
		for(int i=0;i<binary_items;i++)
		{
			binarybitvec.push_back(new BBitmap(BRect(0,0,binary_data[i].w,binary_data[i].h),B_RGB32));
			p = (unsigned char*)binarybitvec[i]->Bits();
			for(int k=0;k<binary_data[i].n;k++)
			{
				p[k] = binary_data[i].p[k];
			}
		}
		//IsBinaryHidden = false;
	}
	
	if(!IsMiscAbsent)
	{
		MiscHeader = new HeaderItem(BRect(0,0,r.Width(),20),"Misc Symbols",B_FOLLOW_NONE,B_WILL_DRAW|B_FRAME_EVENTS,prefs);
		AddChild(MiscHeader);
		
		for(int i=0;i<misc_items;i++)
		{
			miscbitvec.push_back(new BBitmap(BRect(0,0,misc_data[i].w,misc_data[i].h),B_RGB32));
			p = (unsigned char*)miscbitvec[i]->Bits();
			for(int k=0;k<misc_data[i].n;k++)
			{
				p[k] = misc_data[i].p[k];
			}
		}
		//IsMiscHidden = false;
	}
	
	if(!IsBinRelAbsent)
	{
		BinRelHeader = new HeaderItem(BRect(0,0,r.Width(),20),"Binary Relations",B_FOLLOW_NONE,B_WILL_DRAW|B_FRAME_EVENTS,prefs);
		AddChild(BinRelHeader);
		
		for(int i=0;i<bin_rel_items;i++)
		{
			bin_relbitvec.push_back(new BBitmap(BRect(0,0,bin_rel_data[i].w,bin_rel_data[i].h),B_RGB32));
			p = (unsigned char*)bin_relbitvec[i]->Bits();
			for(int k=0;k<bin_rel_data[i].n;k++)
			{
				p[k] = bin_rel_data[i].p[k];
			}
		}
		//IsBinRelHidden = false;
	}
	
	if(!IsMMAAbsent)
	{
		MMAHeader = new HeaderItem(BRect(0,0,r.Width(),20),"Math Mode Accents",B_FOLLOW_NONE,B_WILL_DRAW|B_FRAME_EVENTS,prefs);
		AddChild(MMAHeader);
		
		for(int i=0;i<mmacc_items;i++)
		{
			mmaccbitvec.push_back(new BBitmap(BRect(0,0,mmacc_data[i].w,mmacc_data[i].h),B_RGB32));
			p = (unsigned char*)mmaccbitvec[i]->Bits();
			for(int k=0;k<mmacc_data[i].n;k++)
			{
				p[k] = mmacc_data[i].p[k];
			}
		}
		//IsMMAHidden = false;
	}
	
	if(!IsIntlAbsent)
	{
		
		IntlHeader = new HeaderItem(BRect(0,0,r.Width(),20),"International",B_FOLLOW_NONE,B_WILL_DRAW|B_FRAME_EVENTS,prefs);
		AddChild(IntlHeader);
		
		for(int i=0;i<intl_items;i++)
		{
			intlbitvec.push_back(new BBitmap(BRect(0,0,intl_data[i].w,intl_data[i].h),B_RGB32));
			p = (unsigned char*)intlbitvec[i]->Bits();
			for(int k=0;k<intl_data[i].n;k++)
			{
				p[k] = intl_data[i].p[k];
			}
		}
		//IsIntlHidden = false;
	}
	if(!IsTypeFaceAbsent)
	{
			
		TypeFaceHeader = new HeaderItem(BRect(0,0,r.Width(),20),"Type Faces",B_FOLLOW_NONE,B_WILL_DRAW|B_FRAME_EVENTS,prefs);
		AddChild(TypeFaceHeader);
		//IsTypeFaceHidden = false;
	}
	if(!IsFunctionAbsent)
	{
		FunctionHeader = new HeaderItem(BRect(0,0,r.Width(),20),"Functions",B_FOLLOW_NONE,B_WILL_DRAW|B_FRAME_EVENTS,prefs);
		AddChild(FunctionHeader);
		//IsFunctionHidden = false;
	}
	
	if(!IsGreekAbsent) GreekHeader->SetHidden(IsGreekHidden);
	if(!IsBigAbsent) BigHeader->SetHidden(IsBigHidden);
	if(!IsBinaryAbsent) BinaryHeader->SetHidden(IsBinaryHidden);
	if(!IsMiscAbsent) MiscHeader->SetHidden(IsMiscHidden);
	if(!IsBinRelAbsent) BinRelHeader->SetHidden(IsBinRelHidden);
	if(!IsMMAAbsent) MMAHeader->SetHidden(IsMMAHidden);
	if(!IsIntlAbsent) IntlHeader->SetHidden(IsIntlHidden);
	if(!IsTypeFaceAbsent) TypeFaceHeader->SetHidden(IsTypeFaceHidden);
	if(!IsFunctionAbsent) FunctionHeader->SetHidden(IsFunctionHidden);		

	IsResizing = false;
	Tile(this,false);
}
void TexBar::Tile(BView* v,bool IsOnlyResizing)
{
	
	if(IsResizing)
		return;
		
	BMessage* msg;	
	IsResizing = true;	
	
	int child_offset = 0;
	const int TileSize=23; 
	BRect tiler(0,0,TileSize,TileSize);
	BScrollBar* bar = ScrollBar(B_VERTICAL);
	float ScrollBarOffset=0;
	if(bar)
	{
		ScrollBarOffset = -bar->Value();
		tiler.top = ScrollBarOffset;
	}
	//ConvertToP
	BRect f = v->Bounds();
	int j=1;
	int i;
	int num_across=0;
	float width = f.Width();
	float twidth = tiler.Width()+1;
	
	if(!IsGreekAbsent)
		child_offset++;
	if(!IsBigAbsent)
		child_offset++;
	if(!IsBinaryAbsent)
		child_offset++;
	if(!IsMiscAbsent)
		child_offset++;
	if(!IsBinRelAbsent)
		child_offset++;
	if(!IsMMAAbsent)
		child_offset++;
	if(!IsIntlAbsent)
		child_offset++;
	if(!IsTypeFaceAbsent)
		child_offset++;
	if(!IsFunctionAbsent)
		child_offset++;

	num_across=0;
	width = f.Width();
	twidth = tiler.Width()+1;
	while(width >=0)
	{
		width -= twidth;
		num_across++;
	}
	if(width != -1)
		num_across--;	
	
	bool NeedInitCorr = true;
		
	if(!IsGreekAbsent)
	{
		GreekHeader->ResizeTo(f.Width(),20);
		GreekHeader->Invalidate();
		//child_offset = 9;	//we have this many header items
						
			j=1;
			tiler.OffsetBy(0,GreekHeader->Bounds().Height()+1);
			for(i=0;i<greek_items;i++)
			{
					if(!IsOnlyResizing)
					{	
						msg = new BMessage(InterfaceConstants::K_CMD_TBUTTON_INSERT);
						if(msg->AddString("cmd",greek_data[i].cmd) == B_OK)
						{
							v->AddChild(temp_button = new TButton(tiler,msg,greekbitvec[i]));
							helper->SetHelp(temp_button,greek_data[i].cmd);
						}
						
						
						tiler.OffsetBy(TileSize+1,0);
						if(j == num_across)
						{
							tiler.left=0;
							tiler.right=TileSize;
							tiler.OffsetBy(0,TileSize+1);
							j = 0;
						}
						j++;
					}
					else
					{
						int child_index = i+child_offset;
						BView* child = v->ChildAt(child_index);
						if(child != NULL)
						{
							if(IsGreekHidden)
							{
								if(!child->IsHidden())
								child->Hide();
							}
							else
							{
								if(child->IsHidden())
									child->Show();
						
								child->MoveTo(tiler.LeftTop());
								
								tiler.OffsetBy(TileSize+1,0);
								
								if(j == num_across)
								{
									tiler.left=0;
									tiler.right=TileSize;
									tiler.OffsetBy(0,TileSize+1);
									j = 0;
								}
								j++;
							
							
							}
						}
					}
					
					
		}
			tiler.left=0;
			tiler.right=TileSize;
			if(j <= num_across && j != 1)		
				tiler.OffsetBy(0,TileSize+1);
		
		child_offset += greek_items-1;
		NeedInitCorr = false;	
	}	
	if(!IsBigAbsent)
	{
		
		//if(!IsGreekAbsent)
		//{
			BigHeader->MoveTo(tiler.LeftTop());
		//}
		
		BigHeader->ResizeTo(f.Width(),20);
		BigHeader->Invalidate();
		
		tiler.OffsetBy(0,BigHeader->Bounds().Height()+1);
			j=1;
			for(i=0;i<big_items;i++)
			{
					if(!IsOnlyResizing)
					{
						msg = new BMessage(InterfaceConstants::K_CMD_TBUTTON_INSERT);
						if(msg->AddString("cmd",big_data[i].cmd) == B_OK)
						{
							v->AddChild(temp_button = new TButton(tiler,msg,bigbitvec[i]));
							helper->SetHelp(temp_button,big_data[i].cmd);
						}
	
						tiler.OffsetBy(TileSize+1,0);
						if(j == num_across)
						{
							tiler.left=0;
							tiler.right=TileSize;
							tiler.OffsetBy(0,TileSize+1);
							j = 0;
						}
						j++;
					}
					else
					{
						int child_index = i+child_offset;
						if(!IsGreekAbsent)
							child_index++;
						BView* child = v->ChildAt(child_index);
						if(child != NULL)
						{
							if(IsBigHidden)
							{
								if(!child->IsHidden())
								child->Hide();
							}
							else
							{
								if(child->IsHidden())
									child->Show();
						
								child->MoveTo(tiler.LeftTop());
								
								tiler.OffsetBy(TileSize+1,0);
								
								if(j == num_across)
								{
									tiler.left=0;
									tiler.right=TileSize;
									tiler.OffsetBy(0,TileSize+1);
									j = 0;
								}
								j++;
							
							
							}
						}
					}
					
					
			}
	
		tiler.left=0;
		tiler.right=TileSize;
		if(j <= num_across && j != 1)		
			tiler.OffsetBy(0,TileSize+1);
		child_offset += big_items;	
		if(NeedInitCorr)
		{
			child_offset--;
			NeedInitCorr = false;
		}
	}
	if(!IsBinaryAbsent)
	{	
		/*if(!IsGreekAbsent || !IsBigAbsent)
		{
			tiler.left=0;
			tiler.right=TileSize;
			if(j <= num_across && j != 1)		
				tiler.OffsetBy(0,TileSize+1);
				
			*/
			BinaryHeader->MoveTo(tiler.LeftTop());
		//}
		BinaryHeader->ResizeTo(f.Width(),20);
		BinaryHeader->Invalidate();
		
		tiler.OffsetBy(0,BinaryHeader->Bounds().Height()+1);
			j=1;
			for(i=0;i<binary_items;i++)
			{
					if(!IsOnlyResizing)
					{
						msg = new BMessage(InterfaceConstants::K_CMD_TBUTTON_INSERT);
						if(msg->AddString("cmd",binary_data[i].cmd) == B_OK)
						{
							if(binary_data[i].s == "lhd" || binary_data[i].s == "rhd")
							{
								vector<BBitmap*> Icons;
								vector<BPoint> Origins;
	
								Icons.push_back(binarybitvec[i]);
								Origins.push_back(BPoint(0,-2));
								v->AddChild(temp_button = new  TButton(tiler,msg,Icons,Origins));
							}
							else
								v->AddChild(temp_button = new TButton(tiler,msg,binarybitvec[i]));
							
							helper->SetHelp(temp_button,binary_data[i].cmd);
						}
	
						tiler.OffsetBy(TileSize+1,0);
						if(j == num_across)
						{
							tiler.left=0;
							tiler.right=TileSize;
							tiler.OffsetBy(0,TileSize+1);
							j = 0;
						}
						j++;
					}
					else
					{
						int child_index = i+child_offset;
						if(!IsGreekAbsent || !IsBigAbsent)
							child_index++;
						BView* child = v->ChildAt(child_index);
						if(child != NULL)
						{
							if(IsBinaryHidden)
							{
								if(!child->IsHidden())
								child->Hide();
							}
							else
							{
								if(child->IsHidden())
									child->Show();
						
								child->MoveTo(tiler.LeftTop());
								
								tiler.OffsetBy(TileSize+1,0);
								
								if(j == num_across)
								{
									tiler.left=0;
									tiler.right=TileSize;
									tiler.OffsetBy(0,TileSize+1);
									j = 0;
								}
								j++;
							
							
							}
						}
					}
					
					
			}
			tiler.left=0;
			tiler.right=TileSize;
			if(j <= num_across && j != 1)		
				tiler.OffsetBy(0,TileSize+1);
		child_offset += binary_items;	
		if(NeedInitCorr)
		{
			child_offset--;
			NeedInitCorr = false;
		}
	}		
	if(!IsMiscAbsent)
	{
		//if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent)
		//{
		
			MiscHeader->MoveTo(tiler.LeftTop());
		//}
		MiscHeader->ResizeTo(f.Width(),20);
		MiscHeader->Invalidate();
		
		tiler.OffsetBy(0,MiscHeader->Bounds().Height()+1);
			j=1;
			for(i=0;i<misc_items;i++)
			{
					if(!IsOnlyResizing)
					{
						msg = new BMessage(InterfaceConstants::K_CMD_TBUTTON_INSERT);
						if(msg->AddString("cmd",misc_data[i].cmd) == B_OK)
						{
							if(i!=0)
							v->AddChild(temp_button = new TButton(tiler,msg,miscbitvec[i]));
							else
							{
								vector<BBitmap*> Icons;
								vector<BPoint> Origins;
								Icons.push_back(miscbitvec[i]);
								Origins.push_back(BPoint(0,5));
								v->AddChild(temp_button = new TButton(tiler,msg,Icons,Origins));
							
							}
							helper->SetHelp(temp_button,misc_data[i].cmd);
						}
	
						tiler.OffsetBy(TileSize+1,0);
						if(j == num_across)
						{
							tiler.left=0;
							tiler.right=TileSize;
							tiler.OffsetBy(0,TileSize+1);
							j = 0;
						}
						j++;
					}
					else
					{
						int child_index = i+child_offset;
						if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent)
							child_index++;
						BView* child = v->ChildAt(child_index);
						if(child != NULL)
						{
							if(IsMiscHidden)
							{
								if(!child->IsHidden())
								child->Hide();
							}
							else
							{
								if(child->IsHidden())
									child->Show();
						
								child->MoveTo(tiler.LeftTop());
								
								tiler.OffsetBy(TileSize+1,0);
								
								if(j == num_across)
								{
									tiler.left=0;
									tiler.right=TileSize;
									tiler.OffsetBy(0,TileSize+1);
									j = 0;
								}
								j++;
							
							
							}
						}
					}
					
					
			}
		//Binary Relations
			tiler.left=0;
			tiler.right=TileSize;
			if(j <= num_across && j != 1)		
				tiler.OffsetBy(0,TileSize+1);
		child_offset += misc_items;	
		if(NeedInitCorr)
		{
			child_offset--;
			NeedInitCorr = false;
		}
	}	
	if(!IsBinRelAbsent)
	{	
		//if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent || !IsMiscAbsent)
		//{
		
			BinRelHeader->MoveTo(tiler.LeftTop());
		//}
		BinRelHeader->ResizeTo(f.Width(),20);
		BinRelHeader->Invalidate();
		
		tiler.OffsetBy(0,BinRelHeader->Bounds().Height()+1);
			j=1;
			//we have two "blanks" at the end
			
			for(i=0;i<bin_rel_items-2;i++)
			{
					if(!IsOnlyResizing)
					{
						msg = new BMessage(InterfaceConstants::K_CMD_TBUTTON_INSERT);
						if(msg->AddString("cmd",bin_rel_data[i].cmd) == B_OK)
						{
							vector<BBitmap*> Icons;
							vector<BPoint> Origins;
		
							if(bin_rel_data[i].s == "notin")
							{
								Icons.push_back(bin_relbitvec[39]);
								Origins.push_back(BPoint(0,0));
								
								Icons.push_back(bin_relbitvec[16]);
								Origins.push_back(BPoint(0,0));
								v->AddChild(temp_button = new  TButton(tiler,msg,Icons,Origins));
							}
							else if(bin_rel_data[i].s == "cong")
							{
								Icons.push_back(bin_relbitvec[38]);
								Origins.push_back(BPoint(0,3));
								
								Icons.push_back(bin_relbitvec[30]);
								Origins.push_back(BPoint(0,-4));
								v->AddChild(temp_button = new  TButton(tiler,msg,Icons,Origins));
							}
							else if(bin_rel_data[i].s == "models")
							{
								Icons.push_back(bin_relbitvec[38]);
								Origins.push_back(BPoint(0,0));
								
								Icons.push_back(bin_relbitvec[22]);
								Origins.push_back(BPoint(-6,0));
								v->AddChild(temp_button = new  TButton(tiler,msg,Icons,Origins));
							}
							else if(bin_rel_data[i].s == "neq")
							{
								Icons.push_back(bin_relbitvec[38]);
								Origins.push_back(BPoint(0,0));
								
								Icons.push_back(bin_relbitvec[39]);
								Origins.push_back(BPoint(0,0));
								v->AddChild(temp_button = new  TButton(tiler,msg,Icons,Origins));
							}
							else if(bin_rel_data[i].s == "doteq")
							{
								Icons.push_back(bin_relbitvec[38]);
								Origins.push_back(BPoint(0,0));
								
								Icons.push_back(binarybitvec[2]);
								Origins.push_back(BPoint(0,-6));
								v->AddChild(temp_button = new  TButton(tiler,msg,Icons,Origins));
							}
							else if(bin_rel_data[i].s == "bowtie")
							{
								Icons.push_back(binarybitvec[24]);
								Origins.push_back(BPoint(3,0));
								
								Icons.push_back(binarybitvec[25]);
								Origins.push_back(BPoint(-3,0));
								v->AddChild(temp_button = new  TButton(tiler,msg,Icons,Origins));
							}
							else if(bin_rel_data[i].s == "prec" || bin_rel_data[i].s == "succ")
							{
								Icons.push_back(bin_relbitvec[i]);
								Origins.push_back(BPoint(0,-1));
								v->AddChild(temp_button = new  TButton(tiler,msg,Icons,Origins));
							}
							else if(bin_rel_data[i].s == "subset" || bin_rel_data[i].s == "supset" ||
							bin_rel_data[i].s == "sqsubset" || bin_rel_data[i].s == "sqsupset")
							{
								Icons.push_back(bin_relbitvec[i]);
								Origins.push_back(BPoint(0,-2));
								v->AddChild(temp_button = new  TButton(tiler,msg,Icons,Origins));
							}
							
							else v->AddChild(temp_button = new  TButton(tiler,msg,bin_relbitvec[i]));
							
							helper->SetHelp(temp_button,bin_rel_data[i].cmd);
						}
							
					
				
						tiler.OffsetBy(TileSize+1,0);
						if(j == num_across)
						{
							tiler.left=0;
							tiler.right=TileSize;
							tiler.OffsetBy(0,TileSize+1);
							j = 0;
						}
						j++;
					}
					else
					{
						int child_index = i+child_offset;
						if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent || !IsMiscAbsent)
							child_index++;
						BView* child = v->ChildAt(child_index);
						if(child != NULL)
						{
							if(IsBinRelHidden)
							{
								if(!child->IsHidden())
								child->Hide();
							}
							else
							{
								if(child->IsHidden())
									child->Show();
						
								child->MoveTo(tiler.LeftTop());
								
								tiler.OffsetBy(TileSize+1,0);
								
								if(j == num_across)
								{
									tiler.left=0;
									tiler.right=TileSize;
									tiler.OffsetBy(0,TileSize+1);
									j = 0;
								}
								j++;
							
							
							}
						}
					}
					
					
			}
			//Math Mode Accents
			tiler.left=0;
			tiler.right=TileSize;
			if(j <= num_across && j != 1)		
				tiler.OffsetBy(0,TileSize+1);
				
		child_offset += bin_rel_items-2;	
		if(NeedInitCorr)
		{
			child_offset--;
			NeedInitCorr = false;
		}
	}	
	if(!IsMMAAbsent)
	{
			
		//if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent || !IsMiscAbsent || !IsBinRelAbsent)
		//{
			MMAHeader->MoveTo(tiler.LeftTop());
		//}
		MMAHeader->ResizeTo(f.Width(),20);
		MMAHeader->Invalidate();
		
		tiler.OffsetBy(0,MMAHeader->Bounds().Height()+1);
			j=1;
			//we have two "blanks" at the end
			for(i=0;i<mmacc_items-2;i++)
			{
					if(!IsOnlyResizing)
					{
						vector<BBitmap*> Icons;
						vector<BPoint> Origins;
						msg = new BMessage(InterfaceConstants::K_CMD_TBUTTON_INSERT);
						if(msg->AddString("cmd",mmacc_data[i].cmd) == B_OK)
						{
							BPoint offset(0,-5);
							BPoint aoffset(0,3);
							if(mmacc_data[i].s == "widehat" || mmacc_data[i].s == "widetilde")
							{		
								Icons.push_back(mmaccbitvec[13]);
								offset.x = 3;
								offset.y = -10;
								aoffset.y = 0;
							}
							else
								Icons.push_back(mmaccbitvec[12]);	//the "a"
								
							Origins.push_back(aoffset);
							
							Icons.push_back(mmaccbitvec[i]);
							Origins.push_back(offset);
							v->AddChild(temp_button = new TButton(tiler,msg,Icons,Origins));
									
							helper->SetHelp(temp_button,mmacc_data[i].cmd);
						}
							
					
				
						tiler.OffsetBy(TileSize+1,0);
						if(j == num_across)
						{
							tiler.left=0;
							tiler.right=TileSize;
							tiler.OffsetBy(0,TileSize+1);
							j = 0;
						}
						j++;
					}
					else
					{
						int child_index = i+child_offset;
						if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent || !IsMiscAbsent || !IsBinRelAbsent)
							child_index++;
						BView* child = v->ChildAt(child_index);
						if(child != NULL)
						{
							if(IsMMAHidden)
							{
								if(!child->IsHidden())
								child->Hide();
							}
							else
							{
								if(child->IsHidden())
									child->Show();
						
								child->MoveTo(tiler.LeftTop());
								
								tiler.OffsetBy(TileSize+1,0);
								
								if(j == num_across)
								{
									tiler.left=0;
									tiler.right=TileSize;
									tiler.OffsetBy(0,TileSize+1);
									j = 0;
								}
								j++;
							
							
							}
						}
					}
					
					
			}
		//Intl accents and things
		tiler.left=0;
		tiler.right=TileSize;
		if(j <= num_across && j != 1)		
			tiler.OffsetBy(0,TileSize+1);
		child_offset += mmacc_items-2;	
		if(NeedInitCorr)
		{
			child_offset--;
			NeedInitCorr = false;
		}
		
	}
	if(!IsIntlAbsent)
	{	
	//	if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent || !IsMiscAbsent || !IsBinRelAbsent|| !IsMMAAbsent)	
	//	{		
					
			IntlHeader->MoveTo(tiler.LeftTop());
	//	}
		
		IntlHeader->ResizeTo(f.Width(),20);
		IntlHeader->Invalidate();
		
		tiler.OffsetBy(0,IntlHeader->Bounds().Height()+1);
			j=1;
			//we have 4 "blanks" at the end
			for(i=0;i<intl_items-13;i++)
			{
					if(!IsOnlyResizing)
					{
						vector<BBitmap*> Icons;
						vector<BPoint> Origins;
						msg = new BMessage(InterfaceConstants::K_CMD_TBUTTON_INSERT);
						if(msg->AddString("cmd",intl_data[i].cmd) == B_OK)
						{
							int letter_offset=55;
							int aidx=letter_offset;
							int cidx=aidx+1;
							int eidx=cidx+1;
							int oidx=eidx+1;
							int uidx=oidx+1;
							int yidx=uidx+1;
							int Aidx=yidx+1;
							int Cidx=Aidx+1;
							int Eidx=Cidx+1;
							int Iidx=Eidx+1;
							int Oidx=Iidx+1;
							int Uidx=Oidx+1;
							int Yidx=Uidx+1;
							
							if(intl_data[i].s == "ograve" 
							|| intl_data[i].s == "oacute"
							|| intl_data[i].s == "ohat"
							|| intl_data[i].s == "otilde"
							|| intl_data[i].s == "obar"
							|| intl_data[i].s == "intlodot"
							|| intl_data[i].s == "oddot"
							|| intl_data[i].s == "obreve"
							|| intl_data[i].s == "obreve"
							|| intl_data[i].s == "ocheck"
							|| intl_data[i].s == "oddash")
							{		
								
								
									BPoint offset(1,-7);
								if(intl_data[i].s == "intlodot")
									offset.x = 0;
									
								BPoint ooffset(0,0);
								Icons.push_back(intlbitvec[oidx]);
								Origins.push_back(ooffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
			
							}	
							else if(intl_data[i].s == "obelowdot" 
							|| intl_data[i].s == "obelowbar")
							{
								BPoint offset(1,7);
								if(intl_data[i].s == "obelowdot")
									offset.x = 0;
								
								BPoint ooffset(0,0);
								Icons.push_back(intlbitvec[oidx]);
								Origins.push_back(ooffset);
								
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "cworm")
							{
								BPoint offset(0,7);
								BPoint coffset(0,0);
								Icons.push_back(intlbitvec[cidx]);
								Origins.push_back(coffset);
								
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "oworm")
							{
								BPoint offset(0,7);
								BPoint ooffset(0,0);
								Icons.push_back(intlbitvec[oidx]);
								Origins.push_back(ooffset);
								
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "aa")
							{
								BPoint offset(0,-7);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[aidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "AA")
							{
								BPoint offset(0,-10);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[Aidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "doubleolink")
							{
								BPoint offset(0,-7);
								//BPoint ooffset(0,0);
								Icons.push_back(intlbitvec[oidx]);
								Origins.push_back(BPoint(-5,0));
								
								Icons.push_back(intlbitvec[oidx]);
								Origins.push_back(BPoint(5,0));
								
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "addot")
							{
								BPoint offset(0,-7);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[aidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "eddot")
							{
								BPoint offset(0,-7);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[eidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "iddot")
							{
								BPoint offset(0,-7);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[24]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "oddot")
							{
								BPoint offset(0,-7);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[oidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "uddot")
							{
								BPoint offset(0,-7);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[uidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "yddot")
							{
								BPoint offset(0,-9);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[yidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "Addot")
							{
								BPoint offset(0,-10);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[Aidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "Eddot")
							{
								BPoint offset(0,-10);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[Eidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "Iddot")
							{
								BPoint offset(0,-10);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[Iidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "Oddot")
							{
								BPoint offset(0,-10);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[Oidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "Uddot")
							{
								BPoint offset(0,-10);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[Uidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "Yddot")
							{
								BPoint offset(0,-10);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[Yidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "Egrave"
							|| intl_data[i].s == "Eacute"
							|| intl_data[i].s == "Ehat")
							{
								BPoint offset(1,-10);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[Eidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "Ograve"
							|| intl_data[i].s == "Oacute"
							|| intl_data[i].s == "Ohat")
							{
								BPoint offset(1,-10);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[Oidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "Cworm")
							{
								BPoint offset(0,6);
								BPoint coffset(0,-3);
								Icons.push_back(intlbitvec[Cidx]);
								Origins.push_back(coffset);
								
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							else if(intl_data[i].s == "egrave"
							|| intl_data[i].s == "eacute"
							|| intl_data[i].s == "ehat")
							{
								BPoint offset(1,-7);
								BPoint aoffset(0,0);
								Icons.push_back(intlbitvec[eidx]);
								Origins.push_back(aoffset);	
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
							}
							
							else
							{	
								BPoint offset(0,0);
								Icons.push_back(intlbitvec[i]);
								Origins.push_back(offset);
								
							}			
							v->AddChild(temp_button = new TButton(tiler,msg,Icons,Origins));
							helper->SetHelp(temp_button,intl_data[i].cmd);
						}
							
					
				
						tiler.OffsetBy(TileSize+1,0);
						if(j == num_across)
						{
							tiler.left=0;
							tiler.right=TileSize;
							tiler.OffsetBy(0,TileSize+1);
							j = 0;
						}
						j++;
					}
					else
					{
						int child_index = i+child_offset;
						if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent || !IsMiscAbsent || !IsBinRelAbsent
						|| !IsMMAAbsent)
							child_index++;
						BView* child = v->ChildAt(child_index);
						if(child != NULL)
						{
							if(IsIntlHidden)
							{
								if(!child->IsHidden())
								child->Hide();
							}
							else
							{
								if(child->IsHidden())
									child->Show();
						
								child->MoveTo(tiler.LeftTop());
								
								tiler.OffsetBy(TileSize+1,0);
								
								if(j == num_across)
								{
									tiler.left=0;
									tiler.right=TileSize;
									tiler.OffsetBy(0,TileSize+1);
									j = 0;
								}
								j++;
							
							
							}
						}
					}
					
					
			}
			tiler.left=0;
			tiler.right=TileSize;
			if(j <= num_across && j != 1)		
				tiler.OffsetBy(0,TileSize+1);
		child_offset += intl_items-13;	
		if(NeedInitCorr)
		{
			child_offset--;
			NeedInitCorr = false;
		}
		
		
	}

	int extra = 5;
	//int maxwidth=0;
	int num_text_items = 10;
	int TileWidth=75;
	int TileHeight=23;
		struct text_item
		{
			const char* text;
			BFont font;
			float width;
			const char* cmd;
		};
	BPoint tiler_lt;
	
	if(!IsTypeFaceAbsent)
	{	
		
		//if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent || !IsMiscAbsent || !IsBinRelAbsent
		//|| !IsMMAAbsent || !IsIntlAbsent)
		//{		
	
			TypeFaceHeader->MoveTo(tiler.LeftTop());
		//}
		TypeFaceHeader->ResizeTo(f.Width(),20);
		TypeFaceHeader->Invalidate();
		
		tiler.OffsetBy(0,TypeFaceHeader->Bounds().Height()+1);
		
		BFont boldfont(be_bold_font);
		BFont romanfont(be_plain_font);
		BFont emphfont(be_plain_font);
		BFont sansfont(be_plain_font);
		BFont typefont(be_plain_font);
		BFont italicfont(be_plain_font);
		italicfont.SetFace(B_ITALIC_FACE);
		italicfont.SetSize(12);//Face(B_ITALIC_FACE);
		
		emphfont.SetFace(B_ITALIC_FACE|B_BOLD_FACE);
		boldfont.SetSize(12);
		emphfont.SetSize(12);
		romanfont.SetFamilyAndStyle("Courier10 BT","Roman");
		romanfont.SetSize(14);
		sansfont.SetSize(12);
		typefont.SetFamilyAndStyle("Monospac821 BT","Roman");
		typefont.SetSize(12);
		
		BFont mediumfont(romanfont);
		mediumfont.SetSize(14);
		BFont slantfont(be_plain_font);
		slantfont.SetShear(120);
		slantfont.SetSize(12);
		
		extra = 5;
		text_item textitems[] = {
		{"Emph",emphfont,emphfont.StringWidth("Emph")+extra,"\\emph{}"},
		{"Roman",romanfont,romanfont.StringWidth("Roman")+extra,"\\textrm{}"},
		{"Bold",boldfont,boldfont.StringWidth("Bold")+extra,"\\textbf{}"},
		{"Sans",sansfont,sansfont.StringWidth("Sans")+extra,"\\textsf{}"},
		{"TypeWriter",typefont,typefont.StringWidth("TypeWriter")+extra,"\\texttt{}"},
		{"Medium",mediumfont,mediumfont.StringWidth("Medium")+extra,"\\textmd{}"},
		{"Italic",italicfont,italicfont.StringWidth("Italic")+extra,"\\textit{}"},
		{"CAPS",sansfont,sansfont.StringWidth("CAPS")+extra,"\\textsc{}"},
		{"Slanted",slantfont,slantfont.StringWidth("Slanted")+extra,"\\textsl{}"},
		{"Verbatim",typefont,typefont.StringWidth("Verbatim")+extra,"\\verb\"\""}
		};
		//maxwidth=75;
		num_text_items = 10;
		/*for(int m=0;m<num_text_items;m++)
		{
			if(textitems[m].width > maxwidth)
				maxwidth = 	(int)textitems[m].width;
		}*/
		//cout << maxwidth << endl;
		//IsResizing = true;	
		//TileWidth=maxwidth;
		//TileHeight=23;
		
		tiler_lt = tiler.LeftTop();
		
		tiler = BRect(tiler_lt.x,tiler_lt.y,tiler_lt.x+TileWidth,tiler_lt.y+TileHeight);
		f = v->Bounds();
		
		num_across=0;
		width = f.Width();
		twidth = tiler.Width()+1;
		while(width >=0)
		{
			width -= twidth;
			num_across++;
		}
		if(width != -1)
			num_across--;	
					
		j=1;
		for(i=0;i<num_text_items;i++)
		{
				
				if(!IsOnlyResizing)
				{	
					//BFont font(be_bold_font);
					//const char* text = "Bold";
					msg = new BMessage(InterfaceConstants::K_CMD_TBUTTON_INSERT);
					if(msg->AddString("cmd",textitems[i].cmd) == B_OK)
					{
						v->AddChild(temp_button = new TButton(tiler,msg,textitems[i].text,textitems[i].font));
						helper->SetHelp(temp_button,textitems[i].cmd);
					}
					tiler.OffsetBy(TileWidth+1,0);
					
					if(j == num_across)
					{
						tiler.left=0;
						tiler.right=TileWidth;
						tiler.OffsetBy(0,TileHeight+1);
						j = 0;
					}
					j++;
				}
				else
				{
						int child_index = i+child_offset;
						if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent || !IsMiscAbsent || !IsBinRelAbsent
						|| !IsMMAAbsent || !IsIntlAbsent)
							child_index++;
						BView* child = v->ChildAt(child_index);
						if(child != NULL)
						{
							if(IsTypeFaceHidden)
							{
								if(!child->IsHidden())
								child->Hide();
							}
							else
							{
								if(child->IsHidden())
									child->Show();
						
								child->MoveTo(tiler.LeftTop());
								
								tiler.OffsetBy(TileWidth+1,0);
								
								if(j == num_across)
								{
									tiler.left=0;
									tiler.right=TileWidth;
									tiler.OffsetBy(0,TileHeight+1);
									j = 0;
								}
								j++;
							
							
							}
						}
				}
				
		}		
		
		tiler.left=0;
			tiler.right=TileSize;
			if(j <= num_across && j != 1)		
				tiler.OffsetBy(0,TileSize+1);
			child_offset += num_text_items;	
		if(NeedInitCorr)
		{
			child_offset--;
			NeedInitCorr = false;
		}
		
	}		
	if(!IsFunctionAbsent)
	{
		//if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent || !IsMiscAbsent || !IsBinRelAbsent
		//|| !IsMMAAbsent || !IsIntlAbsent || !IsTypeFaceAbsent)
		//{
		
			FunctionHeader->MoveTo(tiler.LeftTop());
		//}
		FunctionHeader->ResizeTo(f.Width(),20);
		FunctionHeader->Invalidate();
		
		tiler.OffsetBy(0,FunctionHeader->Bounds().Height()+1);
		
		/*struct text_item
		{
			const char* text;
			BFont font;
			float width;
			const char* cmd;
		};*/
		
		BFont funcfont(be_plain_font);
		
		funcfont.SetSize(12);
		
		extra = 5;
		text_item functextitems[] = {
		{"arccos",funcfont,funcfont.StringWidth("arccos")+extra,"\\arccos"},
		{"arcsin",funcfont,funcfont.StringWidth("arcsin")+extra,"\\arcsin"},
		{"arctan",funcfont,funcfont.StringWidth("arctan")+extra,"\\arctan"},
		{"lim",funcfont,funcfont.StringWidth("lim")+extra,"\\lim"},
		{"lim inf",funcfont,funcfont.StringWidth("lim inf")+extra,"\\liminf"},
		{"lim sup",funcfont,funcfont.StringWidth("lim sup")+extra,"\\limsup"},
	
		{"arg",funcfont,funcfont.StringWidth("arg")+extra,"\\arg"},
		{"cos",funcfont,funcfont.StringWidth("cos")+extra,"\\cos"},
		{"cosh",funcfont,funcfont.StringWidth("cosh")+extra,"\\cosh"},
		{"cot",funcfont,funcfont.StringWidth("cot")+extra,"\\cot"},
		{"coth",funcfont,funcfont.StringWidth("coth")+extra,"\\coth"},
		{"csc",funcfont,funcfont.StringWidth("csc")+extra,"\\csc"},
	
		{"det",funcfont,funcfont.StringWidth("det")+extra,"\\det"},
		{"dim",funcfont,funcfont.StringWidth("dim")+extra,"\\dim"},
		{"exp",funcfont,funcfont.StringWidth("exp")+extra,"\\exp"},
		{"gcd",funcfont,funcfont.StringWidth("gcd")+extra,"\\gcd"},
		{"hom",funcfont,funcfont.StringWidth("hom")+extra,"\\hom"},
		{"inf",funcfont,funcfont.StringWidth("inf")+extra,"\\inf"},
		{"ker",funcfont,funcfont.StringWidth("ker")+extra,"\\ker"},
		{"lg",funcfont,funcfont.StringWidth("lg")+extra,"\\lg"},
		{"ln",funcfont,funcfont.StringWidth("ln")+extra,"\\ln"},
		{"log",funcfont,funcfont.StringWidth("log")+extra,"\\log"},
		{"max",funcfont,funcfont.StringWidth("max")+extra,"\\max"},
		{"min",funcfont,funcfont.StringWidth("min")+extra,"\\min"},
		{"sec",funcfont,funcfont.StringWidth("sec")+extra,"\\sec"},
		{"sin",funcfont,funcfont.StringWidth("sin")+extra,"\\sin"},
		{"sinh",funcfont,funcfont.StringWidth("sinh")+extra,"\\sinh"},
		{"sup",funcfont,funcfont.StringWidth("sup")+extra,"\\sup"},
		{"tan",funcfont,funcfont.StringWidth("tan")+extra,"\\tan"},
		{"tanh",funcfont,funcfont.StringWidth("tanh")+extra,"\\tanh"},
		};
		//maxwidth=0;
		int num_func_items = 30;
		/*for(int n=0;n<num_func_items;n++)
		{
			if(functextitems[n].width > maxwidth)
				maxwidth = 	(int)functextitems[n].width;
		}
		*/
		//IsResizing = true;	
		//TileWidth=maxwidth;
		TileHeight=23;
		
		tiler_lt = tiler.LeftTop();
		
		tiler = BRect(tiler_lt.x,tiler_lt.y,tiler_lt.x+TileWidth,tiler_lt.y+TileHeight);
		f = v->Bounds();
		
		num_across=0;
		width = f.Width();
		twidth = tiler.Width()+1;
		while(width >=0)
		{
			width -= twidth;
			num_across++;
		}
		if(width != -1)
			num_across--;	
					
		j=1;
		for(i=0;i<num_func_items;i++)
		{
				
				if(!IsOnlyResizing)
				{	
					//BFont font(be_bold_font);
					//const char* text = "Bold";
					msg = new BMessage(InterfaceConstants::K_CMD_TBUTTON_INSERT);
					if(msg->AddString("cmd",functextitems[i].cmd) == B_OK)
					{
						v->AddChild(temp_button = new TButton(tiler,msg,functextitems[i].text,functextitems[i].font));
						helper->SetHelp(temp_button,functextitems[i].cmd);
					}
					tiler.OffsetBy(TileWidth+1,0);
					
					if(j == num_across)
					{
						tiler.left=0;
						tiler.right=TileWidth;
						tiler.OffsetBy(0,TileHeight+1);
						j = 0;
					}
					j++;
				}
				else
				{
						int child_index = i+child_offset;
						if(!IsGreekAbsent || !IsBigAbsent || !IsBinaryAbsent || !IsMiscAbsent || !IsBinRelAbsent
						|| !IsMMAAbsent || !IsIntlAbsent || !IsTypeFaceAbsent)
							child_index++;
						BView* child = v->ChildAt(child_index);
						if(child != NULL)
						{
							if(IsFunctionHidden)
							{
								if(!child->IsHidden())
								child->Hide();
							}
							else
							{
								if(child->IsHidden())
									child->Show();
						
								child->MoveTo(tiler.LeftTop());
								
								tiler.OffsetBy(TileWidth+1,0);
								
								if(j == num_across)
								{
									tiler.left=0;
									tiler.right=TileWidth;
									tiler.OffsetBy(0,TileHeight+1);
									j = 0;
								}
								j++;
							
							
							}
						}
				}
				
		}		
	}
	TotalHeight = tiler.bottom+fabs(ScrollBarOffset)-24;
	IsResizing = false;
		
}

void TexBar::ScrollBy(float h,float v)
{
	BView::ScrollBy(h,v);
	//Invalidate();
}
void TexBar::ScrollTo(BPoint where)
{
	BView::ScrollTo(where);
	//Invalidate();
}
void TexBar::FrameResized(float w, float h)
{
	if(!IsResizing)
		Tile(this,true);
	AdjustScrollBar();
	//Invalidate();
	BView::FrameResized(w,h);
}
void TexBar::RefreshColours()
{
	if(!IsGreekAbsent) GreekHeader->Invalidate();
	if(!IsBigAbsent) BigHeader->Invalidate();
	if(!IsBinaryAbsent) BinaryHeader->Invalidate();
	if(!IsMiscAbsent) MiscHeader->Invalidate();
	if(!IsBinRelAbsent) BinRelHeader->Invalidate();
	if(!IsMMAAbsent) MMAHeader->Invalidate();
	if(!IsIntlAbsent) IntlHeader->Invalidate();
	if(!IsTypeFaceAbsent) TypeFaceHeader->Invalidate();
	if(!IsFunctionAbsent) FunctionHeader->Invalidate();
}
void TexBar::AdjustScrollBar()
{
	BScrollBar* bar = ScrollBar(B_VERTICAL);
	if(bar)
	{
		BRect f = Frame();
		//float h = ToolBarHeight();
		//cout << h << endl;
		float vh = f.Height();
		if(TotalHeight < vh)
		{
			bar->SetRange(0,0);
		}
		else
		{
			bar->SetRange(0,TotalHeight-vh);	
			bar->SetProportion(vh/TotalHeight);
		}
		
	}
}

void TexBar::AttachedToWindow()
{
	BScrollBar* bar = ScrollBar(B_VERTICAL);
	if(bar)
		bar->SetSteps(50,100);
		
	AdjustScrollBar();
	BView::AttachedToWindow();	
}

void TexBar::MouseMoved(BPoint point,uint32 transit,const BMessage* msg)
{
	switch(transit)
	{
		case B_ENTERED_VIEW:
		{
			MakeFocus(true);
		}break;
		case B_EXITED_VIEW:
		{
			MakeFocus(false);
		}break;
	}
}
void TexBar::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case InterfaceConstants::K_HANDLE_HIERARCHY:
		{	
			bool value;
			if(msg->FindBool("Greek Letters",&value) == B_OK)
			{
				IsGreekHidden = value;
				prefs->IsGreekHidden = IsGreekHidden;
				Tile(this,true);
			}
			else if(msg->FindBool("Big Operators",&value) == B_OK)
			{
				IsBigHidden = value;
				prefs->IsBigHidden = IsBigHidden;
				Tile(this,true);
			}
			else if(msg->FindBool("Binary Operators",&value) == B_OK)
			{
				IsBinaryHidden = value;
				prefs->IsBinaryHidden = IsBinaryHidden;
				Tile(this,true);
			}
			else if(msg->FindBool("Misc Symbols",&value) == B_OK)
			{
				IsMiscHidden = value;
				prefs->IsMiscHidden = IsMiscHidden;
				Tile(this,true);
			}
			else if(msg->FindBool("Binary Relations",&value) == B_OK)
			{
				IsBinRelHidden = value;
				prefs->IsBinRelHidden = IsBinRelHidden;
				Tile(this,true);
			}
			else if(msg->FindBool("Math Mode Accents",&value) == B_OK)
			{
				IsMMAHidden = value;
				prefs->IsMMAHidden = IsMMAHidden;
				Tile(this,true);
			}
			else if(msg->FindBool("International",&value) == B_OK)
			{
				IsIntlHidden = value;
				prefs->IsIntlHidden = IsIntlHidden;
				Tile(this,true);
			}
			else if(msg->FindBool("Type Faces",&value) == B_OK)
			{
				IsTypeFaceHidden = value;
				prefs->IsTypeFaceHidden = IsTypeFaceHidden;
				Tile(this,true);
			}
			else if(msg->FindBool("Functions",&value) == B_OK)
			{
				IsFunctionHidden = value;
				prefs->IsFunctionHidden = IsFunctionHidden;
				Tile(this,true);
			}
			prefs->Save();
			AdjustScrollBar();
		}break;
		case B_MOUSE_WHEEL_CHANGED:
		{	
				float delta_y;
					if(msg->FindFloat("be:wheel_delta_y",&delta_y) == B_OK)
					{
						BScrollBar* bar = ScrollBar(B_VERTICAL);
						if(bar)
						{
							float small,big;
							bar->GetSteps(&small,&big);
							if(modifiers() & B_OPTION_KEY)
							{
								small *= 10;
								big *= 10;
							}
							float val = bar->Value();
							
							if(delta_y > 0)       //move wheel down/back
							{
								bar->SetValue(val+big);	
							}
							else if(delta_y < 0)	//move wheel up/forward
							{
								bar->SetValue(val-big);	
							}
						}
					}
					
		}break;
		default:
			BView::MessageReceived(msg);
			break;
	}
}



