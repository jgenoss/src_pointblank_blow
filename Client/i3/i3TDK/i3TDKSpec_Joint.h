#pragma once
#include "afxwin.h"


// i3TDKSpec_Joint dialog

class i3TDKSpec_Joint : public CDialog
{
	DECLARE_DYNAMIC(i3TDKSpec_Joint)

protected:
	i3PhysixShapeSet * m_pShapeSet = nullptr;

public:
	i3TDKSpec_Joint(CWnd* pParent = nullptr) : CDialog(i3TDKSpec_Joint::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Joint() {}

	// Dialog Data
	enum { IDD = IDD_TDK_SPEC_JOINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EdtJointNameCtrl;
	CEdit m_EdtMotionXCtrl;
	CEdit m_EdtMotionYCtrl;
	CEdit m_EdtMotionZCtrl;
	CEdit m_EdtSwing1MotionCtrl;
	CEdit m_EdtSwing2MotionCtrl;
	CEdit m_EdtTwistMotionCtrl;
	CEdit m_EdtLinearLimitValueCtrl;
	CEdit m_EdtLinearLimitRestitutionCtrl;
	CEdit m_EdtLinearLimitSpringCtrl;
	CEdit m_EdtLinearLimitDampingCtrl;
	CEdit m_EdtSwing1LimitValueCtrl;
	CEdit m_EdtSwing1LimitRestitutionCtrl;
	CEdit m_EdtSwing1LimitSpringCtrl;
	CEdit m_EdtSwing1LimitDampingCtrl;
	CEdit m_EdtSwing2LimitValueCtrl;
	CEdit m_EdtSwing2LimitRestitutionCtrl;
	CEdit m_EdtSwing2LimitSpringCtrl;
	CEdit m_EdtSwing2LimitDampingCtrl;
	CEdit m_EdtGearRatioCtrl;


	CEdit m_EdtTwistLimitHighValueCtrl;
	CEdit m_EdtTwistLimitHighRestitutionCtrl;
	CEdit m_EdtTwistLimitHighSpringCtrl;
	CEdit m_EdtTwistLimitHighDampingCtrl;
	CEdit m_EdtTwistLimitLowValueCtrl;
	CEdit m_EdtTwistLimitLowRestitutionCtrl;
	CEdit m_EdtTwistLimitLowSpringCtrl;
	CEdit m_EdtTwistLimitLowDampingCtrl;

	virtual BOOL OnInitDialog();

	BOOL	Execute( i3PhysixShapeSet * pShapeSet );
	void	AssignJointInfo();
	void	CheckMotion( NxD6JointMotion * pMotion, const TCHAR* pStr );
	afx_msg void OnEnChangeEditMotionX();
	afx_msg void OnEnChangeEditMotionY();
	afx_msg void OnEnChangeEditMotionZ();
	afx_msg void OnEnChangeEditMotionSwing1();
	afx_msg void OnEnChangeEditMotionSwing2();
	afx_msg void OnEnChangeEditMotionTwist();
	afx_msg void OnEnChangeEditLinearlimitValue();
	afx_msg void OnEnChangeEditLinearlimitRestitution();
	afx_msg void OnEnChangeEditLinearlimitSpring();
	afx_msg void OnEnChangeEditLinearlimitDamping();
	afx_msg void OnEnChangeEditSwing1limitValue();
	afx_msg void OnEnChangeEditSwing1limitRestitution();
	afx_msg void OnEnChangeEditSwing1limitSpring();
	afx_msg void OnEnChangeEditSwing1limitDamping();
	afx_msg void OnEnChangeEditSwing2limitValue();
	afx_msg void OnEnChangeEditSwing2limitRestitution();
	afx_msg void OnEnChangeEditSwing2limitSpring();
	afx_msg void OnEnChangeEditSwing2limitDamping();
	afx_msg void OnEnChangeEditTwistlimitHighValue();
	afx_msg void OnEnChangeEditTwistlimitHighRestitution();
	afx_msg void OnEnChangeEditTwistlimitHighSpring();
	afx_msg void OnEnChangeEditTwistlimitHighDamping();
	afx_msg void OnEnChangeEditTwistlimitLowValue();
	afx_msg void OnEnChangeEditTwistlimitLowRestitution();
	afx_msg void OnEnChangeEditTwistlimitLowSpring();
	afx_msg void OnEnChangeEditTwistlimitLowDamping();
	afx_msg void OnEnChangeEditGearratio();
};
