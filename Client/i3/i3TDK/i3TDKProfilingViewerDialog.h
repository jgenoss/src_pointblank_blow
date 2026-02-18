#pragma once
#include "afxcmn.h"

#if !defined( I3_NO_PROFILE)
#include "i3TDKProfilingViewerCtrl.h"

// i3TDKProfilingViewerDialog 대화 상자

class I3_EXPORT_TDK i3TDKProfilingViewerDialog : public CDialog
{
// 생성
public:

	i3TDKProfilingViewerDialog(CWnd* pParent = nullptr) : CDialog(IDD_PROFILINGVIEWER_DIALOG, pParent) {}	// 표준 생성자
	virtual ~i3TDKProfilingViewerDialog( void);

	i3TDKProfilingViewerCtrl *		m_pProfileCtrl = nullptr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

	enum { IDD_PROFILING_VIEWER =		999		};

// 구현
protected:

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	//virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = nullptr);
	//virtual BOOL Create(CWnd* pParentWnd = nullptr);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	void	UpdateReport( REAL_PROFILE_NODE * pRoot)
	{
		if( m_pProfileCtrl)	m_pProfileCtrl->UpdateReport( pRoot);
	}
};


#endif