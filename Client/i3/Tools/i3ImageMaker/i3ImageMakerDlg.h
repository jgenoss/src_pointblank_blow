// i3ImageMakerDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// Ci3ImageMakerDlg 대화 상자
class Ci3ImageMakerDlg : public CDialog
{
// 생성
public:
	Ci3ImageMakerDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_i3ImageMaker_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV 지원

// 구현
protected:
	HICON m_hIcon;

	I3_PLATFORM			m_TargetPlatform;
	INT32				m_CompressMethod;
	const char*			m_szCmdLineArg;
	i3RenderContext *	m_pCtx;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	BOOL Convert( const char *szFileName );

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void UpdateControls( BOOL bTrue = TRUE );

public:
	void SetCommandLineArg( const char *szCmdLineArg ){ m_szCmdLineArg = szCmdLineArg; }

	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	CComboBox m_ctrPlatformCB;
	afx_msg void OnDestroy();
	CComboBox m_CompressCtrl;
	afx_msg void OnBnClickedCheckMipmap();
};
