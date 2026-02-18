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

// i3LevelEditorDoc.cpp : implementation of the Ci3LevelDesignDoc class
//

#include "pch.h"
#include "i3LevelDesign.h"

#include "i3LevelDesignDoc.h"
#include "GlobalVariable.h"
#include "Dlg_LoadError.h"

#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/remove_ext.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3LevelDesignDoc

IMPLEMENT_DYNCREATE(Ci3LevelDesignDoc, CDocument)

BEGIN_MESSAGE_MAP(Ci3LevelDesignDoc, CDocument)
END_MESSAGE_MAP()


// Ci3LevelDesignDoc construction/destruction

Ci3LevelDesignDoc::Ci3LevelDesignDoc()
{
	m_szPath[0] = 0;
}

Ci3LevelDesignDoc::~Ci3LevelDesignDoc()
{
}

// Ci3LevelDesignDoc serialization

void Ci3LevelDesignDoc::Serialize(CArchive& ar)
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


// Ci3LevelDesignDoc diagnostics

#ifdef _DEBUG
void Ci3LevelDesignDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ci3LevelDesignDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void	Ci3LevelDesignDoc::CloseLevel(void)
{
	if(theApp.GetMainFrm() )
		theApp.GetMainFrm()->m_PanelTransform.ClearSelList();

	i3Level::SetScene( NULL);

	i3LevelUndo::Close();
}

void	Ci3LevelDesignDoc::NewLevel(void)
{
	CloseLevel();

	// 새로운 Scene의 생성
	{
		i3LevelScene * pNewScene;

		pNewScene = i3LevelScene::new_object_ref();
		pNewScene->Create();
		pNewScene->SetName( "Test");

		pNewScene->setSnap_Position( g_pProfile->m_PosSnap);
		pNewScene->setSnap_Rotation( g_pProfile->m_RotateSnap);
		pNewScene->setSnap_Scaling( g_pProfile->m_ScaleSnap);

		i3Level::SetScene( pNewScene);
	}

	_SetPath( "");
}

void Ci3LevelDesignDoc::_SetPath( const char * pszPath)
{
	if( m_szPath != pszPath)
	{
		i3::safe_string_copy( m_szPath, pszPath, MAX_PATH );
	}

	if( m_szPath[0] != 0)
	{
		SetPathName( m_szPath);
	}
}


bool	Ci3LevelDesignDoc::LoadLevel( const char * pszPath)
{
	CloseLevel();

	INT32 i;
	i3PersistantElement * pObj;
	i3LevelScene * pScene = NULL;
	char szExt[32];

//	i3String::SplitFileExt( pszPath, szExt, sizeof(szExt) - 1);
	i3::extract_fileext( pszPath, szExt);

	if( i3::generic_is_iequal( szExt, "XML") )
	{
		i3XMLFile file;

		if( file.Load( pszPath) == false)
		{
			DebugBox( AfxGetMainWnd()->m_hWnd, "%s : %s", pszPath, i3Error::getMsg( file.getLastError()));
			return false;
		}

		pScene = (i3LevelScene *) file.getKeyObject();
		pScene->AddRef();
	}
	else
	{
		i3ResourceFile file;

		if( file.Load( pszPath) == STREAM_ERR)
		{
			DebugBox( NULL, "%s 프로젝트 파일이 안열려...나보고 어쩌라고~", pszPath);
			return false;
		}

		for( i = 0; i < file.GetObjectCount(); i++)
		{
			pObj = file.GetObject( i);

			if( i3::same_of<i3LevelScene*>(pObj)) //->IsExactTypeOf( i3LevelScene::static_meta()))
			{
				pScene = (i3LevelScene *) pObj;
				pScene->AddRef();
				break;
			}
		}
	}
	
	if( pScene == NULL)
	{
		DebugBox( NULL, "제대로 된 게 아니구만. 프로젝트 설정이 없네...");
		return false;
	}

	pScene->setSnap_Position( g_pProfile->m_PosSnap);
	pScene->setSnap_Rotation( g_pProfile->m_RotateSnap);
	pScene->setSnap_Scaling( g_pProfile->m_ScaleSnap);


	i3Level::SetScene( pScene);

	pScene->Release();

	_SetPath( pszPath);

	CheckForLoadingError();

	//Post Process 처리
	//각 맵별로 들어가있는 PostProcess.xml을 찾아 적용한다.
	char conv[MAX_PATH];
	i3::generic_string_copy(conv, pszPath);
	i3::remove_ext(conv);
	
	//Post Process 파일 명은 '월드_PostProcess.xml' 형식.
	i3::generic_string_cat(conv, "_PostProcess.xml");
	
	if(i3System::IsFile(conv))
	{
		//존재한다면 이 파일을 로딩한다.
		i3XMLFile xmlFile;
		if(xmlFile.Load(conv))
		{
			i3PostProcessManager * pManager = g_pViewerCtrl->getViewer()->GetPostProcessManager();
			pManager->LoadPostProcessFromXML(&xmlFile);
		}
	}

	// Default Layer
	i3LevelLayer * pLayer = i3LevelLayer::new_object_ref();

	pLayer->SetName("HitMap");

	g_pScene->addLayer(pLayer);

	return true;
}


