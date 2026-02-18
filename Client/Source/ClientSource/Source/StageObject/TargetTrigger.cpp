#include "pch.h"
#include "TargetTrigger.h"
#include "GameCharaBase.h"
#if defined( USE_EDITDLG)
#include "i3Level/i3LevelElement3D.h"
#endif

I3_CLASS_INSTANCE( TargetTrigger);//, i3Trigger);

TargetTrigger::TargetTrigger(void)
{
	m_Team = TEAM_RED;
}

TargetTrigger::~TargetTrigger(void)
{
	removeAllTarget();
}

void	TargetTrigger::addTarget( TargetObject * pObj)
{
	I3_MUST_ADDREF(pObj);

	m_ObjList.push_back( pObj);
}

void	TargetTrigger::removeTarget( TargetObject * pObj)
{
	I3ASSERT( pObj != nullptr);
	INT32 idx;

	idx = i3::vu::int_index_of(m_ObjList, pObj);
	I3ASSERT( idx != -1);

	I3_MUST_RELEASE(pObj);

	i3::vu::erase_index(m_ObjList, idx);
}

void	TargetTrigger::removeAllTarget(void)
{
	INT32 i;
	TargetObject * pObj;

	for( i = 0; i < getTargetCount(); i++)
	{
		pObj = getTarget( i);

		I3_MUST_RELEASE(pObj);
	}

	m_ObjList.clear();
}

void	TargetTrigger::OnEnter( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest)
{
	if( i3::kind_of<CGameCharaBase*>(pDest))
	{
		//CGameCharaBase * pChara = (CGameCharaBase *) pDest;

		// 지정된 Target들을 활성화시킨다.
		for( INT32 i = 0; i < getTargetCount(); i++)
		{
			TargetObject * pTarget = getTarget( i);

			pTarget->Start();
		}
	}
}

void	TargetTrigger::OnLeave( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest)
{
}

void	TargetTrigger::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	for( INT32 i = 0; i < getTargetCount(); i++)
	{
		TargetObject * pObj = getTarget( i);

		pObj->OnBuildObjectList( List);
	}

	i3Trigger::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

struct ALIGN4 PERSIST_ACTION_TARGET_ACTIVATE
{
	INT8		m_ID[4];
	UINT32		m_ObjCount;
	INT32		m_Team;
	UINT32		pad[16];
} ;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	TargetTrigger::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	PERSIST_ACTION_TARGET_ACTIVATE data;

	result = i3Trigger::OnSave( pResFile);
	I3_CHKIO( result);

	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "ATA1", 4);
	data.m_ObjCount			= getTargetCount();
	data.m_Team				= (UINT32) getTeam();

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( getTargetCount() > 0)
	{
		//rc = m_ObjList.SaveTo( pResFile);
		rc = SaveFromListToResourceFile(m_ObjList, pResFile);
		I3_CHKIO( rc);
		result += rc;
	}

	return result;
}

UINT32	TargetTrigger::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	PERSIST_ACTION_TARGET_ACTIVATE data;

	result = i3Trigger::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_ObjCount > 0)
	{
	//	rc = m_ObjList.LoadFrom( pResFile, true);
		rc = LoadFromResourceFileToList(m_ObjList, pResFile, true);
		I3_CHKIO( rc);
		result += rc;
	}
	
	setTeam( (TEAM_TYPE) data.m_Team);

	return result;
}

UINT32 TargetTrigger::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{
	UINT32 sz;
	PERSIST_ACTION_TARGET_ACTIVATE * pData;
	OBJREF * pRef;
	INT32 i;

	#if defined( USE_EDITDLG)
	if( pRefObj != nullptr)
	{
		i3LevelElement3D * pElm = (i3LevelElement3D *) pRefObj;

		removeAllTarget();

		for( i = 0; i < pElm->getTargetCount(); i++)
		{
			i3LevelElement3D * pDestElm = pElm->getTarget( i);

			i3GameObj * pObj = pDestElm->CreateGameInstance();

			if(pObj != nullptr)
			{
				if( i3::kind_of<TargetObject*>(pObj))
				{
					addTarget( (TargetObject *) pObj);
				}

				I3_MUST_RELEASE(pObj);
			}
		}
	}
	#endif

	sz = sizeof(PERSIST_ACTION_TARGET_ACTIVATE) + (sizeof(OBJREF) * getTargetCount());

	*ppData = pData = (PERSIST_ACTION_TARGET_ACTIVATE *) i3MemAlloc( sz);
	
	i3mem::FillZero( pData, sz);
	i3mem::Copy( pData->m_ID, "ATA1", 4);

	pData->m_Team		= getTeam();
	pData->m_ObjCount	= getTargetCount();

	pRef = (OBJREF *) (pData + 1);

	for( i = 0; i < getTargetCount(); i++)
	{
		if( pInfo != nullptr)
		{
			TargetObject * pTarget = getTarget( i);

			pRef[i] = pInfo->GetObjectPersistID( pTarget);
			I3ASSERT( pRef[i] != -1);
		}
		else
		{
			// pInfo가 nullptr인 경우는 실제로 발생할 수 있다.
			// 그것은 Instance List의 저장이 아니라 Resource File에의
			// 저장이기 때문에 여기에서 필요한 Reference를 저장하지
			// 않더라도 OnSave() 함수를 통해 저장되기 때문에 문제가 되지 않는다.
			pRef[i] = 0;
		}
	}

	return sz;
}

UINT32 TargetTrigger::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	PERSIST_ACTION_TARGET_ACTIVATE data;
	i3Stream * pStream = pInfo->getStream();
	UINT32 rc, result = 0;
	UINT32 i;

	rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setTeam( (TEAM_TYPE) data.m_Team);

	if( data.m_ObjCount > 0)
	{
		OBJREF ref;

		if((pInfo->getInstanceList() != nullptr) && ( pInfo->getInstanceList()->size() > 0))
		{
			m_ObjList.resize( data.m_ObjCount);
		}

		for( i = 0; i < data.m_ObjCount; i++)
		{
			rc = pStream->Read( &ref, sizeof(OBJREF));
			I3_CHKIO( rc);
			result += rc;

			TargetObject * pObj = (TargetObject *) pInfo->FindObjectByIndex( (INT32) ref);
			
			if( pObj != nullptr)
			{
				I3_MUST_ADDREF(pObj);
				m_ObjList[i] = pObj;
			}
		}
	}
	
	if( pRefObj != nullptr)
	{
		// pRefObj가 있다면 Loading된 Target List보다 pRefObj가 가지고 있는 값을 우선한다.
		removeAllTarget();
	#if defined(USE_EDITDLG)
		i3LevelElement3D * pElm = (i3LevelElement3D *) pRefObj;

		for( i = 0; i < (UINT32) pElm->getTargetCount(); i++)
		{
			i3LevelElement3D * pDestElm = pElm->getTarget( i);

			i3GameObj * pObj = pDestElm->CreateGameInstance();

			if( pObj != nullptr)
			{
				if( i3::kind_of<TargetObject*>(pObj))
                    addTarget( (TargetObject *) pObj);

				I3_MUST_RELEASE(pObj);
			}
		}
	#endif
	}

	I3ASSERT( result == sz);

	return result;
}

