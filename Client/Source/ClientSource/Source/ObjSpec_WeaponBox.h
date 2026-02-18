#pragma once
#if defined(USE_EDITDLG)//CObjSpec_WeaponBox
#include "afxcmn.h"
#include "i3Framework.h"
#include "resource.h"
#include "i3Framework/i3GameObjPropertyDialog.h"
// CObjSpec_Warp dialog

class CObjSpec_WeaponBox : public i3GameObjPropertyDialog
{
	DECLARE_DYNAMIC(CObjSpec_WeaponBox)

protected:
	CEdit			m_ED_RespawnTime;
	CEdit			m_ED_GroupIdx;
	CComboBox		m_CB_WeaponType;
public:
	CObjSpec_WeaponBox(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CObjSpec_WeaponBox();

// Dialog Data
	enum { IDD = IDD_PB_SPEC_RPG7 };

	virtual void	SetObject( i3ElementBase * pObj) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnEnKillfocusPbEdRespawnTime();
	afx_msg void	OnEnKillfocusPbEdRPG7GroupIdx();
	afx_msg void	OnCbnSelchangeWeaponType();
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
};

#endif