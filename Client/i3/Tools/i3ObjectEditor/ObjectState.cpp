#include "stdafx.h"
#include "ObjectState.h"
#include "GlobalVar.h"
#include "i3Base/string/ext/make_relative_path.h"

I3_CLASS_INSTANCE( CObjectState); //, i3EventReceiver);

CObjectState::CObjectState(void) : m_pCollideRootForClear(NULL)
{
	m_pOwner = NULL;
	m_HPRatio = 0.0f;
	m_pTimeEvent = i3TimeEventGen::new_object();

	m_pSg = NULL;
	m_pCollideRootForClear = i3AttrSet::new_object();
	m_pBody = NULL;

	m_bModified = false;
	m_bContainConvex = false;
}

CObjectState::~CObjectState(void)
{
	RemoveAllTimeEvent();
	I3_SAFE_RELEASE( m_pTimeEvent);
	I3_SAFE_RELEASE( m_pCollideRootForClear);	
	I3_SAFE_RELEASE( m_pSg);

}

void CObjectState::setSgResource( i3SceneGraphInfo * pInfo)
{
	I3_REF_CHANGE( m_pSg, pInfo);

	if( m_pSg != NULL)
	{
		// Bone List와 i3Body Node에 대한 검색을 위해
		// 임시 i3SceneObject를 만들어 Context를 생성해 둔다.
		i3SceneObject * pSceneObj = i3SceneObject::new_object();

		m_pSg->getInstanceSg()->Instantiate();

		pSceneObj->AddChild( m_pSg->getInstanceSg());
		pSceneObj->BuildBoneList();

//		I3_REF_CHANGE( m_pBoneList, pSceneObj->getBoneList());
		m_pBoneList = pSceneObj->getBoneList();
		m_pBody = pSceneObj->GetBody();

		pSceneObj->Release();

		// 처음부터 바로 Physics를 Disable 시켜주어야 문제가 없다.
		Disable();
	}

	_CheckConvex();
	
	// 두번째 인수가 NULL인 경우 카운팅된 생성객체가 리턴되므로 이걸 신경써야함.(12.09.18.수빈)
	i3Node* node_collis = i3SceneUtil::BuildRendableCollider( pInfo->getInstanceSg(), NULL, TRUE, TRUE);
	m_pCollideRootForClear->AddChild(node_collis);	node_collis->Release();
}

void CObjectState::_CheckConvex(void)
{
	i3::vector<i3Node*> list;
	

	m_bContainConvex = false;

	// i3Transform2 Node를 포함하고 있는지 확인
	i3Scene::FindNodeByType( m_pSg->getInstanceSg(), i3Transform2::static_meta(), list);

	if( list.size() > 0)
	{
		m_bContainConvex = true;
		return;
	}

	list.clear();

	// i3Body Node를 확인...
	i3Scene::FindNodeByType( m_pSg->getInstanceSg(), i3Body::static_meta(), list);

	for(size_t i = 0; i < list.size(); i++)
	{
		i3Body * pBody = (i3Body *) list[i];

		i3LOD * pLOD = pBody->getFrameLOD();

		if( pLOD->getSkeleton()->isShapeSet(0))
		{
			m_bContainConvex = true;
			return;
		}
	}
}

void CObjectState::Enable(void)
{
	if( m_pSg != NULL)
	{
		i3Scene::ModifyDynamicState( m_pSg->getInstanceSg(), 0, I3_PHYSIX_STATE_FROZEN | I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE);
	}
}

void CObjectState::Disable(void)
{
	if( m_pSg != NULL)
	{
		i3Scene::ModifyDynamicState( m_pSg->getInstanceSg(), I3_PHYSIX_STATE_FROZEN | I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE, 0);
	}
}

bool CObjectState::Serialize(void)
{
	if( m_bModified == false)
		return true;

	//
	char szFull[ MAX_PATH];

	sprintf( szFull, "%s/%s", g_pOption->m_szWorkPath, m_pSg->GetName());

	{
		i3ResourceFile file;

		file.setKeyObject( m_pSg);

		if( file.Save( szFull) == STREAM_ERR)
		{
			NotifyBox( NULL, "%s Resource 파일을 갱신할 수 없습니다.", szFull);
			return false;
		}
	}

	m_bModified = false;

	return true;
}

INT32 CObjectState::AddTimeEvent( REAL32 tm, i3Action * pAction, UINT32 param)
{
	i3TimeEventInfo * pEvent;
	INT32 idx;

	pEvent = i3TimeEventInfo::new_object_ref();

	pEvent->setStartTime( tm);
	pEvent->setAction( pAction);
	pEvent->setParam( param);

	if((pAction != NULL) && (pAction->hasName()))
		pEvent->SetName( pAction->GetName());

	idx = m_pTimeEvent->AddEvent( pEvent);

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pEvent);

	return idx;
}

void CObjectState::RemoveTimeEvent( INT32 idx)
{
	i3TimeEventInfo * pEvent;

	pEvent = getEvent( idx);

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, pEvent);

	m_pTimeEvent->RemoveEvent( idx);
}

