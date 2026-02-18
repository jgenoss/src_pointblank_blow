#pragma once

namespace ui
{
	class CustomButton;

	struct CustomButtonCallback
	{
		virtual void OnClick(CustomButton* ctrl) {}
		virtual void OnLButtonDown(CustomButton* ctrl) {}
		virtual void OnLButtonUp(CustomButton* ctrl) {}

		virtual ~CustomButtonCallback() {}
	};

	class CustomButton : public CButton
	{
		DECLARE_DYNAMIC(CustomButton)

	public:
		CustomButton();
		virtual ~CustomButton();

		using CustomCallbackPtr = std::unique_ptr<CustomButtonCallback>;
		void					SetCallback(CustomCallbackPtr cb)	{ m_callback = std::move(cb); }
		CustomButtonCallback*	GetCallback() const					{ return m_callback.get(); }

		void SetColor(COLORREF bkColor, COLORREF textColor);

	private:
		CustomCallbackPtr m_callback;
		COLORREF m_clrBkgnd;
		CBrush   m_brBkgnd;
		COLORREF m_clrText;

	private:
		DECLARE_MESSAGE_MAP()

	public:
		afx_msg void OnBnClicked();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	};
}