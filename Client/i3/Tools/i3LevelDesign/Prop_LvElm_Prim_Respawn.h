#pragma once
#include "afxwin.h"

// CProp_LvElm_Prim_Respawn dialog

class CProp_LvElm_Prim_Respawn : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvElm_Prim_Respawn)
protected:
	i3LevelRespawn	*	m_pRespawn;

public:
	virtual void	SetObject( i3ElementBase * pObj);

public:
	CProp_LvElm_Prim_Respawn(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvElm_Prim_Respawn();

// Dialog Data
	enum { IDD = IDD_PROP_LVELM_PRIM_RESPAWN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_TeamCtrl;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnCbnSelchangeCbTeam();
};
