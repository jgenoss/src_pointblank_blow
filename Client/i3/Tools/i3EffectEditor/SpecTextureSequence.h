#pragma once
#include "afxwin.h"


// CSpecTextureSequence dialog

class CSpecTextureSequence : public CDialog
{
	DECLARE_DYNAMIC(CSpecTextureSequence)

protected:
	i3Particle *			m_pParticle;

protected:
	void	AssignValues(void);

public:
	BOOL	Execute( i3Particle * pParticle);

public:
	CSpecTextureSequence(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecTextureSequence();

// Dialog Data
	enum { IDD = IDD_TEXSEQSPEC };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CButton m_StaticTimeCtrl;
	CButton m_RepeatCtrl;
	CButton m_RandomCtrl;
	CEdit m_FrameCountCtrl;
	CEdit m_FrameIntervalCtrl;
	CEdit m_FrameWidthCtrl;
	CEdit m_FrameHeightCtrl;
	int m_TextureWidth;
	int m_TextureHeight;
	CButton m_FixedFrameCtrl;
	CEdit m_StartIndexCtrl;
};
