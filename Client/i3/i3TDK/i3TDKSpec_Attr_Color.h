#pragma once
#include "afxwin.h"
#include "i3TDKColorCtrl.h"

// i3TDKSpec_Attr_Color dialog

class i3TDKSpec_Attr_Color : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_Color)
protected:
	i3ColorAttr *		m_pAttr = nullptr;
	i3TDKColorCtrl		m_ColorCtrl;

public:
	virtual void		SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_Color(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_Color::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_Color();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_COLOR };

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
	CStatic m_ST_ColorPH;
};
