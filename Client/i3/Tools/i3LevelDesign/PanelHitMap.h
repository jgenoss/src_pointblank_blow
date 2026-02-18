#pragma once

#include "HitMapTimeSeqCtrl.h"
#include "Prop_LvElm_HitMap.h"
class CPanelHitMap : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelHitMap)

protected:
	//	CPropWorld	m_PropWorld;
	i3TDKWndFoldCtrl			m_FoldCtrl;
	CHitMapTimeSeqCtrl			m_TimeSeqCtrl;
	CProp_LvElm_HitMap			m_Property_Main;
	CProp_LvElm_HitMapBuild		m_Property_Build;
	// Elements

public:
	CPanelHitMap(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelHitMap();

	// Dialog Data
	enum { IDD = IDD_PANEL_HITMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel() override;
	virtual void OnOK() override;
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void ResetRoomList() { m_Property_Main.ResetRoomList(); }
};

