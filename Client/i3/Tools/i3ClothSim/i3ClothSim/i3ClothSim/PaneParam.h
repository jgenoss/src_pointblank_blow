#pragma once


// CPaneParam dialog

class CPaneParam : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneParam)

public:
	CPaneParam(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneParam();

	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

// Dialog Data
	enum { IDD = IDD_PANE_PARAM };

protected:
	i3PhysicsParticle	* m_pParticle;	// Current selected particle

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	CEdit					m_MassEditCtrl;
	CButton					m_LoadBtCtrl;

	BOOL					m_bLock;
	BOOL					m_bGravity;
	BOOL					m_bVelocity;

	BOOL					m_bCollisionSelf;
	BOOL					m_bCollisionObject;
	BOOL					m_bCollisionWorld;

	BOOL					m_bImpulse;
	CEdit					m_ImpulseEditCtrl;
	CSpinButtonCtrl			m_ImpulseSpinCtrl;
	BOOL					m_bFriction;
	CEdit					m_FrictionEditCtrl;
	CSpinButtonCtrl			m_FrictionSpinCtrl;
	BOOL					m_bDrag;
	CEdit					m_DragEditCtrl;
	CSpinButtonCtrl			m_DragSpinCtrl;

	void		SetParticle( i3PhysicsParticle * pParticle );
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnChangeEdPMass();
	afx_msg void OnEnChangeEdPImpulse();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
