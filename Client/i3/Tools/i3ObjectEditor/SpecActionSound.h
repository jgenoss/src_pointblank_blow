#pragma once
#include "afxwin.h"


// CSpecActionSound dialog

class CSpecActionSound : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpecActionSound)

protected:
	i3ActionPlaySound	* m_pActionSound;
	i3SoundPlayInfo		* m_pPlayInfo;

public:
	CSpecActionSound(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecActionSound();

// Dialog Data
	enum { IDD = IDD_SPEC_ACTION_SOUND };

public:
	virtual void			SetObject( i3ElementBase * pObj) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// Soune Resource Path
	CEdit m_edSoundPath;
	int m_edDefaultVolume;
	int m_edListenerVolume;
	float m_edMinDistance;
	float m_edMaxDistance;
	int m_edInsideConeAngle;
	int m_edOutsideConeAngle;
	int m_edOutsideVolume;
	CEdit m_edDirection_X;
	CEdit m_edDirection_Y;
	CEdit m_edDirection_Z;
	CButton m_btnRepeat;
	CButton m_chkAttach;
	CComboBox m_cbBoneList;
	CButton m_chkRandomEnable;
	CEdit m_edRandomFactor;

protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;

	void	UpdateVariables( i3SoundPlayInfo * pInfo);

public:
	afx_msg void OnBnClickedBtnSelectPath();
	afx_msg void OnEnChangeEditSoundMaxdist();
	CEdit m_ctrlDefaultVolume;
	CEdit m_ctrlListenerVolume;
	CEdit m_ctrlMinDistance;
	CEdit m_ctrlMaxDistance;
	CEdit m_ctrlInsideConeAngle;
	CEdit m_ctrlOutsideConeAngle;
	CEdit m_ctrlOutsideVolume;
	CButton m_btnMuteAtMaxDistance;
	CButton m_btnStopOnReset;

	afx_msg void OnEnChangeEditSoundDirectionX();
	afx_msg void OnEnChangeEditSoundDirectionY();
	afx_msg void OnEnChangeEditSoundDirectionZ();
	afx_msg void OnEnChangeEditSoundInsideangle();
	afx_msg void OnEnChangeEditSoundListenervolume();
	afx_msg void OnEnChangeEditSoundMindist();
	afx_msg void OnEnChangeEditSoundOutsideangle();
	afx_msg void OnEnChangeEditSoundOutsidevolume();
	afx_msg void OnEnChangeEditSoundVolume();
	afx_msg void OnBnClickedChkMuteAtMaxdistance();
	afx_msg void OnBnClickedChkStopOnReset();
	afx_msg void OnBnClickedChkAttach();
	afx_msg void OnCbnSelchangeCbBoneList();
	afx_msg void OnBnClickedChkRepeat();
	afx_msg void OnBnClickedCheckSndRandomEnable();
	afx_msg void OnEnChangeEditSndRandomFactor();
};
