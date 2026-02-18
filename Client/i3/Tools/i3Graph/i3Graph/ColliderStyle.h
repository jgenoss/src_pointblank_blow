#pragma once


// cColliderStyle dialog

class cColliderStyle : public CDialog
{
	DECLARE_DYNAMIC(cColliderStyle)

	i3ColliderSet * m_pColliderSet; 
public:
	
	cColliderStyle(CWnd* pParent = NULL);   // standard constructor
	virtual ~cColliderStyle();
	void SetColliderSet(i3ColliderSet * pColliderSet) { m_pColliderSet = pColliderSet; }

// Dialog Data
	enum { IDD = IDD_DIALOG_COLLIDER_STYLE };
	INT32 GetStyle(void)			{ return m_Style; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(void); 

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bDist;
	BOOL m_bPhys;
	BOOL m_bInter;
	BOOL m_bNor;
	BOOL m_bVel;
	INT32 m_Style; 
	afx_msg void OnBnClickedOk();
};
