// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3Viewer2Doc.cpp : Ci3Viewer2Doc 클래스의 구현
//

#include "stdafx.h"
#include "i3Viewer2.h"
#include "i3Viewer2Doc.h"
#include "GlobalVar.h"

#include "MainFrm.h"

#include "i3Base/string/ext/extract_fileext.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3Viewer2Doc

IMPLEMENT_DYNCREATE(Ci3Viewer2Doc, CDocument)

BEGIN_MESSAGE_MAP(Ci3Viewer2Doc, CDocument)
END_MESSAGE_MAP()


static void NormalizeCmdArg( char *szOut, const char *szIn )
{
	while( *szIn )
	{
		if( *szIn != '"' )
		{
			*szOut = *szIn;
			szOut++;
		}
		szIn++;
	}
	*szOut = 0;
}
// Ci3Viewer2Doc 생성/소멸

Ci3Viewer2Doc::Ci3Viewer2Doc()
{
	m_pUI = NULL;
}

Ci3Viewer2Doc::~Ci3Viewer2Doc()
{
}

BOOL Ci3Viewer2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_szFileName[0] = 0;
	m_pUI = NULL;
	
	return TRUE;
}

void Ci3Viewer2Doc::OnCloseDocument()
{
	CloseI3S();

	I3_SAFE_RELEASE( m_pUI);
	
	i3TDKDlgChangeTex* pDlg = ((CMainFrame*)AfxGetMainWnd())->GetTDKDlgChangeTex();
	pDlg->SetFilenameFromFileDialog("");
		
	CDocument::OnCloseDocument();
}


BOOL Ci3Viewer2Doc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	LoadFile( lpszPathName);

	return TRUE;
}

BOOL Ci3Viewer2Doc::OnSaveDocument(LPCTSTR lpszPathName)
{
	SaveI3S( lpszPathName);

	SetModifiedFlag(FALSE);     // back to unmodified

	return TRUE;
}


// Ci3Viewer2Doc 진단

#ifdef _DEBUG
void Ci3Viewer2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ci3Viewer2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Ci3Viewer2Doc 명령

void Ci3Viewer2Doc::_setCaption( const char * pszPath)
{
	if( pszPath == NULL )
	{
		strcpy( m_szFileName, "" );

		SetTitle( "(없음)");

		SetPathName( pszPath);
	}
	else if( pszPath != m_szFileName)
	{
		strcpy( m_szFileName, pszPath ); 

		if( pszPath[0] != 0)
			SetPathName( pszPath);
	}

	if( m_szFileName[0] == 0 )
	{
		SetTitle( "i3Viewer2 version 2.0" );
	}
	else
	{
		char szName[MAX_PATH << 1];
		sprintf( szName, "i3Viewer2 version 2.0 - %s", m_szFileName);
		SetTitle( szName );
	}
}

bool Ci3Viewer2Doc::CloseI3S(void)
{
	g_pFramework->CloseI3S();

	i3TDK::Update( AfxGetMainWnd()->m_hWnd, I3_TDK_UPDATE_SCENE, NULL, i3SceneGraphInfo::static_meta());

	_setCaption( "");

	return true;
}


bool Ci3Viewer2Doc::LoadI3S( const char * pszPath)
{
	char	szPath[ MAX_PATH];

	NormalizeCmdArg( szPath, pszPath);

	if( g_pFramework->LoadI3S( szPath) == false)
	{
		NotifyBox( AfxGetMainWnd()->m_hWnd, "%s 파일을 열 수 없습니다.", pszPath);
		return false;
	}

	i3TDK::Update( AfxGetMainWnd()->m_hWnd, I3_TDK_UPDATE_SCENE, g_pFramework->getSgInfo());

	_setCaption( szPath);

	g_pOption->addMRU( szPath);
	
	//
	i3TDKDlgChangeTex* pDlg = ((CMainFrame*)AfxGetMainWnd())->GetTDKDlgChangeTex();

	i3SceneGraphInfo * pInfo = g_pFramework->getSgInfo();

	pDlg->SetFilenameAndSgInfo( pszPath, pInfo );
	

	//m_dlgChangeTex
	//
	return true;
}

