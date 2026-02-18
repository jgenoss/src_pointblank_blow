#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CDlg_LoadError dialog

class CDlg_LoadError : public CDialog
{
	DECLARE_DYNAMIC(CDlg_LoadError)

public:
	CDlg_LoadError(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_LoadError();

// Dialog Data
	enum { IDD = IDD_LOADERROR };

protected:
	i3::vector< i3LevelElement3D *>	m_ElmList;

protected:
	void	UpdateElement( INT32 idx, i3LevelElement3D * pElm);
	
public:
	bool	Execute( CWnd * pParent, const i3::vector<i3LevelElement3D *> & list);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	CListCtrl m_ElmListCtrl;	
};
