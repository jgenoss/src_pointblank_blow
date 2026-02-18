#pragma once

namespace ui
{
	class ContentsView;

	class ContentsViewToolbar : public CDialogEx
	{
		DECLARE_DYNAMIC(ContentsViewToolbar)

	public:
		ContentsViewToolbar(CWnd* pParent = NULL);
		virtual ~ContentsViewToolbar();

		void SetTopicText(const std::wstring& wstrText);

	private:
		CBrush	m_bkGndBrush;
		CEdit	m_editSearchContents;
		CStatic m_textFindByKeywords;
		CStatic m_textTopic;
		CFont	m_fontTopic;

#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_CONTENTSVIEW_TOOLBAR };
#endif

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);
		virtual BOOL OnInitDialog();

		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnEnChangeEditSearchContents();
		afx_msg void OnBnClickedOK()		{}
		afx_msg void OnBnClickedCancel()	{}
	};
}
