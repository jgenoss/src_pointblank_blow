// ExpectionCheckDlg.h : 헤더 파일
//

#pragma once

//#include "i3MapFileParser.h"
#include "afxwin.h"

// CExpectionCheckDlg 대화 상자
class CExpectionCheckDlg : public CDialog
{
	CString				m_szECode;
	CString				m_szEMapFile;
	CString				m_szEAddress;
	CString				m_szECodeResult;
	CListBox			m_ctLCallStack;	
	CListBox			m_ctLProcess;
	CString				m_szEInput;

	CString				m_szEOS;
	CString 			m_szECPU;
	CString 			m_szEGPUCard;
	CString 			m_szEGPUDate;
	CString 			m_szEGPUVer;
	CString 			m_szEGPUProv;
	CString 			m_szMEMTotalPhys;
	CString 			m_szMEMAvailPhys;
	CString 			m_szMEMTotalVir;
	CString 			m_szMEMAvailVir;
	CString 			m_szEMultMedia;
	CString 			m_szEDirectX;

// 생성
public:
	CExpectionCheckDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_EXPECTIONCHECK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


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
	afx_msg void OnBnClickedBMapopen();
	afx_msg void OnBnClickedBCallcatch();	
	afx_msg void OnBnClickedBClean();

	afx_msg void OnBnClickedBCode();
	afx_msg void OnBnClickedBInfo();

	void				GetOS();
	void				GetCPU();
	void				GetGPU();
	void				GetMemory();
	void				GetMultimedia();
	void				GetDirectX();
	void				GetProcess();
	void				GetErrorCode();
	void				GetCallStack();	
	CString				GetPrivateString( CString szEInput, char* pszSection, char* pszKey );
	void				SetErroeCode( CString szCode );
	void				SetCallStack( CString szAddress );
};
