#pragma once
#if defined(USE_EDITDLG)
#include "afxwin.h"
#include "afxcmn.h"
#include "i3Framework.h"
#include "resource.h"
#include "i3TDKUpdateSystem.h"
#include "i3Framework/i3GameObjPropertyDialog.h"

class CObjSpec_RespawnHelicopter : public i3GameObjPropertyDialog
{
	DECLARE_DYNAMIC(CObjSpec_RespawnHelicopter)


public:
	CObjSpec_RespawnHelicopter(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CObjSpec_RespawnHelicopter();

// Dialog Data
	enum { IDD = IDD_PB_SPEC_RESPAWNHELICOPTER };

	virtual void	SetObject( i3ElementBase * pObj) override;

	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

protected:
	CEdit		m_ED_TeamIdx;

	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnEnKillfocusPbEdTeamIdx();
};

#endif