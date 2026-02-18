#pragma once
#include "afxwin.h"


// CSetChannelDialog dialog

class CSetChannelDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetChannelDialog)
protected:
	i3Animation *	m_pAnim;
	i3TransformSequence *	m_pSeq;

protected:
	void	CopyTransSeq( i3TransformSequence * pDest, i3TransformSequence * pSrc);

public:
	i3TransformSequence *	Execute( i3Animation * pAnim, i3TransformSequence * pSeq);

public:
	CSetChannelDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetChannelDialog();

// Dialog Data
	enum { IDD = IDD_SETCHANNELDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_TranslateCtrl;
	CButton m_RotateCtrl;
	CButton m_ScaleCtrl;
	CButton m_TimeCtrl;
	CButton m_FixedTranslateCtrl;
	CButton m_FixedRotateCtrl;
	CButton m_FixedScaleCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
