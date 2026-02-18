#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// TextureUsageStateDlg dialog

class TextureUsageStateDlg : public CDialog
{
	DECLARE_DYNAMIC(TextureUsageStateDlg)

public:
	TextureUsageStateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TextureUsageStateDlg();

// Dialog Data
	enum { IDD = IDD_TEXTURE_USAGE_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CString m_strDir;
	CListCtrl m_TexListCtrl;

	INT32 m_nIndex;
	i3List m_TexIDList;
	i3List m_TexList;
	i3List m_NewTexList;
	char m_szPrefix[MAX_PATH];

	CString m_strMsg;
	CString m_RootDir;

public:

	void SetFileNamePrefix( const char *szName ){ strcpy( m_szPrefix, szName ); }

	BOOL GetTexFileName( char *szOut, i3Texture *pTex, INT32 &nIndex, GET_TEX_NAME_MODE nMode, BOOL bForConvert = TRUE );
	int GetTexIndex( i3Texture *pTex );

	void AddListItemCB( i3Texture *pTex );
	i3Texture* ReloadTexturesCB( i3Texture *pTex );

	virtual BOOL OnInitDialog();
	void MakeTexList();
	afx_msg void OnBnClickedButtonSerchFolder();
	afx_msg void OnBnClickedButtonTextureStateSave();
	afx_msg void OnBnClickedButtonReloadAll();
	afx_msg void OnBnClickedButtonSetMipmapFilter();
	afx_msg void OnBnClickedButtonRemoveMipmpaFilters();
	afx_msg void OnBnClickedButtonMakeExtern();
	afx_msg void OnBnClickedButtonMakeInternal();
	afx_msg void OnBnClickedButtonSetCompress();
	afx_msg void OnBnClickedButtonRemoveCompress();
	CStatic m_TotalCountCtrl;
	afx_msg void OnBnClickedBtnSelall();
	afx_msg void OnBnClickedBtnUnselall();
	afx_msg void OnBnClickedBtnSeldiffuse();
	afx_msg void OnBnClickedBtnSelnormal();
	afx_msg void OnBnClickedBtnSelspecular();
	afx_msg void OnBnClickedBtnSellux();
	afx_msg void OnBnClickedBtnReloadTexSearch();
};
