#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_TexWrap dialog

class i3TDKSpec_Attr_TexWrap : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_TexWrap)
protected:
	i3TextureWrapAttr *		m_pAttr = nullptr;

public:
	virtual void		SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_TexWrap(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_TexWrap::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_TexWrap();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_TEXWRAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnCbnSelchangeTdkCbHwrap();
	afx_msg void OnCbnSelchangeTdkCbVwrap();
	CComboBox m_CB_HWrap;
	CComboBox m_CB_VWrap;
};
