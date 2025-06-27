#include "TButton.h"
#include <Screen.h>

BBitmap*
ConvertCMAP8ToGreyRGB32(BBitmap* b)
{
	BScreen screen;
	if (screen.IsValid() && b->ColorSpace() == B_CMAP8) {
		int size = b->BitsLength();
		BBitmap* grey_bitmap
			= new BBitmap(BRect(0, 0, b->Bounds().Width(), b->Bounds().Height()), B_RGB32);
		uint8* src = (uint8*)b->Bits();
		uint8* dest = (uint8*)grey_bitmap->Bits();

		int j = 0;
		for (int i = 0; i < grey_bitmap->BitsLength(); i += 4) {
			rgb_color r;

			r = screen.ColorForIndex(src[j]);
			if (src[j] == B_TRANSPARENT_MAGIC_CMAP8)
				r.alpha = 0;
			else
				r.alpha = 255;

			uint8 avg = (uint8)(0.3 * r.red + 0.59 * r.green
								+ 0.11 * r.blue);  //(r.red + r.blue + r.green)/3;
			dest[i] = dest[i + 1] = dest[i + 2] = avg;

			dest[i + 3] = r.alpha;

			j++;
		}
		return grey_bitmap;
	}
	return NULL;
}

TButton::TButton(
	const char* name, BRect frame, BMessage* msg, BBitmap* Icon /*,BBitmap* ColoredIcon*/)
	: BControl(frame, "tbutton", NULL, msg, B_FOLLOW_NONE, B_WILL_DRAW)
{
	if (name) {
		string sname = string("tbutton_") + name;
		SetName(sname.c_str());
	}
	frame = Bounds();
	icon = Icon;
	disabledicon = ConvertCMAP8ToGreyRGB32(icon);
	// delete ColoredIcon;
	// cicon = ColoredIcon;
	// TintBitmap(B_LIGHTEN_1_TINT);
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	IsDown = false;
	IsOver = false;
	OkToInvoke = false;
	IsMultipleIcon = false;
	IsStretched = false;
	IsText = false;
	IsAuxIcon = false;
}
TButton::TButton(BRect frame, BMessage* msg, const char* text, BFont font)
	: BControl(frame, "tbutton", NULL, msg, B_FOLLOW_NONE, B_WILL_DRAW)
{
	frame = Bounds();
	// delete ColoredIcon;
	// cicon = ColoredIcon;
	// TintBitmap(B_LIGHTEN_1_TINT);
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	IsDown = false;
	IsOver = false;
	OkToInvoke = false;
	IsMultipleIcon = false;
	IsStretched = false;
	IsAuxIcon = false;
	IsText = true;
	textfont = font;
	drawtext << text;
}
TButton::TButton(BRect frame, BMessage* msg, vector<BBitmap*> Icons, vector<BPoint> Origins)
	: BControl(frame, "tbutton", NULL, msg, B_FOLLOW_NONE, B_WILL_DRAW)
{
	frame = Bounds();
	icons = Icons;
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	IsDown = false;
	IsOver = false;
	OkToInvoke = false;
	origins = Origins;
	IsMultipleIcon = true;
	IsStretched = false;
	IsText = false;
	IsAuxIcon = false;
}
TButton::TButton(BRect frame, BMessage* msg, vector<BBitmap*> Icons, vector<BPoint> Origins,
	vector<float> Scales)
	: BControl(frame, "tbutton", NULL, msg, B_FOLLOW_NONE, B_WILL_DRAW)
{
	frame = Bounds();
	icons = Icons;
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	IsDown = false;
	IsOver = false;
	OkToInvoke = false;
	origins = Origins;
	IsMultipleIcon = true;
	scaler = Scales;
	IsStretched = true;
	IsText = false;
	IsAuxIcon = false;
}
void
TButton::SetAuxIcon(bool flag)
{
	IsAuxIcon = flag;
	Invalidate();
}
bool
TButton::AuxIcon()
{
	return IsAuxIcon;
}
void
TButton::AttachedToWindow()
{
	BControl::AttachedToWindow();
	SetTarget(Parent());
}
void
TButton::SetAuxBitmap(BBitmap* aux)
{
	auxicon = aux;
	auxdisabledicon = ConvertCMAP8ToGreyRGB32(auxicon);
}
void
TButton::Draw(BRect r)
{
	r = Bounds();
	SetDrawingMode(B_OP_COPY);
	SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	FillRect(r);
	if (IsDown && IsEnabled() && !IsAuxIcon) {
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		StrokeLine(BPoint(r.left, r.top), BPoint(r.left, r.bottom));
		StrokeLine(BPoint(r.left, r.top), BPoint(r.right, r.top));

		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_1_TINT));
		StrokeLine(BPoint(r.left, r.bottom), BPoint(r.right, r.bottom));
		StrokeLine(BPoint(r.right, r.top + 1), BPoint(r.right, r.bottom));
	}
	/*if(IsDown)
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_1_TINT));
	else
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_LIGHTEN_1_TINT));

	StrokeLine(BPoint(r.left,r.top),BPoint(r.left,r.bottom));
	StrokeLine(BPoint(r.left,r.top),BPoint(r.right,r.top));

	if(IsDown)
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_LIGHTEN_1_TINT));
	else
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_1_TINT));

	StrokeLine(BPoint(r.left,r.bottom),BPoint(r.right,r.bottom));
	StrokeLine(BPoint(r.right,r.top+1),BPoint(r.right,r.bottom));
	*/
	SetDrawingMode(B_OP_ALPHA);
	if (IsText) {
		SetDrawingMode(B_OP_OVER);
		const char* strarray[] = {drawtext.String()};
		BRect* strrect;
		//((BView*)this)->
		SetFont(&textfont);

		// textfont.GetBoundingBoxesForStrings(strarray,1,B_SCREEN_METRIC,NULL,strrect);
		// strrect[0].PrintToStream();
		float w = textfont.StringWidth(drawtext.String());	// strrect[0].Width();
		font_height fh;
		textfont.GetHeight(&fh);
		float h = fh.ascent + fh.descent;
		// float h=strrect[0].Height();
		SetHighColor(0, 0, 0);
		DrawString(drawtext.String(), BPoint((r.Width() - w) / 2, ((r.Height() - h) / 2) + h));
		// DrawString(drawtext.String(),BPoint(0,10));
	} else if (!IsMultipleIcon) {
		BBitmap* theIcon;
		BBitmap* theDisabledIcon;

		if (IsAuxIcon) {
			theIcon = auxicon;
			theDisabledIcon = auxdisabledicon;
		} else {
			theIcon = icon;
			theDisabledIcon = disabledicon;
		}

		MovePenTo(0, 0);
		float w = theIcon->Bounds().Width();
		float h = theIcon->Bounds().Height();

		if (IsEnabled() || theDisabledIcon == NULL) {
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(theIcon, BPoint((r.Width() - w) / 2, (r.Height() - h) / 2));
		} else {
			SetDrawingMode(B_OP_ALPHA);
			DrawBitmap(theDisabledIcon, BPoint((r.Width() - w) / 2, (r.Height() - h) / 2));
		}
	} else {
		for (int i = 0; i < icons.size(); i++) {
			float w = icons[i]->Bounds().Width();
			float h = icons[i]->Bounds().Height();
			if (IsStretched) {
				float swidth = origins[i].x + (r.Width() - w) / 2;
				float sheight = origins[i].y + (r.Height() - h) / 2;

				BRect source(swidth, sheight, swidth + w, sheight + h);
				BRect dest(source);
				float dwidth = dest.Width();
				float dscaledwidth = dwidth * scaler[i];
				float scale_amount = (dscaledwidth - dwidth) / 2;
				dest.left -= scale_amount;
				dest.right += scale_amount;

				DrawBitmap(icons[i], dest);
				// DrawBitmap(icons[i],BPoint(origins[i].x+(r.Width()-w)/2,origins[i].y+(r.Height()-h)/2));
			} else
				DrawBitmap(icons[i], BPoint(origins[i].x + (r.Width() - w) / 2,
										 origins[i].y + (r.Height() - h) / 2));
		}
	}
	/*SetHighColor(255,0,0);
	StrokeLine(BPoint(r.left,r.top),BPoint(r.left,r.bottom));
	StrokeLine(BPoint(r.left,r.top),BPoint(r.right,r.top));
	StrokeLine(BPoint(r.left,r.bottom),BPoint(r.right,r.bottom));
	StrokeLine(BPoint(r.right,r.top),BPoint(r.right,r.bottom));
	*/
}
void
TButton::Render()
{
	Invalidate();
}
void
TButton::MouseDown(BPoint point)
{
	// SetMouseEventMask(B_POINTER_EVENTS);
	uint32 button;
	GetMouse(&point, &button);
	if (button == B_PRIMARY_MOUSE_BUTTON && IsEnabled()) {
		IsDown = true;
		OkToInvoke = true;
		Render();
	}
}
void
TButton::MouseUp(BPoint point)
{
	IsDown = false;
	Render();

	if (OkToInvoke && IsEnabled() && !IsAuxIcon) {
		Invoke();
		OkToInvoke = false;
	}
}
void
TButton::MouseMoved(BPoint point, uint32 transit, const BMessage* msg)
{
	uint32 button;
	GetMouse(&point, &button);
	switch (transit) {
		case B_ENTERED_VIEW:
		{
			// always have !IsDown - default state
			/*if(button == B_PRIMARY_MOUSE_BUTTON)
			{
				OkToInvoke = true;
				IsDown = true;
				IsOver = true;
				Render();
			}*/
			// else
			//{
			IsOver = true;
			// OkToInvoke = true;
			// Render();
			//}
			// Parent()->Parent()->MakeFocus(true);
		} break;
		case B_EXITED_VIEW:
		{
			IsOver = false;
			if (IsDown) {
				IsDown = false;
				Render();
			}

			OkToInvoke = false;
			IsDown = false;
			// Render();
			// Parent()->Parent()->MakeFocus(false);
		} break;
	}
	Parent()->MouseMoved(point, transit, msg);
	// if(transit == B_EXITED_VIEW
	/*else
	{
		IsDown = false;
		Invalidate();
	}
	*/
}
void
TButton::TintBitmap(const float tint)
{
	/*	LightIcon = new BBitmap(BRect(0,0,31,31),B_RGB32);
		LightIcon->SetBits(icon->Bits(),icon->BitsLength(),0,B_RGB32);
		unsigned char* p = (unsigned char*)LightIcon->Bits();
		rgb_color temp;
		for(int i=0;i<LightIcon->BitsLength();i+=4)
		{
			//B G R A
			temp.blue = (uint8)p[i];
			temp.green = (uint8)p[i+1];
			temp.red = (uint8)p[i+2];
			temp.alpha = (uint8)p[i+3];

			temp = tint_color(temp,tint);

			p[i] = (unsigned char)temp.blue;
			p[i+1] = (unsigned char)temp.green;
			p[i+2] = (unsigned char)temp.red;
			p[i+3] = (unsigned char)temp.alpha;
		}
	*/
	/*LightIcon = new BBitmap(BRect(0,0,31,31),B_RGB32);
	LightIcon->SetBits(icon->Bits(),icon->BitsLength(),0,B_CMAP8);

	unsigned char* _p = (unsigned char*)(icon->Bits());
	bool A[1024];

	for(int i=0;i<1024;i++)
	{
		if(_p[i] == B_TRANSPARENT_MAGIC_CMAP8)
			A[i] = true;
		else
			A[i] = false;
	}

	unsigned char* p = (unsigned char*)(LightIcon->Bits());

	int temp_count=0;
	rgb_color temp_color;
	for(int i=0;i<=4096;i++)
	{
		if(temp_count != 3)
		{
			if(temp_count == 2)//red
				temp_color.red = (int)p[i];
			else if(temp_count == 1)//green
				temp_color.green = (int)p[i];
			else if(temp_count == 0)//blue
				temp_color.blue = (int)p[i];

			temp_count++;
		}
		else
		{
			temp_color = tint_color(temp_color,tint);
			p[i-1] = (unsigned char)temp_color.red;
			p[i-2] = (unsigned char)temp_color.green;
			p[i-3] = (unsigned char)temp_color.blue;

			//make sure we preserve transparency
			if(A[i/4] == true)
				p[i] = (unsigned char)0;
			temp_count = 0;
		}
	}*/
}
