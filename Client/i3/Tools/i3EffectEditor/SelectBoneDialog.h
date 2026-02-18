#pragma once
#include "afxcmn.h"


// CSelectBoneDialog dialog

class CSelectBoneDialog : public CDialog
{
	DECLARE_DYNAMIC(CSelectBoneDialog)

protected:
	i3Transform *		m_pBone;
	i3Node *			m_pRoot;

protected:
	void	AssignValue(void);

public:
	BOOL	Execute( i3Node * pRoot);

	i3Transform *	GetSelectedBone(void)		{ return m_pBone; }
	BOOL			IsAttach(void)				{ return m_bAttach; }

public:
	CSelectBoneDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectBoneDialog();

// Dialog Data
	enum { IDD = IDD_SELECTBONEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_BoneListCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	BOOL m_bAttach;
};
