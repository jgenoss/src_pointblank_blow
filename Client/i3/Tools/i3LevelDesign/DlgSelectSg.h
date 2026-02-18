#pragma once
#include "afxcmn.h"


// CDlgSelectSg dialog

class CDlgSelectSg : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectSg)

protected:
	i3LevelResSceneGraph	*		m_pRes;

public:
	CDlgSelectSg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectSg();

	bool					Execute(void);

	i3LevelResSceneGraph *	getSelectedRes(void)			{ return m_pRes; }

// Dialog Data
	enum { IDD = IDD_DLG_SELECTSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_SgListCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
