#pragma once
#include "afxwin.h"


// CDlgWorldSectionInput dialog

class CDlgWorldSectionInput : public CDialog
{
	DECLARE_DYNAMIC(CDlgWorldSectionInput)

public:
	CDlgWorldSectionInput(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWorldSectionInput();

	// Dialog Data
	enum { IDD = IDD_DLG_WORLDSECTION_INPUT };

protected:
	INT32		m_ID;


public:
	bool		Execute(INT32 id, i3::pack::WORLD_SECTION_INFO * pInfo);

	INT32		getSelectedID(void) { return m_ID; }
	char *		getSelectedName(void) { return m_szName; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_IDCtrl;
	CEdit m_NameCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
private:
	char		m_szName[256];
	bool		m_bCanEditID;
};
