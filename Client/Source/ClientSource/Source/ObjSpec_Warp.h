#pragma once

#if defined(USE_EDITDLG)//CObjSpec_Warp
#include "afxcmn.h"
#include "i3Framework.h"
#include "resource.h"
#include "i3Framework/i3GameObjPropertyDialog.h"

// CObjSpec_Warp dialog

class CObjSpec_Warp : public i3GameObjPropertyDialog
{
	DECLARE_DYNAMIC(CObjSpec_Warp)

protected:
	CEdit			m_ED_X;
	CEdit			m_ED_Y;
	CEdit			m_ED_Z;

public:
	CObjSpec_Warp(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CObjSpec_Warp();

// Dialog Data
	enum { IDD = IDD_PB_SPEC_WARP };

	virtual void	SetObject( i3ElementBase * pObj) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnKillfocusPbEdWarpx();
	afx_msg void OnEnKillfocusPbEdWarpy();
	afx_msg void OnEnKillfocusPbEdWarpz();
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
};

#endif