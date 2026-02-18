#pragma once
#include "afxwin.h"

#include "CTimeEventCtrl.h"
// CProp_LvElm_Primitive dialog

class CProp_LvElm_Primitive : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvElm_Primitive)

protected:
	i3LevelPrimitive	*	m_pPrim;

	CTimeEventCtrl		m_TimeEventCtrl;

public:
	virtual void	SetObject( i3ElementBase * pObj);

public:
	CProp_LvElm_Primitive(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvElm_Primitive();

// Dialog Data
	enum { IDD = IDD_PROP_LVELM_PRIM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
//	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	i3TDKColorCtrl	m_ColorCtrl;
	CButton m_VisibleCtrl;
	CButton m_TriggerCtrl;
	CEdit m_TextureCtrl;
	CEdit m_UTileCtrl;
	CEdit m_VTileCtrl;
	afx_msg void OnBnClickedBtnTexture();
	CStatic m_ST_ColorPreviewPH;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnBnClickedChkTrigger();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedChkVisible();
	CComboBox m_CB_TriggerCGroup;
//	afx_msg void OnCbnKillfocusCbTriggerCgroup();
	afx_msg void OnCbnSelchangeCbTriggerCgroup();
};
