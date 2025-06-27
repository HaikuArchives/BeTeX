#ifndef _TBUTTON_H_
#define _TBUTTON_H_

#include <AppFileInfo.h>
#include <Application.h>
#include <Bitmap.h>
#include <Control.h>
#include <File.h>
#include <Mime.h>
#include <String.h>
#include <View.h>
#include <iostream>
#include <vector>
using std::string;
using std::vector;

class TButton : public BControl {
public:
	TButton(const char* name, BRect frame, BMessage* msg, BBitmap* Icon);
	TButton(BRect frame, BMessage* msg, vector<BBitmap*> Icons, vector<BPoint> Origins);
	TButton(BRect frame, BMessage* msg, vector<BBitmap*> Icons, vector<BPoint> Origins,
		vector<float> Scales);
	TButton(BRect frame, BMessage* msg, const char* text, BFont font);
	virtual void AttachedToWindow();
	virtual void Draw(BRect r);
	virtual void MouseDown(BPoint point);
	virtual void MouseUp(BPoint point);
	virtual void MouseMoved(BPoint point, uint32 transit, const BMessage* msg);
	void TintBitmap(const float tint);
	void Render();
	void SetAuxBitmap(BBitmap* aux);
	void SetAuxIcon(bool flag);
	bool AuxIcon();

private:
	BBitmap* icon;
	BBitmap* disabledicon;

	BBitmap* auxicon;
	BBitmap* auxdisabledicon;
	bool IsAuxIcon;

	int IsDown;
	bool IsOver;
	bool OkToInvoke;
	bool IsMultipleIcon;
	bool IsText;
	vector<BBitmap*> icons;
	vector<BPoint> origins;
	bool IsStretched;
	vector<float> scaler;
	BFont textfont;
	BString drawtext;
};
#endif
