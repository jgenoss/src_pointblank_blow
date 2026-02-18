#pragma once
#include "afxwin.h"


// CEditEffectSound dialog

class CEditEffectSound : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CEditEffectSound)

public:
	CEditEffectSound(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditEffectSound();

	i3SoundNode * m_pSoundNode;

	virtual void SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_cbLoopEnable;

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CEdit m_edLoopCount;
	afx_msg void OnEnChangeEditSndLoopcnt();
	afx_msg void OnBnClickedCheck1();
};
