#include "pch.h"
#include "LinkObjTrigger.h"
#include "GameCharaBase.h"

#if defined( USE_EDITDLG)
#include "i3TDKUpdateSystem.h"
#include "i3Level/i3LevelElement3D.h"
#endif

I3_CLASS_INSTANCE( CLinkObjTrigger);//, i3Trigger);

CLinkObjTrigger::CLinkObjTrigger()
{
}

CLinkObjTrigger::~CLinkObjTrigger()
{
	removeAllLinkObj();
}

void	CLinkObjTrigger::addLinkObj( i3Object * pObj)
{
	I3_MUST_ADDREF(pObj);

	m_ObjList.push_back( pObj);
}

void	CLinkObjTrigger::removeLinkObj( i3Object * pObj)
{
	I3ASSERT( pObj != nullptr);
	INT32 idx;

	idx = i3::vu::int_index_of(m_ObjList, pObj);
	I3ASSERT( idx != -1);

	I3_MUST_RELEASE(pObj);

	i3::vu::erase_index(m_ObjList, idx);
}

void	CLinkObjTrigger::removeAllLinkObj(void)
{
	INT32 i;
	i3Object * pObj;

	for( i = 0; i < getLinkObjCount(); i++)
	{
		pObj = getLinkObj( i);

		I3_MUST_RELEASE(pObj);
	}

	m_ObjList.clear();
}


/*virtual*/ void CLinkObjTrigger::OnEnter( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest)
{
}

/*virtual*/ void CLinkObjTrigger::OnLeave( i3PhysixShapeSet * pTriggerShapeSet, i3ElementBase * pDest)
{	
}

/*virtual*/ void CLinkObjTrigger::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	for( INT32 i = 0; i < getLinkObjCount(); i++)
	{
		i3Object * pObj = getLinkObj( i);

		pObj->OnBuildObjectList( List);
	}

	i3Trigger::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

struct ALIGN4 PERSIST_LINK_OBJECT_LIST
{
	INT8		m_ID[4];
	UINT32		m_ObjCount;
	UINT32		pad[16];
} ;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 CLinkObjTrigger::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	PERSIST_LINK_OBJECT_LIST data;

	result = i3Trigger::OnSave( pResFile);
	I3_CHKIO( result);

	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "LOL1", 4);
	data.m_ObjCount			= getLinkObjCount();

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( getLinkObjCount() > 0)
	{
		rc = SaveFromListToResourceFile(m_ObjList, pResFile);		//m_ObjList.SaveTo( pResFile);
		I3_CHKIO( rc);
		result += rc;
	}

	return result;
}

UINT32 CLinkObjTrigger::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	PERSIST_LINK_OBJECT_LIST data;

	result = i3Trigger::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_ObjCount > 0)
	{
		rc = LoadFromResourceFileToList(m_ObjList, pResFile, true );
				//m_ObjList.LoadFrom( pResFile, true);
		I3_CHKIO( rc);
		result += rc;
	}

	return result;
}

UINT32 CLinkObjTrigger::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{
	UINT32 sz;
	PERSIST_LINK_OBJECT_LIST * pData;
	OBJREF * pRef;
	INT32 i;

	sz = sizeof(PERSIST_LINK_OBJECT_LIST) + (sizeof(OBJREF) * getLinkObjCount());

	*ppData = pData = (PERSIST_LINK_OBJECT_LIST *) i3MemAlloc( sz);
	
	i3mem::FillZero( pData, sz);
	i3mem::Copy( pData->m_ID, "LOL1", 4);

	pData->m_ObjCount	= getLinkObjCount();

	pRef = (OBJREF *) (pData + 1);

	for( i = 0; i < getLinkObjCount(); i++)
	{
		if( pInfo != nullptr)
		{
			i3Object * pTarget = getLinkObj( i);

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

UINT32 CLinkObjTrigger::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	PERSIST_LINK_OBJECT_LIST data;
	i3Stream * pStream = pInfo->getStream();
	UINT32 rc, result = 0;
	UINT32 i;

	rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

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

			i3Object * pObj = (i3Object *) pInfo->FindObjectByIndex( (INT32) ref);
			
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
		removeAllLinkObj();
	#if defined(USE_EDITDLG)
		i3LevelElement3D * pElm = (i3LevelElement3D *) pRefObj;

		for( i = 0; i < (UINT32) pElm->getTargetCount(); i++)
		{
			i3LevelElement3D * pDestElm = pElm->getTarget( i);

			i3GameObj * pObj = pDestElm->CreateGameInstance();

			if(pObj != nullptr )
			{
				if( i3::kind_of<i3Object*>(pObj))
				{
					addLinkObj( (i3Object *) pObj);
				}

				I3_MUST_RELEASE(pObj);
			}
		}
	#endif
	}

	I3ASSERT( result == sz);

	return result;
}

#if defined(USE_EDITDLG)//XML
bool CLinkObjTrigger::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;
	i3::vector<i3LevelElement3D*> list;

	// LevelTool에서 저장될 때에는 직접 대상이 되는 i3GameObj Pointer를 저장해서는 안되며,
	// 해당 i3GameObj를 가지고 있는 i3LevelElement3D의 Pointer를 저장해야 한다.
	i3LevelElement3D * pElm = (i3LevelElement3D *) pFile->getCurrentObject();
	I3ASSERT( i3::kind_of<i3LevelElement3D*>(pElm));

	for( INT32 i = 0; i < pElm->getTargetCount(); i++)
	{
		list.push_back( pElm->getTarget( i));
	}

	rv = SaveFromListToXML(list, "ObjList", pFile, pXML, true);
			//list.SaveToXML( "ObjList", pFile, pXML, true);
	if( rv == false)
	{
		I3ERRLOG( i3Error::_WRITEFAIL);
	}
	
	return rv;
}

bool CLinkObjTrigger::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{

	// LevelTool에서 저장될 때에는 직접 대상이 되는 i3GameObj Pointer를 저장해서는 안되며,
	// 해당 i3GameObj를 가지고 있는 i3LevelElement3D의 Pointer를 저장해야 한다.
	i3LevelElement3D * pElm = (i3LevelElement3D *) pFile->getCurrentObject();
	I3ASSERT( i3::kind_of<i3LevelElement3D*>(pElm));

	for( INT32 i = 0; i < pElm->getTargetCount(); i++)
	{
		i3LevelElement3D * pTarget = pElm->getTarget( i);

		i3GameObj * pObj = pTarget->CreateGameInstance();

		if( pObj != nullptr)
		{
			if( i3::kind_of<i3Object*>(pObj))
			{
				addLinkObj( (i3Object *) pObj);
			}

			I3_MUST_RELEASE(pObj);
		}
	}

	return true;
}
#endif

