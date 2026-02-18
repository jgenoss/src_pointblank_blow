#pragma once
#include "afxwin.h"

enum GET_TEX_NAME_MODE
{
    GET_TEX_NAME_UI,
	GET_TEX_NAME_SAVE,
	GET_TEX_NAME_RELOAD,
};

typedef i3Texture* (*TRACE_TEX_CB)( void *pUserData, i3Texture *pTexture );

// i3TDKDlgTextureTool dialog

class I3_EXPORT_TDK i3TDKDlgTextureTool : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlgTextureTool)

public:
	i3TDKDlgTextureTool(CWnd* pParent = nullptr) : CDialog(i3TDKDlgTextureTool::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlgTextureTool() {}

// Dialog Data
	enum { IDD = IDD_TDK_DLG_TEXTOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	i3Node *		m_pRoot = nullptr;

	CListCtrl m_TexListCtrl;
	CStatic m_TotalCountCtrl;
	CEdit m_ED_WorkDir;
	CStatic m_ST_ImageViewPH;

	i3TDKImageViewCtrl	m_ImageCtrl;

	INT32 m_nIndex = 0;
	i3::vector<i3Texture*> m_TexIDList;
	i3::vector<i3Texture*> m_TexList;
	i3::vector<i3Texture*> m_NewTexList;

	char m_szPrefix[MAX_PATH] = { 0 };
	TCHAR	m_szExt[MAX_PATH] = { 0 };

protected:

	INT32		getSelectedItem(void);
	void		ReplaceCtrl( INT32 cx, INT32 cy);

	void		SaveTex( INT32 idx);

public:
	void SetFileNamePrefix( const char *szName ){ strncpy( m_szPrefix, szName, sizeof( m_szPrefix) -1); }

	i3Node *	getRoot(void)					{ return m_pRoot; }

	BOOL		GetTexFileName( TCHAR* szOut, const INT32 nOutMax, i3Texture *pTex, INT32 &nIndex, GET_TEX_NAME_MODE nMode, BOOL bForConvert = TRUE );
	int			GetTexIndex( i3Texture *pTex );

	void		AddListItemCB( i3Texture *pTex );
	i3Texture*	ReloadTexturesCB( i3Texture *pTex );
	void		MakeTexList();

	bool		Execute( i3Node * pNode);

	static void TraceSceneTextures( void *pUserData, TRACE_TEX_CB CBFunc );

	virtual BOOL OnInitDialog();	
	afx_msg void OnBnClickedTdkBtnSelall();
	afx_msg void OnBnClickedTdkBtnUnselall();
	afx_msg void OnBnClickedTdkBtnSeldiff();
	afx_msg void OnBnClickedTdkBtnSelnormal();
	afx_msg void OnBnClickedTdkBtnSelspec();
	afx_msg void OnBnClickedTdkBtnSellux();
	afx_msg void OnBnClickedTdkBtnWorkdir();
	afx_msg void OnBnClickedTdkBtnSavetex();
	afx_msg void OnBnClickedTdkBtnRestoretex();
	afx_msg void OnBnClickedTdkBtnRstrtexSearch();
	afx_msg void OnBnClickedTdkBtnExternal();
	afx_msg void OnBnClickedTdkBtnInternal();
	afx_msg void OnNMClickTdkTexList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
