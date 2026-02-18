
// ControlToolDoc.cpp : implementation of the CControlToolDoc class
//

#include "pch.h"
#include "ControlTool.h"

#include "ControlToolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CControlToolDoc

IMPLEMENT_DYNCREATE(CControlToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CControlToolDoc, CDocument)
END_MESSAGE_MAP()


// CControlToolDoc construction/destruction

CControlToolDoc::CControlToolDoc()
{
	// TODO: add one-time construction code here

}

CControlToolDoc::~CControlToolDoc()
{
}

BOOL CControlToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CControlToolDoc serialization

void CControlToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CControlToolDoc diagnostics

#ifdef _DEBUG
void CControlToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CControlToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CControlToolDoc commands
