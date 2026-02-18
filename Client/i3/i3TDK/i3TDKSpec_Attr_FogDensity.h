#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_FogDensity dialog

class i3TDKSpec_Attr_FogDensity : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_FogDensity)
protected:
	i3FogDensityAttr *		m_pAttr = nullptr;

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_FogDensity(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_FogDensity::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_FogDensity();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_FOGDENSITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Density;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnEnKillfocusTdkEdDensity();
};
