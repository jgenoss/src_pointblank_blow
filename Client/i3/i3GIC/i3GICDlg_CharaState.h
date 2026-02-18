#pragma once
#include "afxwin.h"
#include "resource.h"

// i3GICDlg_CharaState dialog

class i3GICDlg_CharaState : public CDialog
{
	DECLARE_DYNAMIC(i3GICDlg_CharaState)
protected:
	UINT32			m_State = 0;

public:
	i3GICDlg_CharaState(CWnd* pParent = nullptr) : CDialog(i3GICDlg_CharaState::IDD, pParent) {}   // standard constructor
	virtual ~i3GICDlg_CharaState() {}

	UINT32		getCharaState(void)			{ return m_State; }

	bool			Execute( UINT32 state);

// Dialog Data
	enum { IDD = IDD_GIC_DLG_CHARA_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_Flying;
	CButton m_CHK_Falling;
	CButton m_CHK_TouchWall;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
