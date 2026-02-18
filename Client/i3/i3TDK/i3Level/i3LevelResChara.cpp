#include "stdafx.h"
#include "i3LevelResChara.h"
#include "i3LevelChara.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelResChara);

i3LevelResChara::i3LevelResChara(void)
{
	m_ResType		= I3LV_TYPE_CHARA;
}

i3LevelResChara::~i3LevelResChara(void)
{
	I3_SAFE_RELEASE( m_pChara);
}

void i3LevelResChara::setChara( i3Chara * pChara)
{
	I3_REF_CHANGE( m_pChara, pChara);
}

bool i3LevelResChara::OnValidateBrief( bool bForce)
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

	i3ResourceFile file;
	UINT32 i;
	I3_RESOURCE_FILE_HEAD_INFO * pHeader;
	
	if( file.LoadHeader( &strm) == STREAM_ERR)
	{
		return false;
	}

	pHeader = file.getHeader();

	m_LODCount = 0;
	m_AIStateCount = 0;
	m_AnimCount = 0;

	for( i = 0; i < pHeader->m_ObjectCount; i++)
	{
		i3::pack::RESOURCE_FILE_OBJECT_INFO2 * pObj = file.getObjectInfo( i);

		const i3::rc_string& strClass = file.GetStringTable()[ pObj->m_ClassNameIndex];

		//I3TRACE( "%s\n", pszClass);

		if( i3::generic_is_iequal( strClass, "i3LOD") )
			m_LODCount++;
		else if( i3::generic_is_iequal( strClass, "i3AIState") )
			m_AIStateCount++;
		else if( i3::generic_is_iequal( strClass, "i3Animation") )
			m_AnimCount++;
	}

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return true;
}

bool i3LevelResChara::OnValidate( i3LevelScene * pScene, bool bForce)
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

		I3_SAFE_RELEASE( m_pChara);
	}
	EndValidate();

	m_pChara = (i3Chara *) file.getKeyObject();
	I3_MUST_ADDREF(m_pChara);
	m_pChara->SetExternRes( TRUE);
	
	addResState( I3LV_RES_STATE_LOADED);

	return true;
}

i3GameRes *			i3LevelResChara::CreateGameRes(void)
{
	i3GameResChara * pRes;

	pRes = i3GameResChara::new_object();

	pRes->setChara( m_pChara);

	return pRes;
}

i3LevelElement3D *	i3LevelResChara::CreateInstance( i3LevelScene * pScene)
{
	i3LevelChara * pChara;

	OnValidate( pScene, false);

	pChara = i3LevelChara::new_object_ref();
	pChara->BindRes( this);

	pScene->AddElement( pChara, "Chara");

	return pChara;
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)			// [test required] 패킹사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct RES_CHARA
	{
		UINT8			m_ID[4] = { 'C', 'H', 'R', '1' };
		UINT32			m_LODCount = 0;
		UINT32			m_AIStateCount = 0;
		UINT32			m_AnimCount = 0;
		UINT32			pad[16] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3LevelResChara::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::RES_CHARA data;

	Result = i3LevelRes::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_LODCount = m_LODCount;
	data.m_AIStateCount = m_AIStateCount;
	data.m_AnimCount = m_AnimCount;

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3LevelResChara::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::RES_CHARA data;

	Result = i3LevelRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_LODCount = data.m_LODCount;
	m_AIStateCount = data.m_AIStateCount;
	m_AnimCount = data.m_AnimCount;

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return Result;
}
