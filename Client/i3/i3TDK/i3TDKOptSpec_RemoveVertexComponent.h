#pragma once
#include "afxwin.h"


// i3TDKOptSpec_RemoveVertexComponent dialog

class i3TDKOptSpec_RemoveVertexComponent : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpec_RemoveVertexComponent)

public:
	i3TDKOptSpec_RemoveVertexComponent(CWnd* pParent = nullptr) : i3TDKOptSpecBase(i3TDKOptSpec_RemoveVertexComponent::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpec_RemoveVertexComponent() {}

// Dialog Data
	enum { IDD = IDD_OPTSPEC_REMOVEVERTEXCOMPONENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_Normal;
	CButton m_CHK_Color;
	CButton m_CHK_Tex;
	CButton m_CHK_Tangent;
	CButton m_CHK_AllTex;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
