#include "stdafx.h"
#include "i3LevelRes.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelRes);

i3Node * i3LevelRes::GetElementAttachNode( i3LevelScene * pScene)
{
	return nullptr;
}

void i3LevelRes::getFullPath( char * pszPath)
{
	sprintf( pszPath, "%s/%s", g_szResPath, getPath());
}

bool	i3LevelRes::IsLastestVersion(void)
{
	HANDLE hFind;
	WIN32_FIND_DATAA data;

	if( getPath()[0] == 0)
		return false;

	hFind = FindFirstFileA( getPath(), &data);
	if( hFind == INVALID_HANDLE_VALUE)
		return false;

	CloseHandle( hFind);

	INT64 date = ((INT64) data.ftLastWriteTime.dwHighDateTime << 32) | data.ftLastWriteTime.dwLowDateTime;

	return	date <= m_LastUpdateFileTime;
}

void		i3LevelRes::OnChangeScene( i3LevelScene * pScene)
{
}

i3LevelElement3D *	i3LevelRes::CreateInstance( i3LevelScene * pScene)
{
	return nullptr;
}

void	i3LevelRes::OnQueryExportPath( i3LevelScene * pScene, i3LevelElement3D * pObj, char * pszPath, UINT16 nBufSize)
{
	strncpy( pszPath, getPath(), nBufSize);
}

void	i3LevelRes::OnQueryDependancy( i3::vector<i3LevelRes*>& List)
{
	i3LevelRes * pRes;

	for(size_t i = 0; i < m_RefResList.size(); i++)
	{
		pRes = m_RefResList[i];

		pRes->OnQueryDependancy( List);
	}

	List.push_back(this);
}

bool	i3LevelRes::OnSerialize(void)
{
	return true;
}

void i3LevelRes::RemoveDependantResByName( const char * pszPath)
{

	i3LevelRes * pRes;

	for(size_t i = 0; i < m_RefResList.size(); i++)
	{
		pRes = m_RefResList[i];

		if( i3::generic_is_iequal( pRes->getPath(), pszPath) )
		{
			m_RefResList.erase(m_RefResList.begin() + i);
			return;
		}
	}
}

/////////////////////////
void	i3LevelRes::_ExternRefCallback( i3PersistantElement * pElm, const char * pszPath, void * pUserData)
{
	i3LevelRes * pThisRes = (i3LevelRes *) pUserData;

	if( !i3::kind_of< i3ResourceObject*>(pElm))
		return;

	i3ResourceObject * pResObj = (i3ResourceObject *) pElm;
	i3LevelRes * pNeedRes;

	char szName[ MAX_PATH];

	strncpy( szName, pResObj->GetName(), sizeof(szName) - 1);
	i3::make_unix_path( szName);

	pNeedRes = g_pResDB->FindResByPath( szName);
	
	if( pNeedRes == nullptr)
	{
		char szFull[ MAX_PATH];

		sprintf( szFull, "%s/%s", g_szResPath, szName);
		pNeedRes = g_pResDB->AddRes( szFull);

		if( pNeedRes == nullptr)
		{
			I3TRACE( "EXTERN REF : %s\n", pszPath);
			return;
		}
	}

	pThisRes->m_RefResList.push_back( pNeedRes);
}

void	i3LevelRes::BeginValidate(void)
{
	// Extern Ref.들을 확인하기 위해, Callback 함수를 등록한다.
	i3ResourceFile::SetExternalRefCallback( _ExternRefCallback, this);
}

void	i3LevelRes::EndValidate(void)
{
	i3ResourceFile::SetExternalRefCallback( nullptr, nullptr);
}

bool	i3LevelRes::OnExport( i3LevelScene * pScene, const char * pszPath)
{
	// 원본의 내용을 대상으로 복사하기...
	{
		char szSrc[ MAX_PATH];

		i3::snprintf( szSrc, MAX_PATH, "%s/%s", g_szResPath, getPath());

		if( i3::generic_is_iequal( szSrc, pszPath) == false)
		{
			if( CopyFileA( szSrc, pszPath, FALSE) == FALSE)
			{
				I3PRINTLOG(I3LOG_WARN,  "%s 파일을 복사하지 못했습니다", pszPath);
				return false;
			}
		}
	}

	// Update Date
	{
		WIN32_FIND_DATAA info;

		HANDLE hFind = FindFirstFileA( pszPath, &info);

		if( hFind == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		setLastExportFileTime( & info.ftLastWriteTime);

		FindClose( hFind);
	}

	return true;
}

i3GameRes * i3LevelRes::CreateGameRes(void)
{
	return nullptr;
}

void	i3LevelRes::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelPropertyContainer::CopyTo( pDest, method);

	if( i3::kind_of<i3LevelRes* >(pDest))
	{
		i3LevelRes * pRes = (i3LevelRes *) pDest;

		pRes->setPath( m_szPath);
		pRes->m_ResType		= getResType();
		pRes->m_ResState	= getResState();
	}
}

#if defined( I3_COMPILER_VC) && defined(PACKING_RIGHT)		// [test required] ! defined 문으로 인해 적용이 안되고 있었음. 2017.02.09 soon9
#pragma pack( push, 4)				
#endif

namespace pack
{
	struct Level_RESOURCE
	{
		char	m_szPath[256] = { 0 };
		UINT8	m_Reserved[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack( pop)
#endif

UINT32 i3LevelRes::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::Level_RESOURCE Data;

	Result = i3LevelPropertyContainer::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;
	
	strncpy( Data.m_szPath, getPath(), sizeof(Data.m_szPath) - 1);

	Rc = pStream->Write( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelRes::OnSave()", "Could not read resource information.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3LevelRes::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::Level_RESOURCE Data;

	Result = i3LevelPropertyContainer::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	Rc = pStream->Read( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelRes::OnLoad()", "Could not write resource information.");
		return STREAM_ERR;
	}
	Result += Rc;

	setPath( Data.m_szPath);

	return Result;
}
