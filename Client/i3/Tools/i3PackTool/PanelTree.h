#pragma once
#include "afxcmn.h"


// CPanelTree dialog

class CPanelTree : public CDialog
{
	DECLARE_DYNAMIC(CPanelTree)

protected:
	INT32			m_idxIconNormal;
	INT32			m_idxIconSelect;

protected:
	void			_Rec_SetTree( i3PackNode * pNode, HTREEITEM hParent = TVI_ROOT);

	void			AddDirectory( const char * pszPath, i3PackNode * pNode, BOOL IncludeSubDir);
	i3PackNode *	PreparePath( const char * pszPath, bool bIsFile = false);
	bool			IsRelativePath( const char * pszPath);
	BOOL			_BuildFileList( const char * pszPath, i3PackNode * pParent);
	void			OnSelect( I3_TDK_UPDATE_INFO * pInfo);
	void			OnDelete(void);
	

	void			_Rec_Export( i3FileStream * pFile, i3PackNode * pNode, const char * pszPath);

public:
	void			UpdateAll(void);
	void			Add( const char * pszPath, BOOL IncludeSubDir);
	void			Remove( const char * pszPath, const BOOL includeSubDirOK);
	void			OnExport(void);
public:
	CPanelTree(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelTree();

// Dialog Data
	enum { IDD = IDD_PANEL_TREE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNMClickPackTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickPackTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CTreeCtrl m_TreeCtrl;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnTvnSelchangedPackTree(NMHDR *pNMHDR, LRESULT *pResult);
};
