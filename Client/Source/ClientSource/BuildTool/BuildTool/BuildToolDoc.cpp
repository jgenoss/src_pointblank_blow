
// BuildToolDoc.cpp : CBuildToolDoc 클래스의 구현
//

#include "stdafx.h"
#include "BuildTool.h"

#include "BuildToolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBuildToolDoc

IMPLEMENT_DYNCREATE(CBuildToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CBuildToolDoc, CDocument)
END_MESSAGE_MAP()


// CBuildToolDoc 생성/소멸

CBuildToolDoc::CBuildToolDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CBuildToolDoc::~CBuildToolDoc()
{
}

BOOL CBuildToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.
	SetTitle(L"PointBlank");

	return TRUE;
}




// CBuildToolDoc serialization

void CBuildToolDoc::Serialize(CArchive& ar)
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


// CBuildToolDoc 진단

#ifdef _DEBUG
void CBuildToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBuildToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CBuildToolDoc 명령
