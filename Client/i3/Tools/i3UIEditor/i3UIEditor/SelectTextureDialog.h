#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSelectTextureDialog dialog

class CSelectTextureDialog : public CDialog
{
	DECLARE_DYNAMIC(CSelectTextureDialog)

public:
	CSelectTextureDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectTextureDialog();

// Dialog Data
	enum { IDD = IDD_SELECT_TEXTURE };

public:
	void	SetTextureList( i3List * pList)	{	m_pTextureList = pList;}

	void	_SetTextureList( i3List * pList);

	void	AddItem( i3Texture * pTexture );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	i3List			*	m_pTextureList;
	i3Texture		*	m_pSelectedTexture;

	CListCtrl			m_CtrlTexList;
	i3TDKImageViewCtrl	m_PrevCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_CtrlOK;
	CButton m_CtrlCancel;
	afx_msg void OnNMClickListSeltex(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListSeltex(NMHDR *pNMHDR, LRESULT *pResult);
};
