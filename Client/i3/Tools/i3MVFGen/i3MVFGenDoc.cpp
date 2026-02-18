// i3MVFGenDoc.cpp : Ci3MVFGenDoc 클래스의 구현
//

#include "stdafx.h"
#include "i3MVFGen.h"

#include "i3MVFGenDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3MVFGenDoc

IMPLEMENT_DYNCREATE(Ci3MVFGenDoc, CDocument)

BEGIN_MESSAGE_MAP(Ci3MVFGenDoc, CDocument)
END_MESSAGE_MAP()


// Ci3MVFGenDoc 생성/소멸

Ci3MVFGenDoc::Ci3MVFGenDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

Ci3MVFGenDoc::~Ci3MVFGenDoc()
{
}

BOOL Ci3MVFGenDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// Ci3MVFGenDoc serialization

void Ci3MVFGenDoc::Serialize(CArchive& ar)
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


// Ci3MVFGenDoc 진단

#ifdef _DEBUG
void Ci3MVFGenDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ci3MVFGenDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Ci3MVFGenDoc 명령
