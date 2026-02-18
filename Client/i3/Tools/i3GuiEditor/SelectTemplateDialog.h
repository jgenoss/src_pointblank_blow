#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSelectTemplateDialog dialog

class CSelectTemplateDialog : public CDialog
{
	DECLARE_DYNAMIC(CSelectTemplateDialog)

public:
	CSelectTemplateDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectTemplateDialog();

// Dialog Data
	enum { IDD = IDD_SELECT_TEMPLATE };

	i3GuiTemplateElement *	m_pSelectedTemplate;
	i3GuiControlTemplate *	m_pTemplate;
	void	SetTemplate( i3GuiControlTemplate * pTemplate) {	m_pTemplate = pTemplate;}

protected:
	RECT	m_RectDrawRect;	//TemplateElement를 draw하기위한 영역
	void	_OnDrawTemplate(HDC hdc);

	void	_SetTemplateList();
	void	_AddItem( i3GuiTemplateElement * pElement );
	BOOL	_IsValidTemplateElement( i3GuiTemplateElement * pElement);
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	virtual BOOL OnInitDialog() override;
	CListCtrl m_CtrlTemplateListView;
	CButton m_CtrlOK;
	CButton m_CtrlCancel;
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClickListSeltemplate(NMHDR *pNMHDR, LRESULT *pResult);
};
