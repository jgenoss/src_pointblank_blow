#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxcmn.h"

// CDlg_InsertEventHandler dialog

class CDlg_InsertEventHandler : public CDialog
{
	DECLARE_DYNAMIC(CDlg_InsertEventHandler)

public:
	CDlg_InsertEventHandler(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_InsertEventHandler();

// Dialog Data
	enum { IDD = IDD_DLG_INSERTEVENTHANDLER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	bool			m_bScreen;					// Dlg를 생성한 창이 Screen소속인지 Library소속인지
	i3UIControl*	m_pCtrl;					// Classes ListBox에서 현재 선택된 컨트롤
	i3UITemplate*	m_pTemplate;				// Classes ListBox에서 현재 선택된 템플릿
	char			m_szGenCode[MAX_PATH];		// Class와 Handler를 조합하여 생성된 코드
	char			m_szBlock[MAX_PATH];		// Dlg가 열리는 시점에 Caret이 어느 블록 안에 있었는지
	char			m_szFunctionName[MAX_PATH];


	void			_SetInitialClass( void);

	void			_OnLBSelChange_Classes( void);
	void			_OnLBSelChange_Handlers( void);
	void			_OnTBSelChange_Category( void);
	
	//void			_UpdateListBox_Classes( void);
	void			_UpdateListBox_Handlers( void);
	void			_UpdateEditBox_Desc( void);

public:
	void	SetScreen( bool bFlag)				{	m_bScreen = bFlag;	}
	void	SetBlock( const char* szBlock)		{	i3::string_ncopy_nullpad( m_szBlock, szBlock, MAX_PATH);	}
	
	void	GetGeneratedCodeBlock( char* pBuf, INT32 nBufSize);
	void	GetFunctionName( char* pBuf, INT32 nBufSize);
	void	GetBlockName( char* pBuf, INT32 nBufSize);
	bool	isEventHandler( void)				{	return m_bEventHandler;	}
		
		

	// MFC
protected:
	CListBox	m_lbClasses;
	CTabCtrl	m_tabCategory;
	CListBox	m_lbHandlers;
	CString		m_strDesc;
	CButton		m_btnOK;

public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnTcnSelchange_Category(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLbnSelchange_Classes();
	afx_msg void OnLbnSelchange_Handlers();
	afx_msg void OnLbnDblclk_Handlers();

protected:
	virtual void OnOK() override;
	bool			m_bEventHandler;
};
