#pragma once
#include "afxwin.h"
#include "resource.h"


// CSetNodeFlagDlg dialog

class CSetNodeFlagDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetNodeFlagDlg)

public:
	CSetNodeFlagDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetNodeFlagDlg();

	INT32	m_nFlag;
// Dialog Data
	enum { IDD = IDD_DIALOG_NODEFLAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CButton m_bDisable;
	CButton m_bInvisible;
	CButton m_bCollidable;
	CButton m_bTransform;
	CButton m_bTransparent;
	CButton m_bStatic;
	CButton m_bDeleteOriginalNode;
	CButton m_bUser;
public:
	virtual BOOL OnInitDialog();
private:
	bool Update(void);
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CButton m_ControlCtrl;
	CButton m_CHK_Volatile;
	CButton m_CHK_ManualUpdate;
	CButton m_CHK_DisableShadowCaster;
	CButton m_CHK_DisableShadowReceiver;
	CButton m_CHK_AttachPoint;
};
