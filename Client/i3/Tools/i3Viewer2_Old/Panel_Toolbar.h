#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CPanel_Toolbar dialog

class CPanel_Toolbar : public CDialog
{
	DECLARE_DYNAMIC(CPanel_Toolbar)

protected:

public:
	CPanel_Toolbar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanel_Toolbar();

// Dialog Data
	enum { IDD = IDD_PANEL_TOOLBAR };

protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void	_Update();

	BOOL	OnUpdateToolTip( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );


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
	CButton m_BTN_ModeNormal;
	CButton m_BTN_ModeMesh;
	CButton m_BTN_ModeColl;
	CButton m_BTN_ModeWire;
	CButton m_BTN_ShadowEnable;
	CButton m_BTN_Grid;
	afx_msg void OnBnClickedBtnGrid();
	afx_msg void OnBnClickedBtnShadowenable();
	afx_msg void OnBnClickedBtnNormalview();
	afx_msg void OnBnClickedBtnMeshview();
	afx_msg void OnBnClickedBtnCollview();
	afx_msg void OnBnClickedBtnWireview();
	CSliderCtrl m_SL_TimeScale;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBtnDynamic();
	afx_msg void OnBnClickedBtnKinematic();
	CButton m_BTN_ModeSelOnly;
	afx_msg void OnBnClickedBtnViewonlysel();
};
