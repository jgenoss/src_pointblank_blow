#pragma once

#include "mmc.h"

#include "i3TDKListCtrl_Impl.h"

class CImageViewCtrl : public i3TDKImageViewCtrl
{
public:
	DECLARE_MESSAGE_MAP()

	void OnMouseMove(UINT nFlags, CPoint point);

public:
	void ResetScrollPos();
	void ResetImage();
};
