#pragma once
#if defined(USE_EDITDLG)//CObjSpec_JumpHold
#include "afxcmn.h"
#include "i3Framework.h"
#include "resource.h"
#include "i3Framework/i3GameObjPropertyDialog.h"
// CObjSpec_JumpHold dialog

class CObjSpec_JumpHold : public i3GameObjPropertyDialog
{
	DECLARE_DYNAMIC(CObjSpec_JumpHold)

protected:
	CEdit			m_ED_JumpPower;
	CEdit			m_ED_JumpHeight;
	CEdit			m_ED_JumpDig;
	CEdit			m_ED_JumpAnimSpeed;
public:
	CObjSpec_JumpHold(CWnd* pParent = NULL);   // standard constructor
	virtual ~CObjSpec_JumpHold();

	// Dialog Data
	enum { IDD = IDD_PB_SPEC_JUMPHOLD };

	virtual void	SetObject(i3ElementBase * pObj) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnEnKillfocusPbEdJumpPower();
	afx_msg void	OnEnKillfocusPbEdJumpHeight();
	afx_msg void	OnEnKillfocusPbEdJumpDig();
	afx_msg void	OnEnKillfocusPbEdJumpAnimSpeed();
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
};

#endif