#pragma once
#include "afxcmn.h"
#include "i3TDKOptSpecBase.h"

// i3TDKOptSpecConvertDXTTexture dialog

class I3_EXPORT_TDK i3TDKOptSpecConvertDXTTexture : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecConvertDXTTexture)

public:
	i3TDKOptSpecConvertDXTTexture(CWnd* pParent = nullptr) : i3TDKOptSpecBase(IDD_OPTSPEC_CONVERTDXTTEXTURE, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecConvertDXTTexture() {}

	enum { IDD = IDD_OPTSPEC_CONVERTDXTTEXTURE };

protected:
	CComboBox		m_TargetFormatCtrl;

	CStatic			m_StQualityCtrl;
	CComboBox		m_QualityCtrl;

	CButton			m_BinaryAlphaCtrl;

	CStatic			m_StAlphaRefCtrl;
	CEdit			m_AlphaRefCtrl;

	CButton			m_DitherCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void		SetTargetFormat( I3G_IMAGE_FORMAT fmt);

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
