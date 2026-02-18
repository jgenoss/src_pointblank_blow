#pragma once
#include "afxcmn.h"

// CProp_LvRes_Character dialog
#include "ModifiedListCtrl.h"

class CProp_LvRes_Character : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvRes_Character)

protected:
	i3LevelResChara		*	m_pResCharacter;

public:
	CProp_LvRes_Character(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvRes_Character();

// Dialog Data
	enum { IDD = IDD_PROP_LVRES_CHARACTER };

	virtual void	SetObject( i3ElementBase * pObj) override;

protected:
	void	_UpdateResInfo( void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CModifiedListCtrl m_listCharaInfo;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
