#pragma once

#include "afxwin.h"


// i3TDKSpec_Transform dialog

typedef void (*TDK_CB_TRANSFORM_POS)(MATRIX *p1);

class i3TDKSpec_Transform : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Transform)

public:
	i3Transform *		m_pTrans = nullptr;

	TDK_CB_TRANSFORM_POS	m_pCB_setTransPos = nullptr;

	
public:
	i3TDKSpec_Transform(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Transform::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Transform();

	virtual void			SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_TRANSFORM };

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
	CEdit	m_PosX_Ctrl;
	CEdit	m_PosY_Ctrl;
	CEdit	m_PosZ_Ctrl;

	CEdit	m_RotX_Ctrl;
	CEdit	m_RotY_Ctrl;
	CEdit	m_RotZ_Ctrl;

	CEdit	m_ScaleX_Ctrl;
	CEdit	m_ScaleY_Ctrl;
	CEdit	m_ScaleZ_Ctrl;

	CEdit	m_ED_Anim;

	afx_msg void OnBnClickedBtnAnim();

	afx_msg void OnEnKillfocusTdkPosXCtrl();
	afx_msg void OnEnKillfocusTdkPosYCtrl();
	afx_msg void OnEnKillfocusTdkPosZCtrl();

	afx_msg void OnEnKillfocusTdkRotXCtrl();
	afx_msg void OnEnKillfocusTdkRotYCtrl();
	afx_msg void OnEnKillfocusTdkRotZCtrl();

	afx_msg void OnEnKillfocusTdkScaleXCtrl();
	afx_msg void OnEnKillfocusTdkScaleYCtrl();
	afx_msg void OnEnKillfocusTdkScaleZCtrl();

	afx_msg void SetModifiedTransform();
	void SetRotate(MATRIX * pMat, REAL32 x, REAL32 y, REAL32 z);
};

