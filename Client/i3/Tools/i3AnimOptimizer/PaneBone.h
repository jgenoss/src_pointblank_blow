#pragma once
#include "afxcmn.h"


// CPaneBone dialog

class CPaneBone : public CDialog
{
	DECLARE_DYNAMIC(CPaneBone)

protected:
	CImageList * m_pImageList;
	i3Node *	m_pRoot;

protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void	SetSceneGraph( i3Node * pRoot);
	void	Rec_SetTree( HTREEITEM hItem, i3Node * pNode);

	void	GetCheckedItem( i3List * pList);
	BOOL	IsValidItem( HTREEITEM hItem);

	void	OnExpand( HTREEITEM hItem);
	void	OnCollapse( HTREEITEM hItem);
	void	OnSelectAll(void);
	void	OnUnselectAll(void);
	void	OnReverseSelection(void);
	void	OnSelectDescendant( HTREEITEM hItem);
	void	OnUnselectDescendant( HTREEITEM hItem);
	void	OnSelectByType(void);
	void	OnDelete( HTREEITEM hItem);
	void	OnCollapseNode( HTREEITEM hItem);
	void	OnCollapseTransform( HTREEITEM hItem);
	void	OnRemoveNodeName(void);
	void	OnRemoveBoundBox(void);
	void	OnFindBoneUsage( BOOL bUsed);

public:
	CPaneBone(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneBone();

// Dialog Data
	enum { IDD = IDD_PANE_BONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_BoneTreeCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnNMRclickBoneTree(NMHDR *pNMHDR, LRESULT *pResult);
};
