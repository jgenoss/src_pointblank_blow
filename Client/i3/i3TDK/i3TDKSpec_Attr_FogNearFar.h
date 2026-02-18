#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_FogNearFar dialog

class i3TDKSpec_Attr_FogNearFar : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_FogNearFar)

protected:
	i3FogNearFarAttr *		m_pAttr = nullptr;

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_FogNearFar(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_FogNearFar::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_FogNearFar();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_FOGNEARFAR };

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
	afx_msg void OnEnKillfocusTdkEdNear();
	afx_msg void OnEnKillfocusTdkEdFar();
	CEdit m_ED_Near;
	CEdit m_ED_Far;
};
