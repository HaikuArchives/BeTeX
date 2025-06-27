/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef GRADIENT_VIEW_H
#include "GradientView.h"
#endif

GradientView::GradientView(
	BRect frame, rgb_color top, rgb_color bottom, gradient_orientation orientation)
	: DoubleBufferedView(frame, "zGradientView", B_FOLLOW_NONE, B_WILL_DRAW),
	  m_orientation(orientation),
	  m_topColor(top),
	  m_bottomColor(bottom),
	  m_oldTopColor(top),
	  m_oldBottomColor(bottom)
{
	SetOrientation(m_orientation);
}

GradientView::~GradientView() {}

void
GradientView::DrawContent(BView* backView)
{
	BRect bounds = backView->Bounds();
	switch (m_orientation) {
		// verticals
		case K_TOP_BOTTOM:
		case K_BOTTOM_TOP:
		{
			backView->SetDrawingMode(B_OP_COPY);

			BPoint x1 = bounds.LeftTop(), x2 = bounds.RightTop(), end = bounds.LeftBottom();

			int i = 0;
			int size = int(bounds.Height() + 1);

			backView->BeginLineArray(size);
			while (x1.y != end.y + 1) {
				backView->AddLine(x1, x2, m_v[i]);
				i++;
				x1.y++;
				x2.y++;
			}
			backView->EndLineArray();

			backView->SetDrawingMode(B_OP_COPY);
		} break;
		// horizontals
		case K_LEFT_RIGHT:
		case K_RIGHT_LEFT:
		{
			backView->SetDrawingMode(B_OP_COPY);

			BPoint x1 = bounds.LeftTop(), x2 = bounds.LeftBottom(), end = bounds.RightTop();

			int i = 0;
			int size = int(bounds.Width() + 1);

			backView->BeginLineArray(size);
			while (x1.x != end.x + 1) {
				backView->AddLine(x1, x2, m_v[i]);
				i++;
				x1.x++;
				x2.x++;
			}
			backView->EndLineArray();

			backView->SetDrawingMode(B_OP_COPY);
		} break;
		case K_RIGHT_BOTTOM_TOP_LEFT:
		case K_TOP_LEFT_RIGHT_BOTTOM:
		case K_LEFT_BOTTOM_TOP_RIGHT:
		case K_TOP_RIGHT_LEFT_BOTTOM:
		{
			DrawDiagonal(backView, bounds, m_v_vert, m_v_horiz);
		} break;
		case K_CENTER_PYRAMID:
		case K_BORDER_PYRAMID:
		{
			BPoint topMid(bounds.Width() / 2.0f, bounds.top);
			BPoint rightMid(bounds.right, bounds.Height() / 2.0f);
			BPoint bottomMid(bounds.Width() / 2.0f, bounds.bottom);
			BPoint leftMid(bounds.left, bounds.Height() / 2);
			BPoint theMid(bounds.Width() / 2.0f, bounds.Height() / 2.0f);

			BRect topLeft(bounds.LeftTop(), theMid);
			BRect topRight(topMid, rightMid);
			BRect bottomRight(theMid, bounds.RightBottom());
			BRect bottomLeft(leftMid, bottomMid);

			DrawDiagonal(backView, topLeft, m_pvv_tl, m_pvh_tl);
			DrawDiagonal(backView, topRight, m_pvv_tr, m_pvh_tr);
			DrawDiagonal(backView, bottomRight, m_pvv_br, m_pvh_br);
			DrawDiagonal(backView, bottomLeft, m_pvv_bl, m_pvh_bl);
		} break;
	}
	backView->Sync();
}

