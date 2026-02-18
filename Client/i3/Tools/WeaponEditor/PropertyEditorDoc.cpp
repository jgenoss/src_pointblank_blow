// PropertyEditorDoc.cpp : CPropertyEditorDoc 클래스의 구현
//

#include "stdafx.h"
#include "PropertyEditor.h"

#include "PropertyEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPropertyEditorDoc

IMPLEMENT_DYNCREATE(CPropertyEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CPropertyEditorDoc, CDocument)
END_MESSAGE_MAP()


// CPropertyEditorDoc 생성/소멸

CPropertyEditorDoc::CPropertyEditorDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CPropertyEditorDoc::~CPropertyEditorDoc()
{
}

BOOL CPropertyEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}

// CPropertyEditorDoc serialization

void CPropertyEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CPropertyEditorDoc 진단

#ifdef _DEBUG
void CPropertyEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPropertyEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPropertyEditorDoc 명령
