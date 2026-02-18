#pragma once

#include "CustomUGCtrl.h"

namespace ui
{
	struct CustomUGEditBoxCallback
	{
		virtual ~CustomUGEditBoxCallback() {}
	};

	using CustomUGEditBoxCallbackPtr = std::unique_ptr<CustomUGEditBoxCallback>;

	class CustomUGEditBox : public CustomUGCtrl
	{
	public:
		CustomUGEditBox();
		virtual ~CustomUGEditBox();

		void						SetCallback(CustomUGEditBoxCallbackPtr cb)	{ m_callback = std::move(cb); }
		CustomUGEditBoxCallback*	GetCallback() const							{ return m_callback.get(); }

		void SetText(const std::string& strText);
		void SetText(const std::wstring& wstrText);

		void AdjustLayout(const RECT& rt);
		void SetBackColor(const COLORREF clr) { m_backColor = clr; }
		void SetTextColor(const COLORREF clr) { m_textColor = clr; }
		void SetAlignment(const short align) { m_alignment = align; }

		virtual void	OnSetup();

	private:
		CustomUGEditBoxCallbackPtr m_callback;
		COLORREF m_backColor = COLOR_IVORY_BLACK;
		COLORREF m_textColor = COLOR_SILVER;
		COLORREF m_borderColor = COLOR_IVORY_BLACK_LIGHT;
		CPen m_penBorder;
		short m_alignment = (UG_ALIGNLEFT | UG_ALIGNTOP);
	};
}