// i3DbgViewerDoc.cpp : Ci3DbgViewerDoc 클래스의 구현
//

#include "stdafx.h"
#include "i3DbgViewer.h"

#include "i3DbgViewerDoc.h"
#include ".\i3dbgviewerdoc.h"
#include "GfxCmdCore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3DbgViewerDoc

IMPLEMENT_DYNCREATE(Ci3DbgViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(Ci3DbgViewerDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
END_MESSAGE_MAP()


// Ci3DbgViewerDoc 생성/소멸

Ci3DbgViewerDoc::Ci3DbgViewerDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

Ci3DbgViewerDoc::~Ci3DbgViewerDoc()
{
}

BOOL Ci3DbgViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// Ci3DbgViewerDoc serialization

void Ci3DbgViewerDoc::Serialize(CArchive& ar)
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


// Ci3DbgViewerDoc 진단

#ifdef _DEBUG
void Ci3DbgViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ci3DbgViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Ci3DbgViewerDoc 명령

//BOOL Ci3DbgViewerDoc::OnOpenDocument(LPCTSTR lpszPathName)
//{
//	if (!CDocument::OnOpenDocument(lpszPathName))
//		return FALSE;
//
//	// TODO:  Add your specialized creation code here
//
//	return TRUE;
//}

void Ci3DbgViewerDoc::OnFileOpen()
{
	CString FileName; 
	CFileDialog	Dlg( TRUE, "i3DbgLog", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Gfx Debug Log File(*.i3DbgLog)|*.i3DbgLog||", AfxGetMainWnd());

	if( Dlg.DoModal() != IDOK)return;

	FileName = Dlg.GetPathName();

	{
		// 파일의 내용을 한꺼번에 읽어 들인다.
		char * pTemp;
		UINT32 BuffSz;
		i3FileStream stream;
		
		if( stream.Open( LPCTSTR( FileName), STREAM_READ) == FALSE)
		{
			AfxMessageBox( "파일을 열 수 없습니다", MB_OK);
			return;
		}

		if( stream.GetSize() <= 0)
		{
			AfxMessageBox( "파일의 크기가 0입니다.", MB_OK);
			stream.Close();
			return;
		}

		pTemp = new char[ stream.GetSize()];
		if( pTemp == NULL)
		{
			AfxMessageBox( "메모리가 부족합니다.", MB_OK);
			stream.Close();
			return;
		}

		if( stream.Read( pTemp, stream.GetSize()) == STREAM_ERR)
		{
			AfxMessageBox( "파일을 읽어들일 수 없습니다.", MB_OK);
			delete pTemp;
			stream.Close();
			return;
		}

		stream.Close();

		// Gfx Command를 위한 버퍼의 크기를 확인한다.
		{
			i3NullStream strm;
			GfxCmdCore core;

			core._ParseLog( &strm, pTemp, &m_BoundMin, &m_BoundMax);

			BuffSz = strm.GetSize();
			I3TRACE( "Buffer Size : %d\n", BuffSz);
		}

		// 실질적인 Buffer의 생성.
		{
			GfxCmdCore core;

			m_Buff.Create( NULL, STREAM_WRITE | STREAM_READ, BuffSz);

			core._ParseLog( &m_Buff, pTemp, &m_BoundMin, &m_BoundMax);
		}

		delete pTemp;
	}

	this->UpdateAllViews( NULL);
}
