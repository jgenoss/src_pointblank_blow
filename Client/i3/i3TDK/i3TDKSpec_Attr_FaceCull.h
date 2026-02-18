#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_FaceCull dialog

class i3TDKSpec_Attr_FaceCull : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_FaceCull)

protected:
	i3FaceCullModeAttr *		m_pAttr = nullptr;

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_FaceCull(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_FaceCull::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_FaceCull();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_FACECULL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CB_Cull;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnCbnSelchangeTdkCbCull();
};
