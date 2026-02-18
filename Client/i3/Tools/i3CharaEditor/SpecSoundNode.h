#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSpecSoundNode dialog

class CSpecSoundNode : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecSoundNode)

public:
	CSpecSoundNode(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecSoundNode();

	virtual void	SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_SPEC_SOUNDNODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_LoopCtrl;
	CEdit m_LoopCountCtrl;
	CButton m_PositionalCtrl;
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedChkLoop();
	afx_msg void OnBnClickedChkPositional();
	afx_msg void OnEnKillfocusEdLoopcount();
	CEdit m_PriorityCtrl;
	afx_msg void OnEnKillfocusEdPriority();
	CSliderCtrl m_VolCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedChkMuteAtmax();
	afx_msg void OnBnClickedOnStopReset();
	CEdit m_edSoundNodeName;
	CButton m_chkMuteAtMaxdistance;
	CButton m_chkStopOnReset;
	CEdit	m_edMinDistance;
	CEdit	m_edMaxDistance;
	afx_msg void OnEnChangeEdMaxdist();
	afx_msg void OnEnChangeEdMindist();
};
