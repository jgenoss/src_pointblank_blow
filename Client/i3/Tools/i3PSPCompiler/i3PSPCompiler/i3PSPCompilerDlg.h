// i3PSPCompilerDlg.h : 헤더 파일
//

#pragma once

#include "i3DataCompilerPSP.h"
#include "afxcmn.h"
#include "afxwin.h"


// Ci3PSPCompilerDlg 대화 상자
class Ci3PSPCompilerDlg : public CDialog
{
// 생성
public:
	Ci3PSPCompilerDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3PSPCOMPILER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

protected:
	i3RenderContext *			m_pContext;
	i3DataCompilerPSP *			m_pCompiler;

protected:
	void		GetOptions(void);
	void		SetOptions(void);
	void		ReplaceControls( int cx = -1, int cy = -1);

	void		SetVertexArrayOptEnable( BOOL bFlag);

public:
	void		Compile( const char * pszFile);

// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_ForceDither16Bits;
	BOOL m_bSwizzledImage;
	BOOL m_bNativeVertexFormat;
	BOOL m_bCompileStaticGeometry;
	BOOL m_bAddClippingBoundBox;
	BOOL m_bOptimizeIndexArray;
	CListCtrl m_LogListCtrl;
	CStatic m_SplitCtrl;
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CComboBox m_PosTypeCtrl;
	CComboBox m_WeightTypeCtrl;
	CComboBox m_NormalTypeCtrl;
	CComboBox m_TexTypeCtrl;
	CComboBox m_ColorTypeCtrl;
	CStatic m_PosStCtrl;
	CStatic m_WeightStCtrl;
	CStatic m_NormalStCtrl;
	CStatic m_TexStCtrl;
	CStatic m_ColorStCtrl;
	afx_msg void OnBnClickedChkNativevertexformat();
	BOOL m_bRotateAxis;
	CButton m_PSPPostfixCtrl;
	CComboBox m_PlatformCtrl;
	CStatic m_StPlatformCtrl;
};
