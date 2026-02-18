#pragma once

#include "TimeSeqCtrl.h"
#include "GICShapeAI.h"

// CPanelTimeTrack dialog

class CPanelTimeTrack : public CDialog
{
	DECLARE_DYNAMIC(CPanelTimeTrack)

protected:
	CTimeSeqCtrl			m_TimeSeqCtrl;

	GICShapeAI *			m_pAIState;

protected:
	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void			SetAIState( GICShapeAI * pState);

public:
	CPanelTimeTrack(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelTimeTrack();

// Dialog Data
	enum { IDD = IDD_PANEL_TIMETRACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnCancel();
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
