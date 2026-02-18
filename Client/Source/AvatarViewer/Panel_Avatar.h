#pragma once

#include "Panel_Dlg_Avatar.h"

// CPanel_Avatar

class CPanel_Avatar : public i3TDKRibbonDockBase
{
	DECLARE_DYNAMIC(CPanel_Avatar)
protected:
	CPanel_Dlg_Avatar	m_Dlg;

public:
	CPanel_Avatar();
	virtual ~CPanel_Avatar();

	void		UpdateAll(void);
	void		CreatePartsList( void);

	CPanel_Dlg_Avatar *	getAvatarDlg( void)		{ return &m_Dlg; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


