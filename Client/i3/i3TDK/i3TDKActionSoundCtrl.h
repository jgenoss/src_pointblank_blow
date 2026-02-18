#pragma once
#include "afxwin.h"


// i3TDKActionSoundCtrl dialog

class I3_EXPORT_TDK i3TDKActionSoundCtrl : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKActionSoundCtrl)

protected:
	i3ActionPlaySound	*	m_pActionSound = nullptr;
	i3SoundPlayInfo		*	m_pPlayInfo = nullptr;

public:
	i3TDKActionSoundCtrl(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKActionSoundCtrl::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKActionSoundCtrl();

// Dialog Data
	enum { IDD = IDD_ACTION_SOUND };

	virtual void	SetObject( i3ElementBase * pObj);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	// Soune Resource Path
	CEdit m_edSoundPath;
	int m_edDefaultVolume = 100;
	int m_edListenerVolume = 100;
	float m_edMinDistance = 0.1f;
	float m_edMaxDistance = 100.0f;
	int m_edInsideConeAngle = 360;
	int m_edOutsideConeAngle = 360;
	int m_edOutsideVolume = 100;
	CEdit m_edDirection_X;
	CEdit m_edDirection_Y;
	CEdit m_edDirection_Z;


protected:
	virtual void OnOK();
	virtual void OnCancel();

	void	UpdateVariables( i3SoundPlayInfo * pInfo);

public:
	void	SetAction( i3Action * pAction);

	afx_msg void OnBnClickedBtnSelectPath();
	afx_msg void OnEnChangeEditSoundMaxdist();
	CEdit m_ctrlDefaultVolume;
	CEdit m_ctrlListenerVolume;
	CEdit m_ctrlMinDistance;
	CEdit m_ctrlMaxDistance;
	CEdit m_ctrlInsideConeAngle;
	CEdit m_ctrlOutsideConeAngle;
	CEdit m_ctrlOutsideVolume;
	afx_msg void OnEnChangeEditSoundDirectionX();
	afx_msg void OnEnChangeEditSoundDirectionY();
	afx_msg void OnEnChangeEditSoundDirectionZ();
	afx_msg void OnEnChangeEditSoundInsideangle();
	afx_msg void OnEnChangeEditSoundListenervolume();
	afx_msg void OnEnChangeEditSoundMindist();
	afx_msg void OnEnChangeEditSoundOutsideangle();
	afx_msg void OnEnChangeEditSoundOutsidevolume();
	afx_msg void OnEnChangeEditSoundVolume();
	CButton m_btnMuteAtMaxDistance;
	afx_msg void OnBnClickedChkMuteAtMaxdistance();
	afx_msg void OnBnClickedOk2();
	virtual BOOL OnInitDialog();
	CButton m_chkRandomEnable;
	CEdit m_edRandomFactor;
	afx_msg void OnBnClickedCheckSndRandomEnable();
	afx_msg void OnEnChangeEditSndRandomFactor();
};
