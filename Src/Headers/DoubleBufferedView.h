/*****************************************************************
 * Copyright (c) 2005 Tim de Jong								 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef DOUBLE_BUFFERED_VIEW
#define DOUBLE_BUFFERED_VIEW

#include <be/interface/Bitmap.h>
#include <be/interface/Rect.h>
#include <be/interface/View.h>

/** Abstract class that implements a double buffered view.
	Most of the code based on idea by Ramshankar
	@author Tim de Jong
 */
class DoubleBufferedView : public BView {
public:
	DoubleBufferedView(BRect frame, const char* name, uint32 resizingMode, uint32 flags);
	virtual ~DoubleBufferedView();

	virtual void AttachedToWindow();
	virtual void FrameResized(float width, float height);
	virtual void Draw(BRect updateRect);

	virtual void Invalidate();

	virtual void RenderView();
	/** This abstract function has to be implemented by the subclasses to draw
		the actual view content. The content has to be drawn into the backView.
		@param backView, the view that will be used for the double buffered drawing,
		the backView will be passed as a parameter by the RenderView function.
	 */
	virtual void DrawContent(BView* backView) = 0;
	BBitmap* GetBufferBitmap();

private:
	BBitmap* m_backBitmap;
	BView* m_backView;
};

#endif
