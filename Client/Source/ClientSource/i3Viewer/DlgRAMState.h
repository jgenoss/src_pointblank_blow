#pragma once

#if defined( I3_DEBUG)
#include "resource.h"
#include "afxwin.h"

// CDlgRAMState dialog

class CDlgRAMState : public CDialog
{
	DECLARE_DYNAMIC(CDlgRAMState)

protected:
	void		_Update(void);

	static bool	_EnumClassProc( i3ElementBase * pObj, void * pData);
	void		_UpdateClasses(void);

	static bool	_EnumPoolProc( i3FixedMemoryPool * pPool, void * pData);
	void		_UpdatePool(void);

public:
	CDlgRAMState(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRAMState();

// Dialog Data
	enum { IDD = IDD_RAM_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	CEdit m_ED_PhysicalMem;
	CEdit m_ED_ProcessMem;
	CEdit m_ED_UsedCount;
	CEdit m_ED_UsedSize;
	CEdit m_ED_ClassCount;
	CEdit m_ED_ClassSize;
	CEdit m_ED_PoolCount;
	CEdit m_ED_PoolSize;
	afx_msg void OnBnClickedBtnUsed();
	afx_msg void OnBnClickedBtnClass();
	afx_msg void OnBnClickedBtnPool();
};

#endif
