#pragma once
#include "i3TDKOptSpecBase.h"
#include "afxwin.h"

// i3TDKOptSpecNodeFlag dialog

class i3TDKOptSpecNodeFlag : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecNodeFlag)

public:
	i3TDKOptSpecNodeFlag(CWnd* pParent = nullptr) : i3TDKOptSpecBase(i3TDKOptSpecNodeFlag::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecNodeFlag() {}

// Dialog Data
	enum { IDD = IDD_OPTSPEC_NODEFLAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_Disable;
	CButton m_CHK_Disable2;
	CButton m_CHK_Invisible;
	CButton m_CHK_Invisible2;
	CButton m_CHK_MustBeProcess;
	CButton m_CHK_MustBeProcess2;
	CButton m_CHK_Transform;
	CButton m_CHK_Transform2;
	CButton m_CHK_Transparent;
	CButton m_CHK_Transparent2;
	CButton m_CHK_Static;
	CButton m_CHK_Static2;
	CButton m_CHK_Volatile;
	CButton m_CHK_Volatile2;
	CButton m_CHK_Control;
	CButton m_CHK_Control2;
	CButton m_CHK_ManualUpdate;
	CButton m_CHK_ManualUpdate2;
	CButton m_CHK_DisableShadowCast;
	CButton m_CHK_DisableShadowCast2;
	CButton m_CHK_DisableShadowReceive;
	CButton m_CHK_DisableShadowReceive2;
	CButton m_CHK_NoWork;
	CButton m_CHK_NoWork2;
protected:
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
};
