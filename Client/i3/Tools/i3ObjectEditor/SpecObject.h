#pragma once
#include "afxwin.h"

#include "ObjectTemplate.h"

// CSpecObject dialog

class CSpecObject : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecObject)
protected:
	CObjectTemplate *	m_pObj;

public:
	virtual void	SetObject( i3ElementBase * pObj) override;

public:
	CSpecObject(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecObject();

// Dialog Data
	enum { IDD = IDD_SPEC_OBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_NameCtrl;
	CEdit m_HPCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnEnKillfocusEdName();
	afx_msg void OnEnKillfocusEdHp();
};