void CObjectState::RemoveAllTimeEvent(void)
{
	m_pTimeEvent->RemoveAllEvents();

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, NULL, i3TimeEventInfo::static_meta());
	i3TDK::Update( NULL, I3_TDK_UPDATE_EDIT, this);
}

void CObjectState::BuildHull(void)
{
	i3TDKDlgPhysixSetting dlg;

	if( dlg.Execute( m_pSg->getInstanceSg()))
	{
		m_pSg->setInstanceSg( dlg.getScene());

		setSgResource( m_pSg);

		i3Node* node_collis = i3SceneUtil::BuildRendableCollider( m_pSg->getInstanceSg(), NULL, TRUE, TRUE);
		m_pCollideRootForClear->AddChild(node_collis);	node_collis->Release();
		// dynamic State -> dynamic
		i3Scene::ModifyDynamicState( m_pSg->getInstanceSg(), 0, I3_PHYSIX_STATE_KINEMATIC);

		m_bModified = true;
	}

	i3Node* node_collis = i3SceneUtil::BuildRendableCollider( m_pSg->getInstanceSg(), NULL, TRUE, TRUE);
	m_pCollideRootForClear->AddChild(node_collis);	node_collis->Release();
}

void CObjectState::RemoveVolatileNodes( i3Node * pNode)
{
	if( pNode == NULL)
	{
		if(m_pSg != NULL)
			pNode = m_pSg->getInstanceSg();
		else
			return;
	}

	INT32 i;

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild( i);

		if( pChild->IsFlag( I3_NODEFLAG_VOLATILE))
		{
			pChild->RemoveFromParent();

			i--;
		}
		else
		{
			RemoveVolatileNodes( pChild);
		}
	}

}

void CObjectState::OnDestroy(void)
{
	m_pCollideRootForClear->RemoveAllChild(); 
	// 윗줄은 Volatile만 지운다고 될게 아닌듯하여, 그냥 확지움..(2012.09.18.수빈)

	RemoveVolatileNodes();
}

void CObjectState::OnRevive(void)
{
	if(m_pSg != NULL)
	{
		i3Node* node_collis = i3SceneUtil::BuildRendableCollider( m_pSg->getInstanceSg(), NULL, TRUE, TRUE);
		m_pCollideRootForClear->AddChild(node_collis);	
		node_collis->Release();
	}
}

void CObjectState::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pTimeEvent != NULL)
	{
		m_pTimeEvent->OnBuildObjectList( List);
	}

	if( m_pSg != NULL)
	{
		m_pSg->OnBuildObjectList( List);
	}

	i3EventReceiver::OnBuildObjectList( List);
}

#pragma pack(push, 8)				// [test required] 패킹사이즈 누락으로 추가함. 2017.02.16. soon9

namespace pack
{
	struct OBJECT_STATE
	{
		UINT8		m_ID[4] = { 'O', 'S', 'T', '1' };
		REAL32		m_HP = 0.0f;
		OBJREF		m_pTimeEvent = 0;
		OBJREF		m_pSg = 0;
		UINT32		pad[8] = { 0 };
	};
}

#pragma pack(pop)

UINT32 CObjectState::OnSave( i3ResourceFile * pResFile)
{
	UINT32	rc, result;
	pack::OBJECT_STATE data;

	result = i3EventReceiver::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_HP			= getHPRatio();
	data.m_pSg			= pResFile->GetObjectPersistID( getSgResource());
	data.m_pTimeEvent	= pResFile->GetObjectPersistID( m_pTimeEvent);

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 CObjectState::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::OBJECT_STATE data;

	result = i3EventReceiver::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setHPRatio(	data.m_HP);

	if( data.m_pTimeEvent != 0)
	{
		i3TimeEventGen * pGen = (i3TimeEventGen *) pResFile->FindObjectByID( data.m_pTimeEvent);
		I3ASSERT( pGen != NULL);

		setTimeEvent( pGen);
	}

	if( data.m_pSg != NULL)
	{
		i3SceneGraphInfo * pRes = (i3SceneGraphInfo *) pResFile->FindObjectByID( data.m_pSg);
		I3ASSERT( pRes != NULL);

		// 실제 이름을 확인하여 재설정한다.
		// 대소문자를 구분하는 SVN를 위한 조치.
		{
			char szRealName[MAX_PATH], szRelPath[MAX_PATH], szTemp[ MAX_PATH];
			
			if (g_pOption->m_szWorkPath[0] != 0)
				sprintf( szTemp, "%s/%s", g_pOption->m_szWorkPath, pRes->GetName());	
			else
				sprintf( szTemp, "%s", pRes->GetName());	//	WorkPath가 비어있으면 괜이 /가 최선두에 붙어서 파일을 못찾음..
															//	2012.09.18.수빈.	

			i3System::GetRealFilename( szTemp, szRealName);

			i3::make_relative_path( g_pOption->m_szWorkPath, szRealName, szRelPath );
			i3::make_unix_path( szRelPath);

			pRes->SetResourcePath( szRelPath);
		}

		setSgResource( pRes);
	}

	return result;
}
