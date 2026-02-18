#pragma once
#include "afxwin.h"


// CPropCommon dialog

class CPropCommon : public CDialog
{
	DECLARE_DYNAMIC(CPropCommon)

public:
	CPropCommon(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropCommon();

// Dialog Data
	enum { IDD = IDD_PROPERTY_COMMON };

	i3GuiObjBase * m_pObjBase;
	i3GuiImage	 * m_pShape;

	void	SetCommonInfo(i3GuiObjBase * pObj);
	void	SetShapeInfo( i3GuiImage * pImg);
	void	UpdateInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_CtrlPosX;
	CEdit m_CtrlPosY;
	CEdit m_CtrlWidth;
	CEdit m_CtrlHeight;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnEnChangePropCommonPosx();
	afx_msg void OnEnChangePropCommonName();
	CEdit m_Control_ID;
};
