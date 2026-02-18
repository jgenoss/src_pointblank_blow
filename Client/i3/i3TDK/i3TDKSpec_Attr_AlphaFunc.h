#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_AlphaFunc dialog

class i3TDKSpec_Attr_AlphaFunc : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_AlphaFunc)
protected:
	i3AlphaFuncAttr *		m_pAttr = nullptr;

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_AlphaFunc(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_AlphaFunc::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_AlphaFunc();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ALPHAFUNC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Ref;
	CComboBox m_CB_Func;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnCbnSelchangeTdkCbFunc();
	afx_msg void OnEnKillfocusTdkEdRef();
};
