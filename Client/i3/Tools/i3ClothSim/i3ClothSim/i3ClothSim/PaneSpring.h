#pragma once


// CPaneSpring dialog

class CPaneSpring : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneSpring)

public:
	CPaneSpring(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneSpring();

	void		SetSpring( i3Spring * pSpring );
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

// Dialog Data
	enum { IDD = IDD_PANE_SPRING };

protected:
	i3Spring		*m_pSpring;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CComboBox		m_TypeCbCtrl;
	
	CEdit			m_DamperEdCtrl;
	CSpinButtonCtrl	m_DamperSpinCtrl;
	CEdit			m_ConstantEdCtrl;
	CSpinButtonCtrl	m_ConstantSpinCtrl;

	CButton			m_LoadButtonCtrl;

	void		setSpring( i3Spring * pSpring )		{ m_pSpring = pSpring; }

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnChangeEdSDamper();
	afx_msg void OnEnChangeEdSConstant();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
