#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_Geometry dialog

class i3TDKSpec_Attr_Geometry : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_Geometry)

protected:
	i3GeometryAttr *		m_pAttr = nullptr;

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_Geometry(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_Geometry::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_Geometry();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_GEOMETRY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CB_PrimType;
	CEdit m_ED_PrimCount;
	CButton m_CHK_Pos;
	CButton m_CHK_Normal;
	CButton m_CHK_Tangent;
	CButton m_CHK_Binormal;
	CButton m_CHK_Color;
	CButton m_CHK_Transformed;
	CEdit m_ED_TexSet;
	CEdit m_ED_TexCoord;
	CEdit m_ED_BlendIndex;
	CEdit m_ED_BlendWeight;
	CEdit m_ED_IndexBuffer;
	CEdit m_ED_StartIndex;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnCbnSelchangeTdkCbPrimtype();
	afx_msg void OnEnKillfocusTdkEdPrimcount();
	afx_msg void OnEnKillfocusTdkEdStartidx();
};
