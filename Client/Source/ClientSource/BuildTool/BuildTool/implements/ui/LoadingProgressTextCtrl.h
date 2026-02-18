#pragma once

#include "ugctrl/UGCtrl.h"

namespace ui
{
	class LoadingProgressTextCtrl : public CUGCtrl
	{
	public:
		LoadingProgressTextCtrl();
		virtual ~LoadingProgressTextCtrl();

	protected:
		DECLARE_MESSAGE_MAP()

		virtual void OnSetup();

		virtual COLORREF OnGetDefBackColor(int section);
		virtual void OnDrawFocusRect(CDC *dc, RECT *rect);

	public:
		void SetText(const std::wstring& wstrText);
		void AdjustLayout(const RECT& rect);

	private:
		CPen m_cellBorderColorPen;
	};
}