bool Ci3Viewer2Doc::SaveI3S( const char * pszPath)
{
	if( g_pFramework->getSgInfo() == NULL)
		return false;

	if( g_pFramework->SaveI3S( pszPath) == false)
	{
		NotifyBox( AfxGetMainWnd()->m_hWnd, "%s 파일을 저장할 수 없습니다.", pszPath);
		return false;
	}
	
	_setCaption( pszPath);

	return true;
}

bool Ci3Viewer2Doc::LoadAnimation( const char * pszPath)
{
	char	szPath[ MAX_PATH];

	NormalizeCmdArg( szPath, pszPath);

	if( g_pFramework->LoadAnimation( (const char*) szPath) == false)
	{
		NotifyBox( AfxGetMainWnd()->m_hWnd, "Body가 없는 Sg. 이거나 파일을 열 수 없습니다.");
		return false;
	}
	
	return true;
}

enumEngineFileType Ci3Viewer2Doc::_checkFileName( char * FileName)
{
	char Ext[32];

//	i3String::SplitFileExt(FileName, Ext, sizeof(Ext) - 1);
	i3::extract_fileext(FileName, Ext);

	if( i3::generic_is_iequal( (char *)Ext, (const char *)"i3s")  ||
		i3::generic_is_iequal( (char *)Ext, (const char *)"I3S") )
		return I3FILE_TYPE_SG;

	if( i3::generic_is_iequal( (char*) Ext, (const char*)"i3a")  ||
		i3::generic_is_iequal( (char*) Ext, (const char*)"I3A") )
		return I3FILE_TYPE_ANIM;

	if( i3::generic_is_iequal( (char*) Ext, (const char*)"i3Uie")  ||
		i3::generic_is_iequal( (char*) Ext, (const char*)"I3UIE") )
		return I3FILE_TYPE_UI;

	return I3FILE_TYPE_UNKNOWN;
}

bool Ci3Viewer2Doc::LoadFile( const char * pszPath)
{
	char szPath[ MAX_PATH];

	NormalizeCmdArg( szPath, pszPath);
	enumEngineFileType ck = _checkFileName( szPath );

	switch( ck)
	{
	case I3FILE_TYPE_SG :
		//여기부터 실제 적인 씬 파일 로드 입니다. 
		{
		CloseI3S();
		
		i3TDKDlgChangeTex* pDlg = ((CMainFrame*)AfxGetMainWnd())->GetTDKDlgChangeTex();
		pDlg->SetFilenameFromFileDialog(pszPath);
		
		return LoadI3S( szPath ); 
		}

	case I3FILE_TYPE_ANIM :
		return LoadAnimation( szPath);

	case I3FILE_TYPE_UI :
		return LoadUI( szPath);

	default:
		AfxGetMainWnd()->MessageBox( "not support file type!", "Error" );
		break;
	}

	return false;
}



bool Ci3Viewer2Doc::CloseUI(void)
{
	i3UIManager * pManager = i3UI::getManager();
	I3ASSERT( pManager != NULL);

	if( m_pUI != NULL)
	{
		m_pUI->Detach();

		pManager->CloseVirtualTexture();
		pManager->setUILibrary( NULL);

		I3_SAFE_RELEASE( m_pUI);
	}

	return true;
}

bool Ci3Viewer2Doc::LoadUI(const char * pszPath)
{
	i3UIManager * pManager = i3UI::getManager();
	I3ASSERT( pManager != NULL);

	i3UIScene * pUI = pManager->LoadUIScene((char*)pszPath);

	I3_REF_CHANGE( m_pUI, pUI);

	if( m_pUI != NULL)
	{
		m_pUI->Attach(true, AfxGetApp()->m_hInstance);
	}

	return true;
}