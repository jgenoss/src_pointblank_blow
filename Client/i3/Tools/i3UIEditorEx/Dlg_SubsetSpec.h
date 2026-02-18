#pragma once
#include "afxwin.h"
#include "resource.h"

// CDlg_SubsetSpec dialog

class CDlg_SubsetSpec : public CDialog
{
	DECLARE_DYNAMIC(CDlg_SubsetSpec)
protected:
	i3VTSubsetRule*		m_pRule;

public:
	bool		Execute( i3VTSubsetRule * pRule);

public:
	CDlg_SubsetSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_SubsetSpec();

// Dialog Data
	enum { IDD = IDD_DLG_SUBSET_SPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Path;
	CEdit m_ED_Name;
	CEdit m_ED_Size;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
