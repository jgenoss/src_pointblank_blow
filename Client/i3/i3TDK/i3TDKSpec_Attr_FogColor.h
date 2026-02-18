#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_FogColor dialog

class i3TDKSpec_Attr_FogColor : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_FogColor)
protected:
	i3FogColorAttr *	m_pAttr = nullptr;
	i3TDKColorCtrl		m_ColorCtrl;

public:
	virtual void		SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_FogColor(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_FogColor::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_FogColor();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_FOGCOLOR };

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
