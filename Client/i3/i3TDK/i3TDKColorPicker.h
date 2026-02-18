#pragma once
#include "i3TDKRGBPickCtrl.h"
#include "i3TDKPrimaryColorPickCtrl.h"
#include "i3TDKAlphaPickCtrl.h"
#include "i3TDKColorPreviewCtrl.h"
#include "afxwin.h"

// i3TDKColorPicker dialog

class I3_EXPORT_TDK i3TDKColorPicker : public CDialog
{
	DECLARE_DYNAMIC(i3TDKColorPicker)
protected:
	COLORREAL					m_OrgColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	COLORREAL					m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	COLORREAL					m_BaseColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	i3TDKRGBPickCtrl			m_RGBPickCtrl;
	i3TDKPrimaryColorPickCtrl	m_PrimaryColorPickCtrl;
	i3TDKAlphaPickCtrl			m_AlphaPickCtrl;
	i3TDKColorPreviewCtrl		m_ColorPreviewCtrl;

protected:
	void		_FindBaseColor(void);
	void		_SelectColor( CWnd * pFromWnd, COLORREAL * pColor);
	void		_SelectAlpha( CWnd * pFromWnd, COLORREAL * pColor);

public:
	i3TDKColorPicker(CWnd* pParent = nullptr) : CDialog(IDD_DLG_COLORPICKER, pParent) {}   // standard constructor
	virtual ~i3TDKColorPicker() {}

	enum { IDD = IDD_DLG_COLORPICKER };

	bool		Execute( COLORREAL * pColor);

	COLORREAL *	getSelectedColor(void)					{ return &m_Color;						}
	void		setSelectedColor( COLORREAL * pColor)	{ m_RGBPickCtrl.setColor( pColor);		}

// Dialog Data

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CEdit m_RCtrl;
	CEdit m_R2Ctrl;
	CEdit m_GCtrl;
	CEdit m_G2Ctrl;
	CEdit m_BCtrl;
	CEdit m_B2Ctrl;
	CEdit m_ACtrl;
	CEdit m_A2Ctrl;
protected:
	virtual void OnCancel();
public:
	afx_msg void OnEnKillfocusTdkEdR2();
	afx_msg void OnEnKillfocusTdkEdG2();
	afx_msg void OnEnKillfocusTdkEdB2();
	afx_msg void OnEnKillfocusTdkEdA2();
	afx_msg void OnEnKillfocusTdkEdR();
	afx_msg void OnEnKillfocusTdkEdG();
	afx_msg void OnEnKillfocusTdkEdB();
	afx_msg void OnEnKillfocusTdkEdA();
};
