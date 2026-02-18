#pragma once
#include "afxwin.h"


// CPanelTransform dialog

class CPanelTransform : public CDialog
{
	DECLARE_DYNAMIC(CPanelTransform)

protected:
	VEC3D			m_Pos;
	VEC3D			m_Rot;
	VEC3D			m_Scale;

	i3::vector<i3LevelElement3D*> m_SelList;

public:
	void			Reset(void);
	void			Update(void);
	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	CPanelTransform(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelTransform();

// Dialog Data
	enum { IDD = IDD_PANEL_TRANSFORM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_XCtrl;
	CEdit m_YCtrl;
	CEdit m_ZCtrl;
	CEdit m_RotXCtrl;
	CEdit m_RotYCtrl;
	CEdit m_RotZCtrl;
	CEdit m_ScaleXCtrl;
	CEdit m_ScaleYCtrl;
	CEdit m_ScaleZCtrl;
	
	virtual BOOL OnInitDialog() override;

	void ClearSelList() { m_SelList.clear(); }
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};
