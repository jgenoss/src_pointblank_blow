#include "stdafx.h"
#include "UIProject.h"
#include "GlobalVariables.h"
#include "MainFrm.h"
/*
I3_CLASS_INSTANCE( CUIProject); //, i3NamedElement);

CUIProject::CUIProject(void)
{
	m_nVersion				= 1;
	m_pScene				= NULL;

	m_nViewWidth			= 1024;
	m_nViewHeight			= 768;
	m_pLibrary				= NULL;
}

CUIProject::~CUIProject(void)
{
	I3_SAFE_RELEASE( m_pScene);
	I3_SAFE_RELEASE( m_pLibrary);

	if( g_pFramework != NULL)
	{
		i3Framework::SetCurrentFocus( NULL);
	}
}

void CUIProject::Create(void)
{
	m_pScene = i3UIScene::new_object();
}

#if defined( I3_COMPILER_VC )
#pragma pack( push, 4)
#endif

typedef struct _tagi3UIProject
{
	UINT8		m_ID[4];
	OBJREF		m_pScene;
	OBJREF		m_pLibrary;
	UINT32		m_nViewWidth;
	UINT32		m_nViewHeight;
	UINT8		m_WorkingPath[MAX_PATH];
	UINT32		pad[12];
}I3_PERSIST_UIPROJECT;

#if defined( I3_COMPILER_VC )
#pragma pack( pop)
#endif

void	CUIProject::OnBuildObjectList( List * pList)
{
	if( m_pScene != NULL)
	{
		m_pScene->OnBuildObjectList( pList);
	}

	i3NamedElement::OnBuildObjectList( pList);
}

UINT32 CUIProject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	I3_PERSIST_UIPROJECT proj;

	Result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( Result);

	i3mem::FillZero( &proj, sizeof( proj));
	i3::snprintf( (char*)proj.m_ID, 4, "UIP1");

	proj.m_pRoot = (OBJREF)pResFile->GetObjectPersistID( m_pRoot);
	proj.m_pLibrary = (OBJREF)pResFile->GetObjectPersistID( m_pLibrary);

	proj.m_nViewWidth	= g_pViewer->GetViewWidth();
	proj.m_nViewHeight	= g_pViewer->GetViewHeight();

	Rc = pResFile->Write( &proj, sizeof( proj));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 CUIProject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	I3_PERSIST_UIPROJECT proj;

	Result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &proj, sizeof( proj));
	I3_CHKIO( Rc);
	Result += Rc;

	if( i3String::NCompare( (char*)proj.m_ID, "UIP1", 4) != NULL)
	{
		I3TRACE("not a valid signature!! CUIProject::OnLoad()");
		return STREAM_ERR;
	}

	if( proj.m_pScene > 0)
	{
		i3UIScene * pScene = (i3UIRoot*)pResFile->FindObjectByID( proj.m_pScene);
		I3ASSERT( i3UIScene != NULL);

		setScene( i3UIScene);
	}

	if( proj.m_pLibrary > 0)
	{
		i3UILibrary * m_pLibrary = (i3UILibrary*)pResFile->FindObjectByID( proj.m_pLibrary);
		I3ASSERT( m_pLibrary != NULL);

		SetLibrary( m_pLibrary);
	}

	m_nViewWidth = proj.m_nViewWidth;
	m_nViewHeight = proj.m_nViewHeight;

	return Result;
}

bool CUIProject::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	pXML->addAttr( "Version", m_nVersion);

	pXML->addAttr( "Scene", pFile->GetObjectPersistID( m_pRoot));

	pXML->addAttr( "ViewWidth", g_pViewer->GetViewWidth());
	pXML->addAttr( "ViewHeight", g_pViewer->GetViewHeight());

	return true;
}

bool CUIProject::checkSaveCondition(void)
{
	if( m_pLibrary == NULL)
	{
		MessageBox( NULL, "저장할 내용이 없습니다.", "알림", MB_OK);

		return false;
	}

	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
	if( pVTex == NULL)
	{
		MessageBox( NULL, "연결된 Virtual Texture가 없습니다. \n 프로젝트를 저장하기 위해서는, Virtual Texture File을 새로 만들거나, \n기존의 파일을 연결해야 합니다.", "알림", MB_OK);

		return false;
	}

	if( m_pLibrary->hasName() == false)
	{
		ExportTemplateLibrary();

		if( m_pLibrary->hasName() == false)
			return false;
	}

	return true;
}

bool CUIProject::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	pXML->getAttr( "Version", &m_nVersion);

	INT32 iVal;
	pXML->getAttr( "Scene", &iVal);
	if( iVal != 0)
	{
		i3UIScene * pScene = (i3UIScene*)pFile->FindObjectByID( (OBJREF) iVal);
		I3ASSERT( pScene != NULL);

		setScene( pScene);
	}

	pXML->getAttr( "ViewWidth", &m_nViewWidth);
	pXML->getAttr( "ViewHeight", &m_nViewHeight);

	return true;
}

void CUIProject::ImportTemplateLibrary(const char * pszLibPath)
{
	i3XMLFile File;

	if( pszLibPath != NULL)
	{
		char conv[MAX_PATH], szFull[MAX_PATH];

		if( i3::generic_string_size( i3ResourceFile::GetWorkingFolder()) == 0)
		{
			i3::string_ncopy_nullpad( szFull, pszLibPath, MAX_PATH -1);
		}
		else
		{
			i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), pszLibPath, conv);

			i3::snprintf( szFull, MAX_PATH, "%s/%s", i3ResourceFile::GetWorkingFolder(), conv);
		}

		if( File.Load(szFull))
		{
			i3UILibrary * pLibrary = (i3UILibrary*)File.getKeyObject();

			SetLibrary( pLibrary);
		}
	}
	else
	{
		TCHAR szFilters[] = "i3Uil 파일(*.i3Uil)|*.i3Uil|";

		CFileDialog fileDlg(TRUE, _T("i3Uil"), _T(""), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

		if( fileDlg.DoModal() == IDOK)
		{
			i3XMLFile File;
			if( File.Load((LPCTSTR)fileDlg.GetPathName()))
			{
				i3UILibrary * pLibrary = (i3UILibrary*)File.getKeyObject();

				SetLibrary( pLibrary);
			}
		}
	}
}

void CUIProject::ExportTemplateLibrary(const char * pszLibPath)
{
	i3XMLFile File;

	if( pszLibPath == NULL)
	{
		TCHAR szFilters[] = "i3Uil 파일(*.i3Uil)|*.i3Uil|";

		CFileDialog fileDlg(FALSE, _T("i3Uil"), _T("Noname"), OFN_HIDEREADONLY, szFilters);

		if( fileDlg.DoModal() == IDOK)
		{
			{
				char conv[MAX_PATH];
				i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), (LPCTSTR)fileDlg.GetPathName(), conv);

				m_pLibrary->SetName( conv);
			}

			File.setKeyObject( m_pLibrary);
			if( File.Save((LPCTSTR)fileDlg.GetPathName()))
			{
				I3TRACE("Export: %s Done\n", (LPCTSTR)fileDlg.GetPathName());
			}
		}
	}
	else
	{
		File.setKeyObject( m_pLibrary);
		if( File.Save(pszLibPath))
		{
			I3TRACE("Export: %s Done\n", pszLibPath);
		}
	}
}

void CUIProject::SaveTemplateLibrary(void)
{
	if( m_pLibrary == NULL)
		return;

	if( m_pLibrary->hasName())
	{
		i3XMLFile File;
		char conv[MAX_PATH], szFull[MAX_PATH];

		i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), m_pLibrary->GetName(), conv);
		i3::snprintf( szFull, MAX_PATH -1, "%s/%s", i3ResourceFile::GetWorkingFolder(), conv);

		File.setKeyObject( m_pLibrary);
		if( File.Save(szFull))
		{
			I3TRACE("Template Library Saved\n");
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN,"Couldn't save Template Library to %s\n", szFull);
		}
	}
	else
	{
		ExportTemplateLibrary();
	}
}
*/