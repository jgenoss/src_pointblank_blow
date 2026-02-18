#pragma once
#include "afxwin.h"


// i3TDKOptSpecMacroTexture dialog

class I3_EXPORT_TDK i3TDKOptSpecMacroTexture : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecMacroTexture)

public:
	i3TDKOptSpecMacroTexture(CWnd* pParent = nullptr) : i3TDKOptSpecBase(i3TDKOptSpecMacroTexture::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecMacroTexture() {}

// Dialog Data
	enum { IDD = IDD_OPTSPEC_MACROTEXTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CEdit m_ED_Width;
	CEdit m_ED_Height;
	CButton m_CHK_Dump;
};
