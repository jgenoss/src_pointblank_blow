// i3RSCGenDoc.cpp : Ci3RSCGenDoc 클래스의 구현
//

#include "stdafx.h"
#include "i3RSCGen.h"

#include "i3RSCGenDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3RSCGenDoc

IMPLEMENT_DYNCREATE(Ci3RSCGenDoc, CDocument)

BEGIN_MESSAGE_MAP(Ci3RSCGenDoc, CDocument)
END_MESSAGE_MAP()


// Ci3RSCGenDoc 생성/소멸

Ci3RSCGenDoc::Ci3RSCGenDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

Ci3RSCGenDoc::~Ci3RSCGenDoc()
{
}

BOOL Ci3RSCGenDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// Ci3RSCGenDoc serialization

void Ci3RSCGenDoc::Serialize(CArchive& ar)
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


// Ci3RSCGenDoc 진단

#ifdef _DEBUG
void Ci3RSCGenDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ci3RSCGenDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Ci3RSCGenDoc 명령
