#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "Avatar.h"

class CDlg_SelectBody : public CDialog
{
	DECLARE_DYNAMIC(CDlg_SelectBody)
protected:
	CComboBox	m_CmbBodyList;
	CListCtrl	m_ExcludeList;
	CListCtrl	m_IncludeList;

	mapParts	m_ExcludePartsList;
	mapParts	m_IncludePartsList;

public:
	CDlg_SelectBody(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_SelectBody();

	enum { IDD = IDD_DLG_SELECTBODY };

	virtual BOOL	OnInitDialog();
	void		GetPartsList( mapParts & list) { list = m_IncludePartsList; m_IncludePartsList.clear();}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void	OnCbSelectSampleBody( void);

	virtual void	OnDestroy( void);

	virtual void	OnOK( void);

	void	CreateBodyList( void);
	void	UpdateList( void);

	void	ExcludeBySexType( INT32 type);

public:
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnRemove();
};