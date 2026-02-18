#pragma once

#include "CustomUGCtrl.h"
#include "ugctrl/ugctelps.h"

namespace ui
{
	struct CustomUGEditBrowseCallback
	{
		virtual void OnSelectOK(const std::wstring& wstrPath) {}
		virtual ~CustomUGEditBrowseCallback() {}
	};

	using CustomEditUGBrowseCallbackPtr = std::unique_ptr<CustomUGEditBrowseCallback>;

	class CustomUGEditBrowseCtrl : public CustomUGCtrl
	{
	public:
		CustomUGEditBrowseCtrl();
		virtual ~CustomUGEditBrowseCtrl();

		void						SetCallback(CustomEditUGBrowseCallbackPtr cb)	{ m_callback = std::move(cb); }
		CustomUGEditBrowseCallback*	GetCallback() const								{ return m_callback.get(); }

		void AdjustLayout(const RECT& rt);
		void SetExplainText(const std::wstring& wstrText)	{ m_wstrExplain = wstrText; }
		void SetBackColor(const COLORREF clr)				{ m_backColor = clr; }
		void SetTextColor(const COLORREF clr)				{ m_textColor = clr; }
		void SetText(const std::wstring& wstrText)			{ QuickSetText(0, 0, wstrText.c_str()); RedrawCell(0, 0); }
		LPCTSTR GetText()									{ return QuickGetText(0, 0); }

		virtual void	OnSetup();
		virtual int		OnCellTypeNotify(long ID, int col, long row, long msg, long param);

	private:
		CustomEditUGBrowseCallbackPtr m_callback;
		CUGEllipsisType m_ellipType;
		std::wstring m_wstrExplain;
		COLORREF m_backColor;
		COLORREF m_textColor;
		CPen m_penIvoryBlack;
		CPen m_penIvoryBlackLight;
	};
}