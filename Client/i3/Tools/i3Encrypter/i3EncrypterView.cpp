#include "stdafx.h"
#include "i3Encrypter.h"

#include "i3EncrypterDoc.h"
#include "i3EncrypterView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(i3EncrypterView, CEditView)

BEGIN_MESSAGE_MAP(i3EncrypterView, CEditView)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


i3EncrypterView::i3EncrypterView()
{
}

i3EncrypterView::~i3EncrypterView()
{
}

BOOL i3EncrypterView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CEditView::PreCreateWindow(cs);
}

void i3EncrypterView::OnDraw(CDC* /*pDC*/)
{
	i3EncrypterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

}

void i3EncrypterView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void i3EncrypterView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


#ifdef _DEBUG
i3EncrypterDoc* i3EncrypterView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(i3EncrypterDoc)));
	return (i3EncrypterDoc*)m_pDocument;
}
#endif //_DEBUG

int i3EncrypterView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles( TRUE );
	return 0;
}

void i3EncrypterView::OnDropFiles(HDROP hDropInfo)
{
	wchar_t wsz[ MAX_PATH ];
	::DragQueryFile( hDropInfo, 0, wsz, MAX_PATH );

	theApp.OpenDocumentFile( wsz );

	//CEditView::OnDropFiles(hDropInfo);
}
