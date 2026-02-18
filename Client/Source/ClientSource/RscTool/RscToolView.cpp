
// RscToolView.cpp : CRscToolView 클래스의 구현
//

#include "stdafx.h"
#include "RscTool.h"

#include "RscToolDoc.h"
#include "RscToolView.h"

#include "View1.h"
#include "PackScriptView.h"
#include "ItemView.h"
#include "FileCheckView.h"
#include "FileCheck_PackScriptMgr.h"

#include "MainFrm.h"

#include "PreprocessMessageCallbackDerived.h"


#include "PathMgr.h"
#include "PackMode.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{



	struct TabViewCallback : i3MFCTabViewCtrlCallback
	{
		CRscToolView*	owner;
		TabViewCallback(CRscToolView* owner) : owner(owner) {}
		
		virtual void OnActivateView(i3MFCTabViewCtrl* ctrl, 
			CView* oldView, CView* newView, 
			int oldTabNum, int newTabNum) 
		{
			//
			// 탭은 수시로 일어나는 것이고, 로딩은 최초에 1번만 발생해야한다..
			// 구분 방법을 알아야 함..
			// 가장 무난한 것은.. PathMgr쪽 입력과 관련되서 처리하는 것이다..
			//

			if (oldTabNum == 2 || oldTabNum == 3)
			{
				FileCheckView* pCheckView = (FileCheckView*)oldView;
				pCheckView->Update_OnDeactivateView();
			}

			switch(newTabNum)
			{

			case 1:
			{
				FileCheck_PackScriptMgr::i()->ClearPackScriptMap();

				if ( PathMgr::i()->IsPreprocessNeededForItemView() )
				{
					ItemView* pItemView = GetMainFrame()->GetItemView();	

					pItemView->DeleteAllListCtrls();
					
					PreprocessMessageCallbackDerived cb;

					if (PathMgr::i()->PreprocessForItemView(&cb) )
					{
						pItemView->UpdateAllListCtrls();
					}
				}

			}
			break;

			case 2:
				{
					FileCheckView* pCheckView = (FileCheckView*)newView;
					pCheckView->Update_OnActivateView();
					
					PreprocessMessageCallbackDerived cb;
					cb.OnStart();
					FileCheck_PackScriptMgr::i()->BuildPackScriptMap(PACKMODE_MEDIA_TO_QA_COPY, &cb);
					cb.OnEnd();

					pCheckView->Update_AfterActivateView();

				}
			break;
			
			case 3:
				{
					FileCheckView* pCheckView = (FileCheckView*)newView;
					pCheckView->Update_OnActivateView();

					PreprocessMessageCallbackDerived cb;
					cb.OnStart();
					FileCheck_PackScriptMgr::i()->BuildPackScriptMap(PACKMODE_QA_TO_LIVE_PACK, &cb);
					cb.OnEnd();

					pCheckView->Update_AfterActivateView();
				}
			break;

			default:
				{
					FileCheck_PackScriptMgr::i()->ClearPackScriptMap();
				}
			break;

			}

			


		}

	};
		
}


// CRscToolView

IMPLEMENT_DYNCREATE(CRscToolView, CView)

BEGIN_MESSAGE_MAP(CRscToolView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CRscToolView 생성/소멸

CRscToolView::CRscToolView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_tabViewCtrl.SetCallback(new TabViewCallback(this));
}

CRscToolView::~CRscToolView()
{
	delete m_tabViewCtrl.GetCallback();
}

BOOL CRscToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CRscToolView 그리기

void CRscToolView::OnDraw(CDC* /*pDC*/)
{
	CRscToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

void CRscToolView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CRscToolView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CRscToolView 진단

#ifdef _DEBUG
void CRscToolView::AssertValid() const
{
	CView::AssertValid();
}

void CRscToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRscToolDoc* CRscToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRscToolDoc)));
	return (CRscToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CRscToolView 메시지 처리기

int CRscToolView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	
	m_tabViewCtrl.Create( this);
	
	m_tabViewCtrl.AddView(RUNTIME_CLASS(PackScriptView), _T("PackScriptView"), 0);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(ItemView), _T("ItemView"), 1);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(FileCheckView), _T("MEDIA-QA FileCheckView"), 2);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(FileCheckView), _T("QA-LIVE FileCheckView"), 3);

	((FileCheckView*)m_tabViewCtrl.GetView(2))->InitCaption(PACKMODE_MEDIA_TO_QA_COPY);
	((FileCheckView*)m_tabViewCtrl.GetView(3))->InitCaption(PACKMODE_QA_TO_LIVE_PACK);

//	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test2"), 101);
	
	

/*
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test2"), 101);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test3"), 102);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test4"), 103);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test5"), 104);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test6"), 105);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test7"), 106);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test8"), 107);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test9"), 108);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test10"), 109);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test11"), 110);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test12"), 111);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test13"), 112);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test14"), 113);
	m_tabViewCtrl.AddView(RUNTIME_CLASS(CView1), _T("Test15"), 114);
*/
	return 0;
}


void CRscToolView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_tabViewCtrl.Embed_OnSize(cx, cy);
	// TODO: Add your message handler code here
}

void CRscToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	GetMainFrame()->SetViews((PackScriptView*)m_tabViewCtrl.GetView(0), (ItemView*)m_tabViewCtrl.GetView(1));

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}
