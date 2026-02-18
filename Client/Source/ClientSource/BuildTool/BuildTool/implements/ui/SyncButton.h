#pragma once

namespace ui
{
	class SyncButton : public CButton
	{
		DECLARE_DYNCREATE(SyncButton)

	public:
		SyncButton();
		virtual ~SyncButton();

	public:
#ifdef _DEBUG
		virtual void AssertValid() const;
#ifndef _WIN32_WCE
		virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	protected:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		DECLARE_MESSAGE_MAP()

	public:
		void OnClicked();
		void AdjustLayout(const RECT& rt);

		void SetButtonEnable(const bool enable);
		bool GetButtonEnable() const;

	private:
		CFont m_font;

		bool m_enable = true;
	};
}