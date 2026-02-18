#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSingleSelectControlTypeDialog dialog

class CSingleSelectControlTypeDialog : public CDialog
{
	DECLARE_DYNAMIC(CSingleSelectControlTypeDialog)

public:
	CSingleSelectControlTypeDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSingleSelectControlTypeDialog();

	i3ClassMeta * m_pMeta;

// Dialog Data
	enum { IDD = IDD_SELECT_CONTROL };

	char	m_szSelectedMeta[256];
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void	setMeta( i3ClassMeta * pMeta)	{	m_pMeta = pMeta;}
	CComboBox m_TypeCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
