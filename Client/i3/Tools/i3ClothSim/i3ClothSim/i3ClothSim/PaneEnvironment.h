#pragma once


// CPaneEnvironment dialog

class CPaneEnvironment : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneEnvironment)

public:
	CPaneEnvironment(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneEnvironment();

// Dialog Data
	enum { IDD = IDD_PANE_ENV };

protected:
	i3ClothObject	* m_pCloth;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit		m_WindFactorCtrl;
	CEdit		m_WindMinCtrl;
	CEdit		m_WindMaxCtrl;
	CEdit		m_GravityCtrl;
	BOOL		m_bGravityAxisY;
	BOOL		m_bCollisionTest;
	BOOL		m_bImpulseTest;

	void		setCloth( i3ClothObject * pCloth )	{ m_pCloth = pCloth; }
	i3ClothObject * getCloth(void)					{ return m_pCloth; }

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnChangeEdSimGravity();
};
