#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_ShadeMode dialog

class i3TDKSpec_Attr_ShadeMode : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_ShadeMode)
protected:
	i3ShadeModeAttr *		m_pAttr = nullptr;

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_ShadeMode(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_ShadeMode::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_ShadeMode();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_SHADEMODE };

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
	CComboBox m_CB_Mode;
	afx_msg void OnCbnSelchangeTdkCbMode();
};
