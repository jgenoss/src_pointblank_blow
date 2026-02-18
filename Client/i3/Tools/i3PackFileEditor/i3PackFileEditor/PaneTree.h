#pragma once
#include "afxcmn.h"
#include "PackNode.h"

// CPaneTree dialog

class Ci3PackFileEditorView;

class CPaneTree : public CDialog
{
	DECLARE_DYNAMIC(CPaneTree)

protected:
	Ci3PackFileEditorView * m_pView;

protected:
	void OnAddNode( PackNode * pNode);
	void OnRemoveNode( PackNode * pNode);
	void OnRemoveAllNode(void);
	void Rec_UpdatePackFile( PackNode * pNode, HTREEITEM hParent);
	void OnUpdatePackFile(void);
	void Update( I3_TDK_UPDATE_INFO * pInfo);

public:
	CPaneTree(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneTree();

	void	SetView( Ci3PackFileEditorView * pView)
	{
		m_pView = pView;
	}

// Dialog Data
	enum { IDD = IDD_PANETREE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CTreeCtrl m_TreeCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult);
};
