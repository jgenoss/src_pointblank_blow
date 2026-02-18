#include "pch.h"
#include "UserMap.h"

I3_CLASS_INSTANCE( CUserMap, i3GameObj);

CUserMap::CUserMap(void)
{
	m_StageInfo.init();

	m_pWorld = NULL;
}

CUserMap::~CUserMap(void)
{
	I3_SAFE_RELEASE( m_pWorld);
}

UINT32 CUserMap::CalcBufferSize(void)
{
	UINT32 sz = 0;

	// Header
	sz += sizeof( USMAP::STAGE::INFO);

	// Objects
	sz += sizeof( USMAP::OBJECT::INFO) * getObjectCount();

	// Conditions
	sz += sizeof( USMAP::COND::INFO) * getStartConditionCount();
	sz += sizeof( USMAP::COND::INFO) * getConditionCount();
	sz += sizeof( USMAP::COND::INFO) * getRoundEndConditionCount();

	return sz;
}

BOOL CUserMap::Create( i3Node *pParentNode, BOOL bEnable)
{
	i3GameObj::Create( pParentNode, bEnable);

	_InitBlocks();
	_InitSky();

	m_pGrid = CGrid::NewObject();

	m_pGrid->Create();
	m_pGrid->SetSize( 100.0f, 100.0f);

	getAttachNode()->AddChild( m_pGrid->getSceneObject());

	// World
	{
		m_pWorld = CUserMap_World::NewObject();

		m_pWorld->Create();

		getAttachNode()->AddChild( m_pWorld->getSceneObject());
	}

	_GenGround( 30.0f, 30.0f);

	return TRUE;
}

void CUserMap::_InitBlocks(void)
{
	INT32 i;
	USMAP::BLOCK::INFO * pInfo;

	for( i = 0; i < USMAP::BLOCK::getCount(); i++)
	{
		pInfo = USMAP::BLOCK::getInfo( i);

		if( pInfo->m_pRes == NULL)
		{
			if( pInfo->m_szRes[0] != 0)
			{
				i3GameResObject * pRes = (i3GameResObject *) g_pFramework->FindResourceByPath( pInfo->m_szRes);
				I3ASSERT( pRes != NULL);

				pInfo->m_pRes = pRes;

				if( ((pInfo->m_EditStyle & USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX) == 0)&& (pInfo->m_pBlock == NULL))
				{
					i3Object * pObj = pRes->getGameObject();
					i3SceneGraphInfo * pSgInfo = pObj->getSceneGraphInfo( 0);

					// Block
					CUserMap_Block * pBlock = CUserMap_Block::NewObject();

					pBlock->Create(  pSgInfo->getInstanceSg());

					pInfo->m_pBlock = pBlock;
				}
			}
		}

		if( pInfo->m_pIconTex == NULL)
		{
			i3GameResTexture * pTexRes = (i3GameResTexture *) g_pFramework->FindResourceByPath( pInfo->m_szTex);
			I3ASSERT( pTexRes != NULL);

			pInfo->m_pIconTex = pTexRes->getTexture();
		}
	}
}

void CUserMap::_InitSky(void)
{
	INT32 i, cnt;
	USMAP::SKY::INFO * pInfo;

	cnt = USMAP::SKY::getCount();

	for( i = 0; i < cnt; i++)
	{
		pInfo = USMAP::SKY::getInfo( i);

		i3Vector::Normalize( & pInfo->m_LightDir, & pInfo->m_LightDir);

		if( pInfo->m_pSg == NULL)
		{
			i3GameResSceneGraph * pRes = (i3GameResSceneGraph *) g_pFramework->FindResourceByPath( pInfo->m_szRes);
			I3ASSERT( pRes != NULL);

			pInfo->m_pSg = pRes->getSceneGraphInfo();
		}
	}
}


void CUserMap::_GenGround( REAL32 cx, REAL32 cy)
{
	// ¹Ù´Ú Object
	/*
	CUserMap_Object * pGround;

	pGround = CUserMap_Object::NewObjectRef();
	pGround->Create( USMAP::BLOCK::ID_CUBE);
	pGround->SetSize( cx, 1.0f, cy);
	pGround->SetPos( 0.0f, -1.0f, 0.0f);

	addObject( pGround);
	*/
}

