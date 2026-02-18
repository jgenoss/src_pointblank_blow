#pragma once
#include "afxwin.h"


// CEditDlgAction_PlaySound dialog

class CEditDlgAction_PlaySound : public CDialog
{
	DECLARE_DYNAMIC(CEditDlgAction_PlaySound)

public:
	CEditDlgAction_PlaySound(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditDlgAction_PlaySound();

// Dialog Data
	enum { IDD = IDD_EDITACTION_SOUND };

protected:
	i3SoundNode * m_pNode;

public:
	void	SetSoundEventData( i3SoundNode * pData);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edSoundFile;
	CEdit m_edVolume;
	CEdit m_edStartTime;
	CButton m_cbLoopPlay;
	CEdit m_edLoopCount;
	virtual BOOL OnInitDialog();
	CComboBox m_cbSoundType;
	CComboBox m_cb3DMode;
protected:
	virtual void OnOK();
public:
	CButton m_cbSelectSoundFile;
	afx_msg void OnBnClickedEdSoundFileselect();
};
