#pragma once
#include "afxcmn.h"
#include "afxcview.h"
#include "MVFNode.h"
#include "mmc.h"

// CPaneTree dialog

class CPaneTree : public CDialog
{
	DECLARE_DYNAMIC(CPaneTree)
protected:
	HANDLE			m_hModule;
	IImageList *	m_pIImageList;
	CImageList *	m_pImageList;

public:
	void		SetTreeInfo( TVITEM * pInfo, MVFNode * pNode);
	void		UpdateTree( HTREEITEM hItem, MVFNode * pNode);
	HTREEITEM	AddTree( HTREEITEM hParent, MVFNode * pNode);
	void		Rec_AddTree( HTREEITEM hParent, MVFNode * pNode);
	void		UpdateAll(void);

	void		OnDelete( HTREEITEM hItem);

public:
	CPaneTree(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneTree();

// Dialog Data
	enum { IDD = IDD_PANE_TREE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	CTreeCtrl m_TreeCtrl;
	afx_msg void OnTvnGetdispinfoTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult);
};
