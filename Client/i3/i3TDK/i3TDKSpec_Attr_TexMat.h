#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_TexMat dialog

class i3TDKSpec_Attr_TexMat : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_TexMat)
protected:
	i3TextureMatrixAttr*		m_pAttr = nullptr;	

public:
	i3TDKSpec_Attr_TexMat(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_TexMat::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_TexMat();

	virtual void			SetObject( i3ElementBase* pObj);

	// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_TEXMAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic		m_st_UB;
	CStatic		m_st_VB;
	CComboBox	m_cb_UB;
	CComboBox	m_cb_VB;
	CEdit		m_ed_UB1;
	CEdit		m_ed_UB2;
	CEdit		m_ed_VB1;
	CEdit		m_ed_VB2;

	CEdit		m_ed_Projective;
	CEdit		m_ed_ElementCount;
	CComboBox	m_cb_BindType;

	void		AddBindType();
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg void	OnCbnSelchangeTdkCbUB( void);
	afx_msg void	OnCbnSelchangeTdkCbVB( void);
	afx_msg	void	OnEnKillfocusTdkEdUB1( void);
	afx_msg	void	OnEnKillfocusTdkEdUB2( void);
	afx_msg	void	OnEnKillfocusTdkEdVB1( void);
	afx_msg	void	OnEnKillfocusTdkEdVB2( void);
	afx_msg void	OnEnChangeTdkEdVB2( void);
};
