#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_FogMode dialog

class i3TDKSpec_Attr_FogMode : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_FogMode)
protected:
	i3FogModeAttr *		m_pAttr = nullptr;

public:
	virtual void		SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_FogMode(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_FogMode::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_FogMode();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_FOGMODE };

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
	afx_msg void OnCbnSelchangeTdkCbMode();
	CComboBox m_CB_Mode;
};
