#pragma once

//#include "afxcmn.h"
//#include "afxcview.h"
#include "mmc.h"

#include "RSCNode.h"
#include <set>

// CPaneTree dialog

class CPaneTree : public CDialog
{
	DECLARE_DYNAMIC(CPaneTree)

public:
	typedef std::set< std::string > FileNameList;

protected:
	IImageList *	m_pIImageList;
	CImageList *	m_pImageList;
	INT32			m_nIndexOfFirstTDKImage;	//TDK res에서 add된 icon의 첫번째 index

	// 2011-05-06 김재진
	FileNameList	m_FileList;

public:
	void			SetTreeInfo( TVITEM * pInfo, RSCNode * pNode);
	void			UpdateTree( HTREEITEM hItem, RSCNode * pNode);
	HTREEITEM		AddTree( HTREEITEM hParent, RSCNode * pNode);
	HTREEITEM		GetRootItem() const;

	void			Rec_AddTree( HTREEITEM hParent, RSCNode * pNode);
	void			UpdateAll( void);

	void			OnDelete( HTREEITEM hItem);
	// 2011-05-06 김재진
	void			FileListLink(FileNameList* fl);
	void			ChildDelete(HTREEITEM hItem);

	FileNameList& GetFileNameList() {	return m_FileList;	}
	///////////////////

public:
	CPaneTree(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneTree();

	// Dialog Data
	enum { IDD = IDD_PANE_TREE };

	//
	CTreeCtrl m_TreeCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnGetdispinfoTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnCancel() override;
	virtual void OnOK() override;
public:
	afx_msg void OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};