void CUserMap::addObject( CUserMap_Object * pObj)
{
	I3ASSERT( pObj != NULL);
	pObj->AddRef();

	m_ObjectList.Add( pObj);

	// Sg attach.
	pObj->AttachScene();

	pObj->setDynamicActiveState( true);
	pObj->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC);
}

CUserMap_Object * CUserMap::addObject( USMAP::BLOCK::ID id, VEC3D * pPos)
{
	CUserMap_Object * pObj;

	pObj = CUserMap_Object::NewObjectRef();

	pObj->Create( id);

	if( pPos != NULL)
		pObj->SetPos( pPos);

	addObject( pObj);

	return pObj;
}

void CUserMap::removeObject( CUserMap_Object * pObj)
{
	I3ASSERT( pObj != NULL);

	pObj->getSceneObject()->RemoveFromParent();

	m_ObjectList.Remove( pObj);

	pObj->Release();
}

void CUserMap::RemoveAllObject(void)
{
	INT32 i;
	CUserMap_Object * pObj;

	for( i = 0; i < getObjectCount(); i++)
	{
		pObj = getObject( i);

		pObj->getSceneObject()->RemoveFromParent();

		pObj->Release();
	}

	m_ObjectList.Clear();
}

void CUserMap::RemoveAllCondition(void)
{
	// Start Condition
	m_StartConditionList.SafeReleaseAll();

	// Always Condition
	m_ConditionList.SafeReleaseAll();

	// RoundEnd Condition
	m_RoundEndConditionList.SafeReleaseAll();
}

bool CUserMap::SaveToFile( const char * pszPath)
{
	UINT32 sz;
	char * pBuff = NULL, * pDest;
	i3FileStream stream;
	bool bResult = false;
	UINT32 rc;
	INT32 i;

	sz = CalcBufferSize();
	if( sz <= 0)
		goto ExitPart;

	pBuff = (char *) i3MemAlloc( sz);
	if( pBuff == NULL)
		goto ExitPart;

	pDest = pBuff;

	// Stage Info
	{
		i3mem::Copy( pDest, &m_StageInfo, sizeof( USMAP::STAGE::INFO));

		pDest += sizeof( USMAP::STAGE::INFO);
	}

	// Object Info
	for( i = 0; i < getObjectCount(); i++)
	{
		CUserMap_Object * pObj = getObject( i);

		i3mem::Copy( pDest, pObj->getInfo(), sizeof( USMAP::OBJECT::INFO));

		pDest += sizeof( USMAP::OBJECT::INFO);
	}

	// Conditions
	for( i = 0; i < getStartConditionCount(); i++)
	{
		CUserMap_Condition * pCond = getStartCondition( i);

		i3mem::Copy( pDest, pCond->getInfo(), sizeof( USMAP::COND::INFO));

		pDest += sizeof( USMAP::COND::INFO);
	}


	if( stream.Create( pszPath, STREAM_WRITE | STREAM_READ) == FALSE)
	{
		I3FATAL( "Could not save %s UserMap Data.\n", pszPath);
		return false;
	}

	rc = stream.Write( pBuff, sz);
	if( rc == STREAM_ERR)
	{
		return false;
	}

	bResult = true;

ExitPart:
	I3MEM_SAFE_FREE( pBuff);

	stream.Close();

	return bResult;
}

bool CUserMap::LoadFromFile( const char * pszPath)
{
	return true;
}

CUserMap_Object	* CUserMap::GetSelectedObject( VEC3D * pvStart, VEC3D *pvEnd)
{
	i3CollideeLine line;
	line.SetStart( pvStart);
	line.SetEnd( pvEnd);

	I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
	if( pResult != NULL)
	{
		i3EventReceiver * pRecv = pResult->m_pDestShapeSet->getEventReceiver();

		I3ASSERT( pRecv->IsTypeOf( CUserMap_Object::GetClassMeta()) == TRUE);

		return (CUserMap_Object*)pRecv;
	}

	return NULL;
}
