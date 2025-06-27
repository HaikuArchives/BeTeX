#ifndef DOC_VIEW_H
#define DOC_VIEW_H

#include <be/interface/Bitmap.h>
#include <be/interface/Rect.h>
#include <be/interface/View.h>

class DocView : public BView {
public:
	DocView(BRect frame);
	virtual ~DocView();

	virtual void Draw(BRect r);

private:
	BBitmap* m_logo;
};

#endif
