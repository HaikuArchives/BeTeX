/*******************************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski                             *
 *                                                                             *
 * All rights reserved.                                                        *
 * Distributed under the terms of the MIT License.                             *
 ******************************************************************************/
#ifndef GRADIENT_VIEW_H
#define GRADIENT_VIEW_H

#include <vector>
using std::vector;

#include "DoubleBufferedView.h"

/**	GradientView
	@author Brent Miszalski
	@author Tim de Jong
	@comment cleaned up code, moved orientation enum into class, added virtual
	destructor, added Orientation() method, made some methods private.

	TODO: implement correct bool code for diagonals!!!
 */
class GradientView : public DoubleBufferedView {
public:
	// for GradientView - used to specify gradient orientation
	enum gradient_orientation {
		K_TOP_BOTTOM = 0,
		K_BOTTOM_TOP,
		K_LEFT_RIGHT,
		K_RIGHT_LEFT,
		K_RIGHT_BOTTOM_TOP_LEFT,
		K_TOP_LEFT_RIGHT_BOTTOM,
		K_LEFT_BOTTOM_TOP_RIGHT,
		K_TOP_RIGHT_LEFT_BOTTOM,
		K_CENTER_PYRAMID,
		K_BORDER_PYRAMID
	};

public:
	GradientView(BRect frame, rgb_color top, rgb_color bottom,
		gradient_orientation orientation = K_TOP_BOTTOM);
	virtual ~GradientView();

	void DrawContent(BView* backView);
	void SetOrientation(gradient_orientation orientation);
	gradient_orientation Orientation();

	void SetTopColor(rgb_color top);
	rgb_color TopColor();

	void SetBottomColor(rgb_color bottom);
	rgb_color BottomColor();

private:
	void SwitchColors();
	void Refill();
	void RestoreColors();
	void FillDiagonal(vector<rgb_color>& vert_vec, vector<rgb_color>& horiz_vec,
		bool switch_vertical, bool switch_horizontal);
	void DrawDiagonal(
		BView* backView, BRect r, vector<rgb_color>& draw_vert, vector<rgb_color>& draw_horiz);
	void FillGradientVector(vector<rgb_color>& v, rgb_color c1, rgb_color c2, int hi, int lo);
	rgb_color GetMidColor(rgb_color c1, rgb_color c2);

private:
	gradient_orientation m_orientation;
	rgb_color m_topColor, m_bottomColor, m_oldTopColor, m_oldBottomColor;

	vector<rgb_color> m_v, m_v_horiz, m_v_vert, m_pvh_tl, m_pvv_tl, m_pvh_tr, m_pvv_tr, m_pvh_bl,
		m_pvv_bl, m_pvh_br, m_pvv_br;
};
#endif
