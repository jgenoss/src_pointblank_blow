#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CParticleSpecObject dialog

class CParticleSpecObject : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CParticleSpecObject)

public:
	CParticleSpecObject(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParticleSpecObject();

// Dialog Data
	enum { IDD = IDD_PARTICLESPEC_OBJECT };

protected:
	i3ParticleInfo * m_pInfo;

public:
	void	SetParticle( i3ParticleInfo * pInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_LifetimeCtrl;
	CSpinButtonCtrl m_LifetimeSpinCtrl;
	CEdit m_MaxCountCtrl;
	CSpinButtonCtrl m_MaxCountSpinCtrl;
	CEdit m_InitCountCtrl;
	CEdit m_CapacityCountCtrl;
	CSpinButtonCtrl m_InitCountSpinCtrl;
	CSpinButtonCtrl m_CapacityCountSpinCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	CEdit m_MassMinCtrl;
	CEdit m_MassMaxCtrl;
	afx_msg void OnEnKillFocus();
	afx_msg void OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult);

private:
	void InitSpinCtrl();
};
