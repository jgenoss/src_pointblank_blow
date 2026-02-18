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
	void	SetTextureList( const i3::shared_ptr< i3::vector<class i3GuiToolImageElement*> >&  pList)	{	m_pTextureList = pList;}

	void	_SetTextureList(const i3::shared_ptr<i3::vector<i3GuiToolImageElement*> >& pList);

	void	AddItem( i3Texture * pTexture );

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	i3::shared_ptr<i3::vector<i3GuiToolImageElement*> >	m_pTextureList;

	i3Texture		*	m_pSelectedTexture;

	CListCtrl			m_CtrlTexList;
	i3TDKImageViewCtrl	m_PrevCtrl;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_CtrlOK;
	CButton m_CtrlCancel;
	afx_msg void OnNMClickListSeltex(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListSeltex(NMHDR *pNMHDR, LRESULT *pResult);
};
