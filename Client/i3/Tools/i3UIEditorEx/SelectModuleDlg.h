#pragma once
#include "afxwin.h"
#include "resource.h"


// CSelectModuleDlg dialog

class CSelectModuleDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectModuleDlg)

public:
	CSelectModuleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectModuleDlg();

// Dialog Data
	enum { IDD = IDD_DLG_SELECT_RUNMODULE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_chkRunEnable;
	CEdit m_edModuleName;
	afx_msg void OnBnClickedChkModulerunEnable();
	afx_msg void OnEnKillfocusEdModulename();
protected:
	virtual void OnCancel() override;
	virtual void OnOK() override;

	bool	m_bRunModule;
	char	m_szModuleName[MAX_PATH];
	void	_UpdateInfo(void);
public:
	virtual BOOL OnInitDialog() override;

	void	setModuleRunEnable( bool bEnable)	{	m_bRunModule = bEnable;}
	void	setModuleName( const char * pszmoduleName)	{	i3::string_ncopy_nullpad( m_szModuleName, pszmoduleName, MAX_PATH -1);}
};
