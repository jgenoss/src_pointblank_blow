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

	i3UIControl	 * m_pControl;
	i3UIImage	 * m_pShape;

	void	SetCommonInfo(i3UIControl * pObj);
	void	SetShapeInfo( i3UIImage * pImg);
	void	UpdateInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_CtrlPosX;
	CEdit m_CtrlPosY;
	CEdit m_CtrlWidth;
	CEdit m_CtrlHeight;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnEnChangePropCommonPosx();
	afx_msg void OnEnChangePropCommonName();
	CEdit m_Control_ID;
};
