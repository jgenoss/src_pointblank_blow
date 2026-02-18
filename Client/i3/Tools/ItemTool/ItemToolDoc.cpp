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

// ItemToolDoc.cpp : CItemToolDoc 클래스의 구현
//

#include "stdafx.h"
#include "ItemTool.h"

#include "ItemToolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CItemToolDoc

IMPLEMENT_DYNCREATE(CItemToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CItemToolDoc, CDocument)
END_MESSAGE_MAP()


// CItemToolDoc 생성/소멸

CItemToolDoc::CItemToolDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CItemToolDoc::~CItemToolDoc()
{
}

BOOL CItemToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CItemToolDoc serialization

void CItemToolDoc::Serialize(CArchive& ar)
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


// CItemToolDoc 진단

#ifdef _DEBUG
void CItemToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CItemToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CItemToolDoc 명령
