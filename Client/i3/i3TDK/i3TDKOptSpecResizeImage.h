#pragma once
#include "afxcmn.h"
#include "i3TDKOptSpecBase.h"

// i3TDKOptSpecResizeImage dialog

class I3_EXPORT_TDK i3TDKOptSpecResizeImage : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecResizeImage)

public:
	i3TDKOptSpecResizeImage(CWnd* pParent = nullptr) : i3TDKOptSpecBase(IDD_OPTSPEC_RESIZEIMAGE, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecResizeImage() {}

	enum { IDD = IDD_OPTSPEC_RESIZEIMAGE };

protected:
	CComboBox		m_TypeCtrl;
	CStatic			m_StScaleCtrl;
	CEdit			m_ScaleFactorCtrl;
	CStatic			m_StLimitCtrl;
	CStatic			m_StMaxCtrl;
	CStatic			m_StMinCtrl;
	CStatic			m_StWCtrl;
	CStatic			m_StHCtrl;
	CEdit			m_MaxWCtrl;
	CEdit			m_MaxHCtrl;
	CEdit			m_MinWCtrl;
	CEdit			m_MinHCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void		SetType( i3OptResizeTexture::TYPE type);

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
