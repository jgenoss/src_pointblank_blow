#pragma once
#include "afxwin.h"


// CProp_LvElm_CameraObj dialog

class CProp_LvElm_CameraObj : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvElm_CameraObj)

protected:
	i3Camera * m_pCamera;

public:
	CProp_LvElm_CameraObj(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvElm_CameraObj();

// Dialog Data
	enum { IDD = IDD_PROP_CAMERA };

	virtual void	SetObject( i3ElementBase * pObj) override;

	void			UpdateInfo(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edFOV;
	CEdit m_edNear;
	CEdit m_edFar;
	CEdit m_edWidth;
	CEdit m_edHeight;
	CComboBox m_cbMode;
	CButton m_chkEnable;

protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnEnChangeEdCameraFov();
	afx_msg void OnEnChangeEdCameraNear();
	afx_msg void OnEnChangeEdCameraFar();
	afx_msg void OnEnChangeEdCameraWidth();
	afx_msg void OnEnChangeEdCameraHeight();
	afx_msg void OnCbnSelchangeCbCameraMode();
	afx_msg void OnBnClickedChkCameraEnable();
	CEdit m_edUpdateInterval;
	afx_msg void OnEnChangeEdCameraUpdateinterval();
	afx_msg void OnCbnSelchangeCbCameraRtWidth();
	CComboBox m_cbCamRT_Width;
	CComboBox m_cbCamRT_Height;
	CComboBox m_cbCamRT_Color_Format;
	CComboBox m_cbCamRT_Depth_Format;
	afx_msg void OnCbnSelchangeCbCameraRtHeight();
	afx_msg void OnCbnSelchangeCbCameraRtColorFormat();
	afx_msg void OnCbnSelchangeCbCameraRtDepthFormat();
};
