#pragma once

// CBaseSpecDialog dialog

class CBaseSpecDialog : public CDialog
{
	DECLARE_DYNAMIC(CBaseSpecDialog)

public:
	CBaseSpecDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBaseSpecDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	BOOL			m_bDock;
//	WTCObject *		m_pObject;

public:
	virtual BOOL Create( BOOL bDock, CWnd * pParentWnd);
	//virtual void Refresh( INT32 iRefreshMode, WTCObject * pObject);
	virtual void Refresh( INT32 iRefreshMode );

//	void	SetObject( WTCObject * pObj)					{ m_pObject = pObj; }

public:
	virtual BOOL OnInitDialog();
};