void
GradientView::SetOrientation(gradient_orientation orientation)
{
	m_orientation = orientation;

	BRect bounds = Bounds();
	int vsize = int(bounds.Height() + 1);
	int hsize = int(bounds.Width() + 1);

	switch (m_orientation) {
		// verticals
		case K_TOP_BOTTOM:
		case K_BOTTOM_TOP:
		{
			if (m_orientation == K_BOTTOM_TOP)
				SwitchColors();

			vector<rgb_color> v1(vsize);

			int lo = vsize - 1;
			v1[lo] = m_bottomColor;

			int hi = 0;
			v1[hi] = m_topColor;

			FillGradientVector(v1, m_topColor, m_bottomColor, hi, lo);
			RestoreColors();
			m_v = v1;
		} break;
		// horizontals
		case K_LEFT_RIGHT:
		case K_RIGHT_LEFT:
		{
			if (m_orientation == K_RIGHT_LEFT)
				SwitchColors();

			vector<rgb_color> v1(hsize);

			int lo = hsize - 1;
			v1[lo] = m_bottomColor;

			int hi = 0;
			v1[hi] = m_topColor;

			FillGradientVector(v1, m_topColor, m_bottomColor, hi, lo);
			RestoreColors();
			m_v = v1;
		} break;
		// diagonals
		case K_RIGHT_BOTTOM_TOP_LEFT:
		case K_TOP_LEFT_RIGHT_BOTTOM:
		case K_LEFT_BOTTOM_TOP_RIGHT:
		case K_TOP_RIGHT_LEFT_BOTTOM:
		{
			vector<rgb_color> v1(vsize);
			vector<rgb_color> v2(hsize);

			// bools kloppen niet!!!
			FillDiagonal(v1, v2,
				(m_orientation == K_RIGHT_BOTTOM_TOP_LEFT
					|| m_orientation == K_LEFT_BOTTOM_TOP_RIGHT),
				(m_orientation == K_TOP_LEFT_RIGHT_BOTTOM
					|| m_orientation == K_TOP_RIGHT_LEFT_BOTTOM));
			m_v_vert = v1;
			m_v_horiz = v2;
		} break;
		// pyramids
		case K_CENTER_PYRAMID:
		case K_BORDER_PYRAMID:
		{
			vsize = vsize / 2 + 1;
			hsize = hsize / 2 + 1;

			vector<rgb_color> v1(vsize);
			vector<rgb_color> v2(hsize);

			vector<rgb_color> v3(vsize);
			vector<rgb_color> v4(hsize);

			vector<rgb_color> v5(vsize);
			vector<rgb_color> v6(hsize);

			vector<rgb_color> v7(vsize);
			vector<rgb_color> v8(hsize);

			// top left
			FillDiagonal(
				v1, v2, m_orientation == K_CENTER_PYRAMID, m_orientation == K_CENTER_PYRAMID);
			// top right
			FillDiagonal(
				v3, v4, m_orientation == K_CENTER_PYRAMID, m_orientation == K_BORDER_PYRAMID);
			// bottom left
			FillDiagonal(
				v5, v6, m_orientation == K_BORDER_PYRAMID, m_orientation == K_CENTER_PYRAMID);
			// bottom right
			FillDiagonal(
				v7, v8, m_orientation == K_BORDER_PYRAMID, m_orientation == K_BORDER_PYRAMID);

			m_pvh_tl = v2;
			m_pvv_tl = v1;

			m_pvh_tr = v4;
			m_pvv_tr = v3;

			m_pvh_bl = v6;
			m_pvv_bl = v5;

			m_pvh_br = v8;
			m_pvv_br = v7;
		} break;
		default:
			break;
	}
	if (Window())
		Invalidate();
}

GradientView::gradient_orientation
GradientView::Orientation()
{
	return m_orientation;
}

void
GradientView::SetTopColor(rgb_color top)
{
	// update the color
	m_topColor = top;
	m_oldTopColor = top;
	// refill the vector
	Refill();
}

rgb_color
GradientView::TopColor()
{
	// m_oldTopColor always has the value to which top color was set
	return m_oldTopColor;
}

void
GradientView::SetBottomColor(rgb_color bottom)
{
	// update the color
	m_bottomColor = bottom;
	m_oldBottomColor = bottom;
	// refill the vector
	Refill();
}

rgb_color
GradientView::BottomColor()
{
	// m_oldBottomColor always has the value to which bottom color was set
	return m_oldBottomColor;
}

void
GradientView::SwitchColors()
{
	rgb_color temp = m_topColor;
	m_topColor = m_bottomColor;
	m_bottomColor = temp;
	// refill the vector
	Refill();
}

