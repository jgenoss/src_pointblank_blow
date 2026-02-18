// i3MipmapGenDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class TEXINFO
{
public:
	i3Texture *pTex;
	char szPath[MAX_PATH];

	TEXINFO()
	{
		pTex = NULL;
		szPath[0] = 0;
	}

	~TEXINFO()
	{
		Clear();
	}

	void Clear()
	{
		I3_SAFE_RELEASE( pTex );
		szPath[0] = 0;
	}
};


// Ci3MipmapGenDlg 대화 상자
class Ci3MipmapGenDlg : public CDialog
{
// 생성
public:
	Ci3MipmapGenDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_I3MIPMAPGEN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

// 구현
protected:
	HICON m_hIcon;

	TEXINFO m_TexInfo[8];
	char m_szInputFileName[256];
	char m_szListFileName[256];
	const char*			m_szCmdLineArg;

	BOOL AddImage( const char *szPath );

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void SetCommandLineArg( const char *szCmdLineArg ){ m_szCmdLineArg = szCmdLineArg; }

	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedOk();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	CListCtrl m_ctrlMipmapList;
	afx_msg void OnLvnKeydownListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
