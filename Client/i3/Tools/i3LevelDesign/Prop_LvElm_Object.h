#pragma once
#include "afxwin.h"


// Prop_LvElm_Object dialog

class Prop_LvElm_Object : public i3TDKDialogBase	
{
	DECLARE_DYNAMIC(Prop_LvElm_Object)

protected:
	i3LevelObject	*	m_pObj;

public:
	Prop_LvElm_Object(CWnd* pParent = NULL);   // standard constructor
	virtual ~Prop_LvElm_Object();

	virtual void	SetObject( i3ElementBase * pObj) override;

// Dialog Data
	enum { IDD = IDD_PROP_OBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cb_ObjType;
	CComboBox m_cb_TeamType;
	CButton m_btnInvisible;
	CEdit m_ed_JumpPosX;
	CEdit m_ed_JumpPosY;
	CEdit m_ed_JumpPosZ;

protected:
	void _UpdateElementState();

public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnCbnSelchangeCbObjecttype();
	afx_msg void OnCbnSelchangeCbObjteamType();
	afx_msg void OnBnClickedChkInvisible();
	
	afx_msg void OnEnChangeEditJumpposx();
	afx_msg void OnEnChangeEditJumpposy();
	afx_msg void OnEnChangeEditJumpposz();
	virtual void OnOK() override;
};
