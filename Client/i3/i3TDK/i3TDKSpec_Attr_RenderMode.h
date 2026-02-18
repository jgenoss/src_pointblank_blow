#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_RenderMode dialog

class i3TDKSpec_Attr_RenderMode : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_RenderMode)
protected:
	i3RenderModeAttr *		m_pAttr = nullptr;

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_RenderMode(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_RenderMode::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_RenderMode();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_RENDERMODE };

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
	afx_msg void OnCbnSelchangeTdkCbRendermode();
	CComboBox m_CB_Mode;
};
