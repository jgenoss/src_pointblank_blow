#include "stdafx.h"
#include "i3Encrypter.h"

#include "MainFrm.h"

#include "i3EncrypterDoc.h"
#include "i3EncrypterView.h"

#include <algorithm>
#include <cctype>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(i3EncrypterDoc, CDocument)

BEGIN_MESSAGE_MAP(i3EncrypterDoc, CDocument)
END_MESSAGE_MAP()

i3EncrypterDoc::i3EncrypterDoc()
{
}

i3EncrypterDoc::~i3EncrypterDoc()
{
}

BOOL	i3EncrypterDoc::OnNewDocument()
{
	if( !CDocument::OnNewDocument() )
		return FALSE;

	if( NULL != GetActiveView() )
	{
		CEdit& editCtrl = GetActiveView()->GetEditCtrl();
		editCtrl.SetWindowText( _T("") );
	}
	
	return TRUE;
}


void	i3EncrypterDoc::Serialize(CArchive& ar)
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

BOOL	i3EncrypterDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if( FALSE == CDocument::OnOpenDocument(lpszPathName) )
	{
		return FALSE;
	}

	if( NULL == lpszPathName )
	{
		return FALSE;
	}

	std::wstring filename, ext, dir;
	if( FALSE == SplitFileName( lpszPathName, filename, dir, ext) )
	{
		return FALSE;
	}
	
	if( L"sif" != ext )
	{
		AfxMessageBox( _T("sif 파일이 아닙니다.") );
		return FALSE;
	}

	bool isEncrypted = false;	
	
	if( L"_en" == filename.substr( filename.length() -3 ) )
	{
		isEncrypted = true;
	}

	if( false == m_enc.Load( lpszPathName, isEncrypted ) )
	{
		return FALSE;
	}

	if( GetActiveView() )
	{
		GetActiveView()->GetEditCtrl().SetWindowText( m_enc.GetText() );
	}

	return TRUE;
}

BOOL	i3EncrypterDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	//return CDocument::OnSaveDocument(lpszPathName);

	std::wstring filename, ext, dir;
	if( FALSE == SplitFileName( lpszPathName, filename, dir, ext) )
	{
		return FALSE;
	}

	if( L"_en" == filename.substr( filename.length() -3 ) )
	{
		filename = filename.substr( 0, filename.length() -3 );
	}

	
	if( GetActiveView() )
	{
		CString str;
		GetActiveView()->GetEditCtrl().GetWindowText( str );
		m_enc.SetText( str.GetString() );
	}

	std::wstring path;

	path = (dir + L"\\" + filename + L"." + ext);
	if( false == m_enc.Save( path, false ) )
	{
		return FALSE;
	}

	path = (dir + L"\\" + filename + L"_En." + ext);
	if( false == m_enc.Save( path, true ) )
	{
		return FALSE;
	}

	SetModifiedFlag( FALSE );
	return TRUE;
}

i3EncrypterView* i3EncrypterDoc::GetActiveView()
{
	CMainFrame* pMain = static_cast< CMainFrame* >(theApp.m_pMainWnd);
	return pMain ? static_cast< i3EncrypterView* >(pMain->GetActiveView()) : NULL;
}

BOOL	i3EncrypterDoc::SplitFileName( const std::wstring& path, std::wstring &filename, std::wstring &dir, std::wstring &ext )
{
	if( path.empty() )
	{
		return FALSE;
	}

	int _pos = path.rfind( L'\\' );
	if( std::wstring::npos == _pos )
	{
		_pos = path.rfind( L'/' );
	}

	filename	= path.substr( _pos +1, path.length() - (_pos +1) );
	dir			= path.substr( 0, _pos );

	_pos = filename.rfind( L'.' );

	if( std::wstring::npos != _pos )
	{
		ext			= filename.substr( _pos +1, filename.length() - (_pos+1) );
		filename	= filename.substr( 0, _pos );
	}

	std::transform( dir.begin(), dir.end(), dir.begin(),				std::tolower );
	std::transform( ext.begin(), ext.end(), ext.begin(),				std::tolower );
	std::transform( filename.begin(), filename.end(), filename.begin(), std::tolower );

	return TRUE;
}
