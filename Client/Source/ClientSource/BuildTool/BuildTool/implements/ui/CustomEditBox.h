#pragma once

namespace ui
{
	class CustomButton;

	struct CustomEditBoxCallback
	{
		virtual ~CustomEditBoxCallback() {}
	};

	using CustomEditBoxCallbackPtr = std::unique_ptr<CustomEditBoxCallback>;

	class CustomEditBox : public CEdit
	{
		DECLARE_DYNAMIC(CustomEditBox)

	public:
		CustomEditBox();
		virtual ~CustomEditBox();

		void					SetCallback(CustomEditBoxCallbackPtr cb)	{ m_callback = std::move(cb); }
		CustomEditBoxCallback*	GetCallback() const							{ return m_callback.get(); }

		void SetColor(COLORREF bkColor, COLORREF textColor);

	private:
		CustomEditBoxCallbackPtr m_callback;
		COLORREF m_clrBkgnd;
		CBrush   m_brBkgnd;
		COLORREF m_clrText;

	private:
		DECLARE_MESSAGE_MAP()

	public:
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	};
}