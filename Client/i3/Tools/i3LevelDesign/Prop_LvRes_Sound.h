#pragma once
#include "afxcmn.h"


// CProp_LvRes_Sound dialog
#include "ModifiedListCtrl.h"

class CProp_LvRes_Sound : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvRes_Sound)

protected:
	i3LevelResSound	*	m_pResSound;

public:
	CProp_LvRes_Sound(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvRes_Sound();

// Dialog Data
	enum { IDD = IDD_PROP_LVRES_SOUND };

	virtual void	SetObject( i3ElementBase * pObj) override;

protected:
	void	_UpdateResInfo( void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CModifiedListCtrl m_listSoundInfo;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
