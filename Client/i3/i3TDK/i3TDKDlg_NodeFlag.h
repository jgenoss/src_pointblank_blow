#pragma once
#include "afxwin.h"


// i3TDKDlg_NodeFlag dialog

class i3TDKDlg_NodeFlag : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlg_NodeFlag)
protected:
	i3Node *		m_pNode = nullptr;

protected:
	void			setCheck( INT32 flag, CButton * pBtn);
	void			getCheck( INT32 flag, CButton * pBtn);

public:
	i3TDKDlg_NodeFlag(CWnd* pParent = nullptr) : CDialog(i3TDKDlg_NodeFlag::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlg_NodeFlag() {}

	bool	Execute( i3Node * pNode);

// Dialog Data
	enum { IDD = IDD_TDK_DLG_NODEFLAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_Disable;
	CButton m_Invisible;
	CButton m_MustBeProcess;
	CButton m_Transparent;
	CButton m_Static;
	CButton m_Volatile;
	CButton m_Control;
	CButton m_ManualUpdate;
	CButton m_Dynamic;
	CButton m_DisableShadowCasting;
	CButton m_DisableShadowReceive;
	CButton m_NoWork;
	CButton m_AttachPoint;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
