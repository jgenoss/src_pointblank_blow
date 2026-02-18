#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_BlendMode dialog

class i3TDKSpec_Attr_BlendMode : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_BlendMode)
protected:
	i3BlendModeAttr *		m_pAttr = nullptr;

protected:

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_BlendMode(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_BlendMode::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_BlendMode();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_BLENDMODE };

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
	afx_msg void OnCbnSelchangeTdkCbDest();
	afx_msg void OnCbnSelchangeTdkCbSrc();
	afx_msg void OnCbnSelchangeTdkCbOp();
	CComboBox m_CB_Src;
	CComboBox m_CB_Dest;
	CComboBox m_CB_OP;
};
