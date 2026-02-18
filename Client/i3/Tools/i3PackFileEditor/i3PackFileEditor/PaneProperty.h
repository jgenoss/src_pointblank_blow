#pragma once
#include "resource.h"

// CPaneProperty dialog

class CPaneProperty : public CDialog
{
	DECLARE_DYNAMIC(CPaneProperty)
protected:
	CDialog *			m_pCurSpec;

public:
	CPaneProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneProperty();

	void	PopupSpec( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_PANEPROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
