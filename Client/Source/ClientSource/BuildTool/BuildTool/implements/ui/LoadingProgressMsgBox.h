#pragma once

namespace ui
{
	// LoadingProgressMsgBox 대화 상자입니다.

	class LoadingProgressMsgBox : public CDialogEx
	{
		DECLARE_DYNAMIC(LoadingProgressMsgBox)

	public:
		LoadingProgressMsgBox(CWnd* pParent = NULL);   // 표준 생성자입니다.
		virtual ~LoadingProgressMsgBox();

		// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_DIALOG_LOADING_PROGRESS };
#endif

		void SetText(const std::wstring& wstrText);

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
		virtual BOOL OnInitDialog();

		DECLARE_MESSAGE_MAP()
		afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	private:
		CStatic m_textLoadingWhat;
		CFont	m_font;
		CBrush	m_bkGndBrushBlack;
	};
}
