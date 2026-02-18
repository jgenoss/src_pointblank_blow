#include "pch.h"
#include "PanelHitMap.h"
#include "i3LevelDesign.h"
#include ".\panelproperty.h"


// CPanelProperty dialog

IMPLEMENT_DYNAMIC(CPanelHitMap, i3TDKDialogBase)
CPanelHitMap::CPanelHitMap(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelHitMap::IDD, pParent)
{
}

CPanelHitMap::~CPanelHitMap()
{
}

void CPanelHitMap::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelHitMap, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanelProperty message handlers

void CPanelHitMap::OnCancel()
{
}

void CPanelHitMap::OnOK()
{
}

BOOL CPanelHitMap::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rect;

		GetClientRect(&rect);

		rect.left = 3;
		rect.top = 3;
		rect.right -= 3;
		rect.bottom -= 3;
		m_FoldCtrl.Create("HitMap", WS_VISIBLE | WS_CHILD, rect, this, 200);

		m_Property_Main.Create(CProp_LvElm_HitMap::IDD, this);
 		m_FoldCtrl.AddPane(&m_Property_Main, "HitMap Propertys");
 		
		m_TimeSeqCtrl.Create(WS_VISIBLE | WS_CHILD, rect, this, 201);
 		m_FoldCtrl.AddPane(&m_TimeSeqCtrl, "TimeLine");

		m_Property_Build.Create(CProp_LvElm_HitMapBuild::IDD, this);
		m_FoldCtrl.AddPane(&m_Property_Build, "HitMap Property Build");

		m_Property_Main.SetTimeCtrl(&m_TimeSeqCtrl);
		m_Property_Build.SetTimeCtrl(&m_TimeSeqCtrl);

		m_TimeSeqCtrl.SetHitMapCommonProperty(&m_Property_Main);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelHitMap::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if (::IsWindow(m_FoldCtrl.m_hWnd))
	{
		m_FoldCtrl.SetWindowPos(NULL, 3, 3, cx - 6, cy - 6, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}
