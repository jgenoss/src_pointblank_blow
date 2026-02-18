#pragma once

#if defined( I3_DEBUG)
#include "resource.h"
#include "afxwin.h"

// CDlgPoolSpec dialog

class CDlgPoolSpec : public CDialog
{
	DECLARE_DYNAMIC(CDlgPoolSpec)

protected:
	i3FixedMemoryPool *	m_pPool;

public:
	CDlgPoolSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPoolSpec();

	bool	Execute( i3FixedMemoryPool * pPool);

// Dialog Data
	enum { IDD = IDD_POOL_SPEC};

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	CEdit m_ED_TotalPage;
	CEdit m_ED_TotalSize;
	CEdit m_ED_PageSize;
	CEdit m_ED_UnitSize;
	CEdit m_ED_AllocCount;
	CEdit m_ED_FreeCount;
};

#endif
