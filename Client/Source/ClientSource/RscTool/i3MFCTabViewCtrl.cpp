#include "stdafx.h"
#include "i3MFCTabViewCtrl.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(i3MFCTabViewCtrl, CMFCTabCtrl)

BEGIN_MESSAGE_MAP(i3MFCTabViewCtrl, CMFCTabCtrl)
	//{{AFX_MSG_MAP(CTabView)
	ON_WM_CREATE()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

namespace
{
	i3MFCTabViewCtrlCallback		g_cb_null;
}


i3MFCTabViewCtrl::i3MFCTabViewCtrl() : m_cb(&g_cb_null), m_iOldTab(-1)
{
	
}

i3MFCTabViewCtrl::~i3MFCTabViewCtrl()
{

}

BOOL i3MFCTabViewCtrl::Create(CWnd* parent)
{
	BOOL res = CMFCTabCtrl::Create(CMFCTabCtrl::STYLE_3D, CRect(0,0,0,0), parent, 1, CMFCTabCtrl::LOCATION_TOP, FALSE);

	return res;
}


CDocument* i3MFCTabViewCtrl::GetDocument() const
{
	CView* pView = DYNAMIC_DOWNCAST(CView, GetParent() );
	if (pView == NULL) return NULL;
	return pView->GetDocument();			// 일단 이정도면 무난해 보인다..
}

CView*		i3MFCTabViewCtrl::GetParentView() const
{
	return DYNAMIC_DOWNCAST(CView, GetParent() );
}

int i3MFCTabViewCtrl::AddView(CRuntimeClass* pViewClass, const CString& strViewLabel, int iIndex , CCreateContext* pContext )
{
	
	ASSERT_VALID(this);
	ENSURE(pViewClass != NULL);
	ENSURE(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CView)));

	CView* pView = DYNAMIC_DOWNCAST(CView, pViewClass->CreateObject());
	ASSERT_VALID(pView);

	if (!pView->Create(NULL, _T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, (UINT) -1, pContext) )
	{
		TRACE1("i3MFCTabViewCtrl:Failed to create view '%s'\n", pViewClass->m_lpszClassName);
		return -1;
	}

	CDocument* pDoc = GetDocument();			// 원래 뷰에만 존재하던 함수일텐데...호환성고민이 좀 된다..

	if (pDoc != NULL)
	{
		ASSERT_VALID(pDoc);

		BOOL bFound = FALSE;
		for (POSITION pos = pDoc->GetFirstViewPosition(); !bFound && pos != NULL;)
		{
			if (pDoc->GetNextView(pos) == pView)
			{
				bFound = TRUE;
			}
		}

		if (!bFound)
		{
			pDoc->AddView(pView);
		}
	}

	this->InsertTab(pView, strViewLabel, iIndex);

	int nTabs = this->GetTabsNum();
	return nTabs - 1;
}

int i3MFCTabViewCtrl::FindTab(HWND hWndView) const
{
	for (int i = 0; i < this->GetTabsNum(); i++)
	{
		if (this->GetTabWnd(i)->GetSafeHwnd() == hWndView)
		{
			return i;
		}
	}

	return -1;	
}

BOOL i3MFCTabViewCtrl::RemoveView(int iTabNum)
{
	return this->RemoveTab(iTabNum);
}

BOOL i3MFCTabViewCtrl::SetActiveView(int iTabNum)
{
	return this->SetActiveTab(iTabNum);
}

CView* i3MFCTabViewCtrl::GetActiveView() const
{
	int iActiveTab = this->GetActiveTab();
	if (iActiveTab < 0)
		return NULL;
	
	return DYNAMIC_DOWNCAST(CView, this->GetTabWnd(iActiveTab));
}

CView* i3MFCTabViewCtrl::GetView(int iTabNum) const
{
	return 	DYNAMIC_DOWNCAST(CView, this->GetTabWnd(iTabNum));
}


int i3MFCTabViewCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1) 
		return -1;

	this->SetFlatFrame();
	this->SetTabBorderSize(0);
	this->AutoDestroyWindow(FALSE);
//	this->SetFont (&afxGlobalData.fontRegular);
	this->SetActiveTabBoldFont(TRUE);
	this->ModifyTabStyle(STYLE_3D_VS2005);

//	this->HideActiveWindowHorzScrollBar();
	
	return 0;
}

