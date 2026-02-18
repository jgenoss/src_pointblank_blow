// DebugPanel.cpp : implementation file
//

#include "stdafx.h"
#include <psapi.h>

#if defined( I3_DEBUG)
#include "DlgPoolSpec.h"
#include "GlobalVar.h"

// CDlgPoolSpec dialog

IMPLEMENT_DYNAMIC(CDlgPoolSpec, CDialog)

CDlgPoolSpec::CDlgPoolSpec(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPoolSpec::IDD, pParent)
{
	m_pPool = NULL;
}

CDlgPoolSpec::~CDlgPoolSpec()
{
}

bool CDlgPoolSpec::Execute( i3FixedMemoryPool * pPool)
{
	m_pPool = pPool;

	if( DoModal() == IDOK)
		return true;

	return false;
}

void CDlgPoolSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_TOTALCOUNT, m_ED_TotalPage);
	DDX_Control(pDX, IDC_ED_TOTALSIZE, m_ED_TotalSize);
	DDX_Control(pDX, IDC_ED_PAGESIZE, m_ED_PageSize);
	DDX_Control(pDX, IDC_ED_UNITSIZE, m_ED_UnitSize);
	DDX_Control(pDX, IDC_ED_ALLOCCOUNT, m_ED_AllocCount);
	DDX_Control(pDX, IDC_ED_FREECOUNT, m_ED_FreeCount);
}


BEGIN_MESSAGE_MAP(CDlgPoolSpec, CDialog)
END_MESSAGE_MAP()


// CDlgPoolSpec message handlers


BOOL CDlgPoolSpec::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		char conv[256];
		UINT32 sz, alloccnt, totalcnt;

		// Name
		i3::snprintf( conv, sizeof(conv), "<%s> memory pool", m_pPool->getName());
		SetWindowText( conv);

		totalcnt = m_pPool->GetTotalBlockCount();
		alloccnt = m_pPool->GetAllocatedBlockCount();

		// Total Page
		i3::snprintf( conv, sizeof(conv), "%d", m_pPool->GetPageCount());
		m_ED_TotalPage.SetWindowText( conv);

		// Total Size
		sz = m_pPool->getPageSize() * m_pPool->GetPageCount();
		i3::snprintf( conv, sizeof(conv), "%d", sz);
		m_ED_TotalSize.SetWindowText( conv);

		// Page Size
		i3::snprintf( conv, sizeof(conv), "%d", m_pPool->getPageSize());
		m_ED_PageSize.SetWindowText( conv);

		// Unit Size
		i3::snprintf( conv, sizeof(conv), "%d", m_pPool->getUnitSize());
		m_ED_UnitSize.SetWindowText( conv);

		// Alloc Count
		i3::snprintf( conv, sizeof( conv), "%d", alloccnt);
		m_ED_AllocCount.SetWindowText( conv);

		// Free Count
		i3::snprintf( conv, sizeof( conv), "%d", totalcnt - alloccnt);
		m_ED_FreeCount.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


#endif
