// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MenutestView.cpp : CItemToolView 클래스의 구현
//

#include "stdafx.h"
#include "ItemTool.h"

#include "ItemToolDoc.h"
#include "ItemToolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CItemToolView

IMPLEMENT_DYNCREATE(CItemToolView, CView)

BEGIN_MESSAGE_MAP(CItemToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CItemToolView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CItemToolView 생성/소멸

CItemToolView::CItemToolView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CItemToolView::~CItemToolView()
{
}

BOOL CItemToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CItemToolView 그리기

void CItemToolView::OnDraw(CDC* /*pDC*/)
{
	CItemToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CItemToolView 인쇄


void CItemToolView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CItemToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CItemToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CItemToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CItemToolView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CItemToolView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CItemToolView 진단

#ifdef _DEBUG
void CItemToolView::AssertValid() const
{
	CView::AssertValid();
}

void CItemToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CItemToolDoc* CItemToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CItemToolDoc)));
	return (CItemToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CItemToolView 메시지 처리기
