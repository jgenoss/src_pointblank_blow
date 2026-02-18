#include "stdafx.h"
#include "i3LevelRes.h"
#include "i3LevelResEvent.h"
#include "i3LevelGlobalVariable.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/make_relative_path.h"

I3_CLASS_INSTANCE( i3LevelResEvent);

i3LevelResEvent::i3LevelResEvent(void)
{
	m_ResType = I3LV_TYPE_EVENT;
}

i3LevelResEvent::~i3LevelResEvent(void)
{
	I3_SAFE_RELEASE( m_pTimeSequence);
}

i3LevelElement3D *	i3LevelResEvent::CreateInstance( i3LevelScene * pScene)
{
	return nullptr;
}

bool i3LevelResEvent::OnValidateBrief( bool bForce)
{
	i3FileStream strm;

	if( bForce == false)
	{
		if( isBriefLoaded())
			return true;

		if( isLoaded())
			return true;
	}

	char szFull[MAX_PATH];

	getFullPath( szFull);

	if( strm.Open( szFull, STREAM_READ | STREAM_SHAREREAD) == FALSE)
		return false;

	strm.Close();

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return true;
}

bool i3LevelResEvent::OnValidate( i3LevelScene * pScene, bool bForce)
{
	if( bForce == false)
	{
		if( isLoaded())
			return true;
	}

	i3ResourceFile file;
	char szFull[MAX_PATH];

	BeginValidate();
	{
		getFullPath( szFull);

		if( file.Load( szFull) == STREAM_ERR)
			return false;
	}
	EndValidate();

	i3TimeEventGen * pEvent = (i3TimeEventGen*)file.getKeyObject();
	I3_REF_CHANGE( m_pTimeSequence, pEvent);
	
	addResState( I3LV_RES_STATE_LOADED);
	return true;
}

bool i3LevelResEvent::OnSerialize(void)
{
	i3ResourceFile file;

	if( m_pTimeSequence == nullptr)
		return true;

	file.setKeyObject( m_pTimeSequence);

	char szFull[MAX_PATH];

	sprintf( szFull, "%s/%s", g_szResPath, getPath());

	if( file.Save( szFull) == STREAM_ERR)
		return false;

	return true;
}

void i3LevelResEvent::Load( const char * pszPath)
{
	i3ResourceFile file;
	char szFull[ MAX_PATH];

	if( pszPath[1] == ':')
	{
		// 절대 경로
		sprintf_s( szFull, _countof(szFull)-1, "%s", pszPath);
	}
	else
	{
		// 상대 경로
		sprintf( szFull, "%s/%s", g_szResPath, pszPath);
	}

	if( file.Load( szFull) == STREAM_ERR)
		return;

	i3TimeEventGen * pEvent = (i3TimeEventGen *) file.getKeyObject();

	I3_REF_CHANGE( m_pTimeSequence, pEvent);

	{
		char szRelPath[ MAX_PATH];

		i3::make_relative_path( g_szResPath, szFull, szRelPath);

		setPath( szRelPath);
	}
}

i3GameRes * i3LevelResEvent::CreateGameRes(void)
{
	i3GameResEvent * pRes;

	pRes = i3GameResEvent::new_object();

	pRes->setTimeEvent( m_pTimeSequence);

	return pRes;
}

void i3LevelResEvent::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	I3ASSERT( pDest != nullptr);

	i3LevelRes::CopyTo( pDest, method);
	
	if( !i3::kind_of<i3LevelResEvent*>(pDest))
	{
		return ;
	}

	i3LevelResEvent * pAction = (i3LevelResEvent*)pDest;

	pAction->setTimeEvent( m_pTimeSequence);
}

void i3LevelResEvent::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pTimeSequence != nullptr)
	{
		m_pTimeSequence->OnBuildObjectList( List);
	}

	i3LevelRes::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct RES_ACTION
	{
		UINT8		nID[4] = { 'L', 'R', 'A', '1' };
		UINT32		nVersion = 1;
		OBJREF64	refTimeEvent = 0;
		UINT32		pad[12] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3LevelResEvent::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::RES_ACTION data;

	Result = i3LevelRes::OnSave( pResFile);
	I3_CHKIO( Result);

	
	data.refTimeEvent = (OBJREF64)pResFile->GetObjectPersistID( m_pTimeSequence);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3LevelResEvent::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::RES_ACTION data;

	Result = i3LevelRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( i3::generic_is_niequal( (char*)data.nID, "LRA1", 4) == false)
	{
		return STREAM_ERR;
	}

	I3ASSERT( data.nVersion == 1);

	if( data.refTimeEvent > 0)
	{
		i3TimeEventGen * pEvent = (i3TimeEventGen*)pResFile->FindObjectByID( (OBJREF)data.refTimeEvent);
		I3ASSERT( pEvent != nullptr);

		setTimeEvent( pEvent);
	}

	return Result;
}