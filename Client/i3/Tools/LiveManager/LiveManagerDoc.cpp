
// LiveManagerDoc.cpp : CLiveManagerDoc 클래스의 구현
//

#include "stdafx.h"
#include "LiveManager.h"

#include "LiveManagerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLiveManagerDoc

IMPLEMENT_DYNCREATE(CLiveManagerDoc, CDocument)

BEGIN_MESSAGE_MAP(CLiveManagerDoc, CDocument)
END_MESSAGE_MAP()


// CLiveManagerDoc 생성/소멸

CLiveManagerDoc::CLiveManagerDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CLiveManagerDoc::~CLiveManagerDoc()
{
}

BOOL CLiveManagerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CLiveManagerDoc serialization

void CLiveManagerDoc::Serialize(CArchive& ar)
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


// CLiveManagerDoc 진단

#ifdef _DEBUG
void CLiveManagerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLiveManagerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLiveManagerDoc 명령
