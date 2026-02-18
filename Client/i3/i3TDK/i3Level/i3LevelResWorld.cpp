#include "stdafx.h"
#include "i3LevelResWorld.h"
#include "i3LevelWorld.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelResWorld);

i3LevelResWorld::i3LevelResWorld(void)
{
	m_ResType		= I3LV_TYPE_WORLD;
}

i3LevelResWorld::~i3LevelResWorld(void)
{
	I3_SAFE_RELEASE( m_pWorld);
}

void i3LevelResWorld::CreateEmptyWorld(void)
{
	m_pWorld = i3World::new_object();

	addResState( I3LV_RES_STATE_MODIFIED | I3LV_RES_STATE_LOADED);
}

void i3LevelResWorld::setWorld( i3World * pWorld)
{
	I3_REF_CHANGE( m_pWorld, pWorld);
}

void	i3LevelResWorld::SetWorld( i3LevelResSceneGraph * pSg)
{
	// 이전 Ref Res를 제거한다.
	i3SceneGraphInfo * pInfo = m_pWorld->getWorld();
	
	if( pInfo != nullptr)
	{
		RemoveDependantResByName( pInfo->GetName());
	}

	m_pWorld->SetWorld( pSg->getSceneGraphInfo());

	m_RefResList.push_back( pSg);

	addResState( I3LV_RES_STATE_MODIFIED);
}

void	i3LevelResWorld::SetSky( i3LevelResSceneGraph * pSg)
{
	// 이전 Ref Res를 제거한다.
	i3SceneGraphInfo * pInfo = m_pWorld->getSky();
	
	if( pInfo != nullptr)
	{
		RemoveDependantResByName( pInfo->GetName());
	}

	m_pWorld->SetSky( pSg->getSceneGraphInfo());

	m_RefResList.push_back( pSg);

	addResState( I3LV_RES_STATE_MODIFIED);
}

void	i3LevelResWorld::SetWorldCollision( i3LevelResSceneGraph * pSg)
{
	// 이전 Ref Res를 제거한다.
	i3SceneGraphInfo * pInfo = m_pWorld->getWorldCollision();
	
	if( pInfo != nullptr)
	{
		RemoveDependantResByName( pInfo->GetName());
	}

	m_pWorld->SetWorldCollision( pSg->getSceneGraphInfo());

	m_RefResList.push_back( pSg);

	addResState( I3LV_RES_STATE_MODIFIED);
}

void	i3LevelResWorld::SetHitCollision( i3LevelResSceneGraph * pSg)
{
	// 이전 Ref Res를 제거한다.
	i3SceneGraphInfo * pInfo = m_pWorld->getHitCollision();
	
	if( pInfo != nullptr)
	{
		RemoveDependantResByName( pInfo->GetName());
	}

	m_pWorld->SetHitCollision( pSg->getSceneGraphInfo());

	m_RefResList.push_back( pSg);

	addResState( I3LV_RES_STATE_MODIFIED);
}

void	i3LevelResWorld::SetLightVolumeCollision( i3LevelResSceneGraph * pSg)
{
	// 이전 Ref Res를 제거한다.
	i3SceneGraphInfo * pInfo = m_pWorld->getLightVolumeCollision();
	
	if( pInfo != nullptr)
	{
		RemoveDependantResByName( pInfo->GetName());
	}

	m_pWorld->SetLightVolumeCollision( pSg->getSceneGraphInfo());

	m_RefResList.push_back( pSg);

	addResState( I3LV_RES_STATE_MODIFIED);
}

bool i3LevelResWorld::OnValidateBrief( bool bForce)
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

bool i3LevelResWorld::OnValidate( i3LevelScene * pScene, bool bForce)
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

	I3_SAFE_RELEASE( m_pWorld);

	m_pWorld = (i3World *) file.getKeyObject();
	I3_MUST_ADDREF(m_pWorld);

	m_pWorld->SetExternRes( TRUE);
	
	addResState( I3LV_RES_STATE_LOADED);

	return true;
}

bool i3LevelResWorld::OnSerialize(void)
{
	i3ResourceFile file;

	if( m_pWorld == nullptr)
		return true;

	file.setKeyObject( m_pWorld);

	char szFull[MAX_PATH];

	sprintf( szFull, "%s/%s", g_szResPath, getPath());

	if( file.Save( szFull) == STREAM_ERR)
		return false;

	return true;
}

i3GameRes *	 i3LevelResWorld::CreateGameRes(void)
{
	i3GameResWorld * pRes;

	pRes = i3GameResWorld::new_object();
	pRes->setWorld( m_pWorld);

	return pRes;
}

i3LevelElement3D *	i3LevelResWorld::CreateInstance( i3LevelScene * pScene)
{
	i3LevelWorld * pWorld;

	OnValidate( pScene, false);

	pWorld = i3LevelWorld::new_object_ref();
	pWorld->BindRes( this);

	pScene->AddElement( pWorld, "World");

	return pWorld;
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)						// [test required] 패킹사이즈 누락으로 추가함. 2017.02.14. soon9
#endif

namespace pack
{
	struct RES_WORLD
	{
		UINT8			m_ID[4] = { 'W', 'L', 'D', '1' };
		UINT32			pad[16] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3LevelResWorld::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::RES_WORLD data;

	Result = i3LevelRes::OnSave( pResFile);
	I3_CHKIO( Result);


	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3LevelResWorld::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::RES_WORLD data;

	Result = i3LevelRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return Result;
}