bool	Ci3LevelDesignDoc::SaveLevel( const char * pszPath)
{
	i3::vector<i3PersistantElement*> ObjList;
	bool bXML = false;

	g_pScene->PrepareSerialize();

	g_pResDB->FlushDB();

	{
		char szPath[MAX_PATH], szName[MAX_PATH];

		strcpy( szPath, pszPath);

//		i3String::SplitFileName( szPath, szName, FALSE);
		i3::extract_filetitle(szPath, szName);

		g_pScene->SetName( szName);

//		i3String::SplitFileExt( szPath, szName, sizeof(szName) - 1);
		i3::extract_fileext(szPath, szName);

		if( i3::generic_is_iequal( szName, "XML") )
			bXML = true;
	}

	ObjList.push_back( g_pScene);

	if( bXML == false)
	{
		i3ResourceFile file;

		file.SetObjects( ObjList);

		if( file.Save( pszPath) == STREAM_ERR)
		{
			DebugBox( NULL, "%s 프로젝트 파일을 저장할 수 없다....워쪄~!", pszPath);
			return false;
		}
	}
	else
	{
		i3XMLFile file;

		file.setKeyObject( g_pScene);

		if( file.Save( pszPath) == false)
		{
			DebugBox( AfxGetMainWnd()->m_hWnd, "%s 프로젝트 파일을 저장할 수 없습니다.", pszPath);
			return false;
		}
	}

	_SetPath( pszPath);

	{
		char conv[MAX_PATH];
		i3::generic_string_copy(conv, pszPath);
		i3::remove_ext(conv);

		i3::generic_string_cat(conv, "_PostProcess.xml");

		i3XMLFile xmlFile;

		i3PostProcessManager * pManager = g_pViewerCtrl->getViewer()->GetPostProcessManager();
		pManager->SavePostProcessToXML(&xmlFile);

		bool bResult = xmlFile.Save(conv);

		if(!bResult)
			return false;
	}


	return true;
}

// Ci3LevelDesignDoc commands

BOOL Ci3LevelDesignDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_szPath[0] = 0;

	NewLevel();

	return TRUE;
}


BOOL Ci3LevelDesignDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if( LoadLevel( lpszPathName))
	{
		strcpy( m_szPath, lpszPathName);
	}

	SetTitle( m_szPath);

	return TRUE;
}

BOOL Ci3LevelDesignDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	char * pszPath = (char *)lpszPathName;
	char conv[MAX_PATH];

	// 파일의 확장자가 기존의 i3LevelDesign이라면 XML로 변경해준다.
	{
		char szExt[128];

	//	i3String::SplitFileExt( lpszPathName, szExt, sizeof(szExt) - 1);
		i3::extract_fileext(lpszPathName, szExt);

		if( i3::generic_is_iequal( szExt, "i3LevelDesign") )
		{
			i3::change_file_ext_copy( conv, lpszPathName, "XML" );

			pszPath = conv;
		}
	}

	SaveLevel( pszPath);

	return TRUE;
}

void Ci3LevelDesignDoc::OnCloseDocument(void)
{
	CloseLevel();

	CDocument::OnCloseDocument();
}

void Ci3LevelDesignDoc::CheckForLoadingError(void)
{
	i3LevelScene * pScene = i3Level::GetScene();
	i3::vector< i3LevelElement3D *> list;

	for( INT32 i = 0; i < pScene->GetElementCount(); i++)
	{
		i3LevelElement3D * pElm = pScene->GetElement( i);

		if( pElm->IsState( I3_LEVEL_STATE_FAILED_BINDRES))
		{
			list.push_back( pElm);
		}
	}

	if( list.size() > 0)
	{
		CDlg_LoadError dlg;

		dlg.Execute( AfxGetMainWnd(), list);
	}
}