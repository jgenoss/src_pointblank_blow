// DebugPanel.cpp : implementation file
//

#include "stdafx.h"
#include <psapi.h>

#if defined( I3_DEBUG)
#include "DlgRAMState.h"
#include "GlobalVar.h"
#include "DlgHeapInspector.h"
#include "DlgClassInspector.h"
#include "DlgMemoryPoolInspector.h"
#include "i3Base/string/ext/integral_to_comma_str.h"


#pragma comment( lib, "psapi.lib")

// CDlgRAMState dialog

IMPLEMENT_DYNAMIC(CDlgRAMState, CDialog)

CDlgRAMState::CDlgRAMState(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRAMState::IDD, pParent)
{

}

CDlgRAMState::~CDlgRAMState()
{
}

void CDlgRAMState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_PHYSICAL_MEM, m_ED_PhysicalMem);
	DDX_Control(pDX, IDC_ED_PROCESS_MEM, m_ED_ProcessMem);
	DDX_Control(pDX, IDC_ED_USED_COUNT, m_ED_UsedCount);
	DDX_Control(pDX, IDC_ED_USED_SIZE, m_ED_UsedSize);
	DDX_Control(pDX, IDC_ED_CLASS_COUNT, m_ED_ClassCount);
	DDX_Control(pDX, IDC_ED_CLASS_SIZE, m_ED_ClassSize);
	DDX_Control(pDX, IDC_ED_POOL_COUNT, m_ED_PoolCount);
	DDX_Control(pDX, IDC_ED_POOL_SIZE, m_ED_PoolSize);
}

struct CLASSREPORTINFO
{
	INT32		m_Count;
	UINT32		m_Size;
};

bool CDlgRAMState::_EnumClassProc( i3ElementBase * pObj, void * pData)
{
	CLASSREPORTINFO * pInfo = (CLASSREPORTINFO *) pData;
	i3ClassMeta * pMeta = pObj->meta();

	pInfo->m_Count++;
	pInfo->m_Size += pMeta->class_size();

	return true;
}

void CDlgRAMState::_UpdateClasses(void)
{
	CLASSREPORTINFO info;

	info.m_Count = 0;
	info.m_Size = 0;

	i3ElementBase::EnumObjects( _EnumClassProc, &info);

	char conv[256];

	i3::integral_to_comma_str( info.m_Count, conv, sizeof(conv));
	m_ED_ClassCount.SetWindowText( conv);

	i3::integral_to_comma_str( info.m_Size / (1024 * 1024), conv, sizeof(conv));
	m_ED_ClassSize.SetWindowText( conv);
}

bool CDlgRAMState::_EnumPoolProc( i3FixedMemoryPool * pPool, void * pData)
{
	CLASSREPORTINFO * pInfo = (CLASSREPORTINFO *) pData;

	pInfo->m_Count++;
	pInfo->m_Size += pPool->getPageSize() * pPool->GetPageCount();

	return true;
}

void CDlgRAMState::_UpdatePool(void)
{
	CLASSREPORTINFO info;

	info.m_Count = 0;
	info.m_Size = 0;

	i3FixedMemoryPool::Enum( _EnumPoolProc, &info);

	char conv[256];

	i3::integral_to_comma_str( info.m_Count, conv, sizeof(conv));
	m_ED_PoolCount.SetWindowText( conv);

	i3::integral_to_comma_str( info.m_Size / (1024 * 1024), conv, sizeof(conv));
	m_ED_PoolSize.SetWindowText( conv);
}

void CDlgRAMState::_Update(void)
{
	char conv[256];
	INT32 MB;

	// Physical Memory
	{
		MEMORYSTATUSEX state;

		state.dwLength =  sizeof(state);

		::GlobalMemoryStatusEx(&state);

		MB = (INT32)( state.ullTotalPhys / (1024 * 1024));

		i3::integral_to_comma_str( MB, conv, sizeof(conv));
		m_ED_PhysicalMem.SetWindowText( conv);
	}

	// Process Memory
	{
		PROCESS_MEMORY_COUNTERS_EX info;
		DWORD idProcess = GetCurrentProcessId();
		bool bGood = false;

		info.cb = sizeof(info);

		HANDLE hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, idProcess );

		if( hProcess != NULL)
		{
			if( GetProcessMemoryInfo( hProcess, (PROCESS_MEMORY_COUNTERS *) &info, sizeof(info)))
			{
				MB = (info.WorkingSetSize) / (1024 * 1024);

				i3::integral_to_comma_str( MB, conv, sizeof(conv));			
				m_ED_ProcessMem.SetWindowText( conv);

				bGood = true;
			}

			CloseHandle( hProcess);
		}

		if( bGood == false)
		{
			m_ED_ProcessMem.SetWindowText( "Error");
		}
	}

	// Heap
	{
		i3::integral_to_comma_str( i3mem::GetAllocCount(), conv, sizeof(conv));
		m_ED_UsedCount.SetWindowText( conv);

		MB = i3mem::GetAllocSize() / (1024 * 1024);
		i3::integral_to_comma_str( MB, conv, sizeof(conv));
		m_ED_UsedSize.SetWindowText( conv);
	}

	// Classes
	{
		_UpdateClasses();
	}

	_UpdatePool();
}

BEGIN_MESSAGE_MAP(CDlgRAMState, CDialog)
	ON_BN_CLICKED(IDC_BTN_USED, &CDlgRAMState::OnBnClickedBtnUsed)
	ON_BN_CLICKED(IDC_BTN_CLASS, &CDlgRAMState::OnBnClickedBtnClass)
	ON_BN_CLICKED(IDC_BTN_POOL, &CDlgRAMState::OnBnClickedBtnPool)
END_MESSAGE_MAP()


// CDlgRAMState message handlers


BOOL CDlgRAMState::OnInitDialog()
{
	CDialog::OnInitDialog();

	_Update();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRAMState::OnBnClickedBtnUsed()
{
	CDlgHeapInspector dlg;

	dlg.DoModal();
}

void CDlgRAMState::OnBnClickedBtnClass()
{
	CDlgClassInspector dlg;

	dlg.DoModal();
}

void CDlgRAMState::OnBnClickedBtnPool()
{
	CDlgMemoryPoolInspector dlg;

	dlg.DoModal();
}


#endif
