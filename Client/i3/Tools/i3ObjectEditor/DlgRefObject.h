#pragma once
#include "afxcmn.h"


// CDlgRefObject dialog

class CDlgRefObject : public CDialog
{
	DECLARE_DYNAMIC(CDlgRefObject)
protected:
	i3GameObj *		m_pObj;

protected:
	void		_UpdateAllRef(void);
	void		_UpdateObj( INT32 idx, i3ResourceObject * pObj);
	void		_AddAnim( const char * pszPath);
	void		_AddTex( const char * pszPath);

public:
	CDlgRefObject(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRefObject();

	bool Execute( i3GameObj * pObj);

// Dialog Data
	enum { IDD = IDD_DLG_REFOBJ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ObjListCtrl;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedBtnDelete();
};