void   i3MFCTabViewCtrl::Embed_OnSize( int cx_view, int cy_view)
{
	this->SetWindowPos(NULL, -1, -1, cx_view + 1, cy_view + 3, SWP_NOACTIVATE | SWP_NOZORDER );


//	this->SetTabsHeight();
//	this->SetTabMaxWidth(100);
//	this->RecalcLayout();
//	this->RedrawWindow ();

	

}

// 아래 가상 함수는 기본함수 호출을 뒤에 둔다.. (내부는 별도구현이 아닌 부모 윈도우에 대한 노티 호출로 되어있다.)

void  i3MFCTabViewCtrl::FireChangeActiveTab(int nNewTab)
{
	i3MFCTabViewCtrl::ChangeActiveTabImpl(nNewTab);

	CMFCTabCtrl::FireChangeActiveTab(nNewTab);
}

// 아래 가상 함수는 기본함수 호출을 뒤에 둔다.. (내부는 별도구현이 아닌 부모 윈도우에 대한 노티 호출로 되어있다.)

BOOL  i3MFCTabViewCtrl::FireChangingActiveTab(int nNewTab)
{
	
	return CMFCTabCtrl::FireChangingActiveTab(nNewTab);
}

void	i3MFCTabViewCtrl::ChangeActiveTabImpl(int nNewTab)
{
	/*
	if (!m_bIsReady)
	{
		m_nFirstActiveTab = nNewTab;
		return;
	}
*/

	// TODO : iOldTab 변수가 제대로 처리되지 않음..멤버변수로 OldTab변수를 보관하도록 수정할것..

	int iOldTab = m_iOldTab; //this->GetActiveTab();

	CView* pOldView = (iOldTab < 0) ? NULL : DYNAMIC_DOWNCAST(CView, this->GetTabWnd(iOldTab));

	CFrameWnd* pFrame = AFXGetParentFrame(this);			// 다소 거지같지만 많이 쓰이는 함수이다.. 일단 방치.. (메인 프레임이다..)
	ASSERT_VALID(pFrame);

	int iTabNum = m_iOldTab = (int) nNewTab;
	if (iTabNum >= 0)
	{
		CView* pView = DYNAMIC_DOWNCAST(CView, this->GetTabWnd(iTabNum));
		ASSERT_VALID(pView);
	
		pFrame->SetActiveView(pView);
		m_cb->OnActivateView(this, pOldView, pView, iOldTab, iTabNum);

	}
	else
	{
		
		pFrame->SetActiveView(NULL);		
		m_cb->OnActivateView(this, pOldView, NULL, iOldTab, iTabNum);
	}
	
}


int i3MFCTabViewCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	CMFCTabCtrl::OnMouseActivate(pDesktopWnd, nHitTest, message);

	return MA_NOACTIVATE;
}

/*
namespace 
{

	class CInternalTabView : public CView
	{
		friend class i3MFCTabViewCtrl;
	};

}

int i3MFCTabViewCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{

	CView* pCurrView = GetActiveView();

	if (pCurrView == NULL)
	{
		return CMFCTabCtrl::OnMouseActivate(pDesktopWnd, nHitTest, message);
	}

	int nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);

	if (nResult == MA_NOACTIVATE || nResult == MA_NOACTIVATEANDEAT)
		return nResult;   // frame does not want to activate

	CFrameWnd* pParentFrame = AFXGetParentFrame(this);
	
	if (pParentFrame != NULL)
	{
		// eat it if this will cause activation
		ASSERT(pParentFrame == pDesktopWnd || pDesktopWnd->IsChild(pParentFrame));

		// either re-activate the current view, or set this view to be active
		CView* pView = pParentFrame->GetActiveView();
		HWND hWndFocus = ::GetFocus();

//		if (pView->GetParent() == pCurrView )
//		{
//			pCurrView = pView;
//		}

		if (pView == pCurrView && pCurrView->m_hWnd != hWndFocus && !::IsChild(pCurrView->m_hWnd, hWndFocus))
		{
			// re-activate this view
			((CInternalTabView*)pCurrView)->OnActivateView(TRUE, pCurrView, pCurrView);
		}
		else
		{
			// activate this view
			pParentFrame->SetActiveView(pCurrView);
		}
	}

	return nResult;	
	
}
*/

