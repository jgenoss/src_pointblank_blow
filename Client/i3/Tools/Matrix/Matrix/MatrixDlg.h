// MatrixDlg.h : 헤더 파일
//

#pragma once


// CMatrixDlg 대화 상자
class CMatrixDlg : public CDialog
{
// 생성
public:
	CMatrixDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_MATRIX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

	bool	IsDigitString( CString * pStr);
	void	Mul( CString * pOut, CString * pIn1, CString * pIn2, bool bReset = false);
	void	Add( CString * pOut, CString * pIn1, CString * pIn2, CString * pIn3, CString * pIn4, bool bReset = false);

	void	CalcMatrix(void);

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
	CString m_S1_00;
	CString m_S1_01;
	CString m_S1_02;
	CString m_S1_03;
	CString m_S1_10;
	CString m_S1_11;
	CString m_S1_12;
	CString m_S1_13;
	CString m_S1_20;
	CString m_S1_21;
	CString m_S1_22;
	CString m_S1_23;
	CString m_S1_30;
	CString m_S1_31;
	CString m_S1_32;
	CString m_S1_33;

	CString m_S2_00;
	CString m_S2_01;
	CString m_S2_02;
	CString m_S2_03;
	CString m_S2_10;
	CString m_S2_11;
	CString m_S2_12;
	CString m_S2_13;
	CString m_S2_20;
	CString m_S2_21;
	CString m_S2_22;
	CString m_S2_23;
	CString m_S2_30;
	CString m_S2_31;
	CString m_S2_32;
	CString m_S2_33;

	CString m_D_00;
	CString m_D_01;
	CString m_D_02;
	CString m_D_03;
	CString m_D_10;
	CString m_D_11;
	CString m_D_12;
	CString m_D_13;
	CString m_D_20;
	CString m_D_21;
	CString m_D_22;
	CString m_D_23;
	CString m_D_30;
	CString m_D_31;
	CString m_D_32;
	CString m_D_33;
	afx_msg void OnBnClickedBtnCalc();
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:

//	afx_msg void OnEnChangeEdPrefix1();
//	afx_msg void OnEnChangeEdPrefix4();
	CString m_Prefix1;
	CString m_Prefix2;
	CString m_X1;
	CString m_Y1;
	CString m_Z1;
	CString m_X2;
	CString m_Y2;
	CString m_Z2;
	afx_msg void OnBnClickedBtnIdentity();
	afx_msg void OnBnClickedBtnIdentity2();
	afx_msg void OnBnClickedBtnPrefix1();
	afx_msg void OnBnClickedBtnPrefix2();
};