void
GradientView::Refill()
{
	switch (m_orientation) {
		// verticals
		case K_TOP_BOTTOM:
		case K_BOTTOM_TOP:
		{
			if (m_orientation == K_BOTTOM_TOP)
				SwitchColors();

			int lo = m_v.size() - 1;
			int hi = 0;
			m_v[lo] = m_bottomColor;
			m_v[hi] = m_topColor;

			FillGradientVector(m_v, m_topColor, m_bottomColor, hi, lo);
			RestoreColors();
		} break;
		// horizontals
		case K_LEFT_RIGHT:
		case K_RIGHT_LEFT:
		{
			if (m_orientation == K_RIGHT_LEFT)
				SwitchColors();

			int lo = m_v.size() - 1;
			m_v[lo] = m_bottomColor;

			int hi = 0;
			m_v[hi] = m_topColor;

			FillGradientVector(m_v, m_topColor, m_bottomColor, hi, lo);
			RestoreColors();
		} break;
		// diagonals
		case K_RIGHT_BOTTOM_TOP_LEFT:
		case K_TOP_LEFT_RIGHT_BOTTOM:
		case K_LEFT_BOTTOM_TOP_RIGHT:
		case K_TOP_RIGHT_LEFT_BOTTOM:
		{
			// bools kloppen niet!!!
			FillDiagonal(m_v_vert, m_v_horiz,
				(m_orientation == K_RIGHT_BOTTOM_TOP_LEFT
					|| m_orientation == K_LEFT_BOTTOM_TOP_RIGHT),
				(m_orientation == K_TOP_LEFT_RIGHT_BOTTOM
					|| m_orientation == K_TOP_RIGHT_LEFT_BOTTOM));
		} break;
		case K_CENTER_PYRAMID:
		case K_BORDER_PYRAMID:
		{
			// top left
			FillDiagonal(m_pvv_tl, m_pvh_tl, m_orientation == K_CENTER_PYRAMID,
				m_orientation == K_CENTER_PYRAMID);
			// top right
			FillDiagonal(m_pvv_tr, m_pvh_tr, m_orientation == K_CENTER_PYRAMID,
				m_orientation == K_BORDER_PYRAMID);
			// bottom left
			FillDiagonal(m_pvv_bl, m_pvh_bl, m_orientation == K_BORDER_PYRAMID,
				m_orientation == K_CENTER_PYRAMID);
			// bottom right
			FillDiagonal(m_pvv_br, m_pvh_br, m_orientation == K_BORDER_PYRAMID,
				m_orientation == K_BORDER_PYRAMID);
		} break;
	}

	Invalidate();
}

void
GradientView::RestoreColors()
{
	m_topColor = m_oldTopColor;
	m_bottomColor = m_oldBottomColor;
}

void
GradientView::FillDiagonal(vector<rgb_color>& vert_vec, vector<rgb_color>& horiz_vec,
	bool switch_vertical, bool switch_horizontal)
{
	// fill vertical
	if (switch_vertical)
		SwitchColors();

	int lo = vert_vec.size() - 1;
	vert_vec[lo] = m_bottomColor;

	int hi = 0;
	vert_vec[hi] = m_topColor;

	FillGradientVector(vert_vec, m_topColor, m_bottomColor, hi, lo);
	RestoreColors();

	// fill horizontal
	if (switch_horizontal)
		SwitchColors();

	lo = horiz_vec.size() - 1;
	horiz_vec[lo] = m_bottomColor;

	hi = 0;
	horiz_vec[hi] = m_topColor;

	FillGradientVector(horiz_vec, m_topColor, m_bottomColor, hi, lo);
	RestoreColors();
}

void
GradientView::DrawDiagonal(
	BView* backView, BRect r, vector<rgb_color>& draw_vert, vector<rgb_color>& draw_horiz)
{
	backView->SetDrawingMode(B_OP_COPY);
	//******DRAW VERTICAL********//
	int size = int(r.Height() + 1);

	BPoint x1 = r.LeftTop(), x2 = r.RightTop(), end = r.LeftBottom();

	int i = 0;

	backView->BeginLineArray(size);
	while (x1.y != end.y + 1) {
		backView->AddLine(x1, x2, draw_vert[i++]);
		x1.y++;
		x2.y++;
	}
	backView->EndLineArray();

	backView->SetDrawingMode(B_OP_COPY);
	backView->SetDrawingMode(B_OP_BLEND);

	size = int(r.Width() + 1);
	x1 = r.LeftTop();
	x2 = r.LeftBottom();
	end = r.RightTop();
	i = 0;

	backView->SetDrawingMode(B_OP_BLEND);
	backView->BeginLineArray(size);
	while (x1.x != end.x + 1) {
		backView->AddLine(x1, x2, draw_horiz[i++]);
		x1.x++;
		x2.x++;
	}
	backView->EndLineArray();

	backView->SetDrawingMode(B_OP_COPY);
}

rgb_color
GradientView::GetMidColor(rgb_color c1, rgb_color c2)
{
	rgb_color temp;
	temp.red = (c1.red + c2.red) / 2;
	temp.green = (c1.green + c2.green) / 2;
	temp.blue = (c1.blue + c2.blue) / 2;
	temp.alpha = 255;

	return temp;
}

void
GradientView::FillGradientVector(vector<rgb_color>& v, rgb_color c1, rgb_color c2, int hi, int lo)
{
	// not sure why this guard works :-)
	if (hi != lo - 1) {
		// calculate middle
		int mid = (lo + hi) / 2;
		// store the first mid...
		v[mid] = GetMidColor(c1, c2);
		// I'll take the lo road
		FillGradientVector(v, v[mid], v[lo], mid, lo);
		// I'll take the hi road
		FillGradientVector(v, v[hi], v[mid], hi, mid);
	}
}
