#include "stdafx.h"
#include "ObjectTemplate.h"
#include "GlobalVar.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/make_relative_path.h"

I3_CLASS_INSTANCE( CObjectTemplate); //, i3GameObj);

CObjectTemplate::CObjectTemplate(void)
{ 
	m_idxOldState = -1;
	m_idxCurState = -1;
	m_timeLocal = 0.0f;

	m_MaxHP = 100.0f;
}

CObjectTemplate::~CObjectTemplate(void)
{
	RemoveAllState();
	RemoveAllEffect();
}

void CObjectTemplate::RemoveAllState(void)
{
	INT32 i;
	CObjectState * pState;

	for( i = 0; i < getStateCount(); i++)
	{
		pState = getState( i);

		pState->Release();
	}

	m_StateList.clear();

	SetNode( NULL);
	m_idxCurState = -1;

	i3TDK::Update( NULL, I3_TDK_UPDATE_SCENE, this);
}

void CObjectTemplate::AddState( CObjectState * pState)
{
	I3ASSERT( pState != NULL);

	pState->AddRef();

	m_StateList.push_back( pState);

	if( m_StateList.size() == 1)
	{
		// 첫 State이기 때문에 CurrentState로 설정해 둔다.
		SetCurrentState( 0);
		i3TDK::Update( NULL, I3_TDK_UPDATE_EDIT, this);
	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pState);
}

void CObjectTemplate::LoadAndAddState( const char * pszPath)
{
	i3SceneFile file;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		NotifyBox( NULL, "%s 파일을 열 수가 없네...더 진행 못함.", pszPath);
		return;
	}

	// New State
	{
		char szRelPath[ MAX_PATH];
		i3SceneGraphInfo * pInfo;

		pInfo = file.getSceneGraphInfo();

		i3::make_relative_path( g_pOption->m_szWorkPath, pszPath, szRelPath );
		pInfo->SetName( szRelPath);
		pInfo->SetExternRes( TRUE);

		CObjectState * pState = CObjectState::new_object_ref();
		pState->setOwner( this);
		pState->setHPRatio( 0.0f);
		pState->setSgResource( pInfo);

		i3ActionBreakObject * pAction = i3ActionBreakObject::new_object_ref();
		pAction->SetName( "Break");
		pState->AddTimeEvent( 0.0f, pAction, 0);

		AddState( pState);
	}
}

void CObjectTemplate::ChangeStateSg( CObjectState * pState, const char * pszPath)
{
	i3SceneFile file;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		NotifyBox( NULL, "%s 파일을 읽을 수 없다.", pszPath);
		return;
	}

	// New State
	{
		char szRelPath[ MAX_PATH];
		i3SceneGraphInfo * pInfo;

		pInfo = file.getSceneGraphInfo();
		pInfo->SetExternRes( TRUE);

		i3::make_relative_path( g_pOption->m_szWorkPath, pszPath, szRelPath  );
		pInfo->SetName( szRelPath);

		pState->setSgResource( pInfo);

		if( pState == getState( m_idxCurState))
		{
			// 현재 선택된 State라면 화면도 갱신 유도...
			_SetCurrentSg();
		}

		i3TDK::Update( NULL, I3_TDK_UPDATE_EDIT, pState);
	}
}

void CObjectTemplate::SetCurrentState( INT32 idx)
{
	CObjectState * pState;

	if(m_idxCurState != -1)
	{
		pState = getState( m_idxCurState);

		i3TimeEventGen * pEvent = pState->getTimeEvent();

		pEvent->OnChangeTime( 0.0f);
	}

	m_idxOldState = m_idxCurState;
	m_idxCurState = idx;
	pState = getState( idx);

	if( pState->getTimeEvent() != NULL)
	{
		i3TimeEventGen * pEvent = pState->getTimeEvent();

		pEvent->Reset();
		pEvent->OnUpdate( 0.0f);
	}

	m_timeLocal = 0.0f;

	i3TDK::Update( NULL, I3_TDK_UPDATE_SELECT, pState);
}

void CObjectTemplate::_SetCurrentSg(void)
{
	if( m_idxOldState != -1)
	{
		CObjectState * pState = getState( m_idxOldState);
//		i3SceneGraphInfo * pInfo = pState->getSgResource();

		pState->Disable();
	}

	if( m_idxCurState != -1)
	{
		CObjectState * pState = getState( m_idxCurState);
		i3SceneGraphInfo * pInfo = pState->getSgResource();

		pState->Enable();

		if( pInfo != NULL)
		{
			getAttachNode()->RemoveAllChild();
			getAttachNode()->AddChild( pInfo->getInstanceSg());

			getSceneObject()->setBoneList( pState->getBoneList());
			getSceneObject()->SetBody( pState->getBody());

			getSceneObject()->FindWorldTransformSource();
			getSceneObject()->UpdateLastWorldTransform( getSceneObject()->GetCacheMatrix());
		}
		//
	}
}

void CObjectTemplate::BuildConvex( INT32 idx)
{
	CObjectState * pState = getState( idx);

	pState->BuildHull();

	if( m_idxCurState == idx)
	{
		SetCurrentState( idx);
	}
}

INT32	CObjectTemplate::LoadEffect( const char * pszEffect)
{
	i3SceneFile file;
	char szRelPath[ MAX_PATH], szFullPath[ MAX_PATH];
	bool bAbsPath = false;

	if( pszEffect[1] == ':')
		bAbsPath = true;

	if( bAbsPath)
	{
		i3::make_relative_path( g_pOption->m_szWorkPath, pszEffect, szRelPath );
		i3::safe_string_copy( szFullPath, pszEffect, MAX_PATH );
	}
	else
	{
		strcpy( szRelPath, pszEffect);

		sprintf( szFullPath, "%s/%s", g_pOption->m_szWorkPath, pszEffect);
	}

	i3::make_unix_path( szRelPath);

	// 이름으로 현재 동일한 Effect가 있는지 확인 (Relative Path) 
	{
		INT32 idx;

		idx = FindEffect( szRelPath);
		if( idx != -1)
			return idx;
	}

	if( file.Load( szFullPath) == STREAM_ERR)
	{
		NotifyBox( NULL, "%s Effect 파일을 읽어들일 수 없습니다.", szFullPath);
		return -1;
	}

	i3SceneGraphInfo * pInfo = (i3SceneGraphInfo *) file.getSceneGraphInfo();

	// Relative Path
	pInfo->SetName( szRelPath);

	return AddEffect( pInfo);
}

INT32	CObjectTemplate::AddEffect( i3SceneGraphInfo * pInfo)
{
	pInfo->AddRef();

	// Effect Manager에의 추가
	{
		char szName[MAX_PATH];
		i3EffectTypeInfo * pType;
		i3GameResSceneGraph * pRes = i3GameResSceneGraph::new_object();

		pRes->SetName( pInfo->GetName());
		pRes->setSceneGraphInfo( pInfo);

		//i3String::SplitFileName( pInfo->GetName(), szName, FALSE);
		i3::extract_filetitle(pInfo->GetName(), szName);

		pType = i3EffectTypeInfo::new_object_ref();

		pType->SetName( szName);
		pType->Create( pRes, 5);

		pRes->Release();

		i3Game::getEffectManager()->AddEffectType( pType);
	}

	m_EffectList.push_back( pInfo);

	return (INT32)m_EffectList.size() - 1;
}

void	CObjectTemplate::RemoveAllEffect(void)
{
	
	i3SceneGraphInfo * pInfo;

	for(size_t i = 0; i < m_EffectList.size(); i++)
	{
		pInfo = m_EffectList[i];

		pInfo->Release();
	}

	m_EffectList.clear();

}

INT32 CObjectTemplate::FindEffect( const char * pszPath)
{
	INT32 i;
	i3SceneGraphInfo * pInfo;

	for( i = 0; i < getEffectCount(); i++)
	{
		pInfo = getEffect( i);

		if( i3::generic_is_iequal( pszPath, pInfo->GetName()) )
			return i;
	}

	return -1;
}

void CObjectTemplate::OnUpdate( REAL32 tm)
{
	if( m_idxCurState != -1)
	{
		CObjectState * pState = getState( m_idxCurState);

		if( pState->getTimeEvent() != NULL)
		{
			pState->getTimeEvent()->OnUpdate( tm);
		}

		m_timeLocal += tm;
	}
}

bool CObjectTemplate::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( event == I3_EVT_ACTION)
		{
			I3ASSERT( i3::kind_of<i3Action*>(pObj)); //->IsTypeOf( i3Action::static_meta()));

			if( i3::same_of<i3ActionBreakObject*>(pObj)) //->IsExactTypeOf( i3ActionBreakObject::static_meta()))
			{
				_SetCurrentSg();

				return true;
			}
			else
			{
			}
		}
	}

	return i3GameObj::OnEvent( event, pObj, param, code);
}

void CObjectTemplate::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	CObjectState * pState;

	for( i = 0; i < getStateCount(); i++)
	{
		pState = getState( i);

		pState->OnBuildObjectList( List);
	}

	i3GameObj::OnBuildObjectList( List);
}

#pragma pack(push, 8)					// [test required] 패킹사이즈 누락으로 추가함. 2017.02.16. soon9

namespace pack
{
	struct OBJECT_TEMPLATE
	{
		UINT8		m_ID[4] = { 'O', 'T', 'P', '1' };
		REAL32		m_MaxHP = 0.0f;
		UINT32		m_EffectCount = 0;
		UINT32		m_SoundCount = 0;
		UINT32		pad[62] = { 0 };
	};
}

#pragma pack(pop)

UINT32 CObjectTemplate::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::OBJECT_TEMPLATE data;

	result = i3GameObj::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_MaxHP		= m_MaxHP;
	data.m_EffectCount =	m_EffectList.size();

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	rc = SaveFromListToResourceFile(m_StateList, pResFile);		//	m_StateList.SaveTo( pResFile);
	I3_CHKIO( rc);
	result += rc;

	// Effect List;
	{
		INT32 i;
		i3SceneGraphInfo * pInfo;

		for( i = 0; i < getEffectCount(); i++)
		{
			pInfo = getEffect( i);

			char * pszPath = (char *) pInfo->GetName();
			UINT32 len = strlen( pszPath);

			rc = pResFile->Write( &len, sizeof(len));
			I3_CHKIO( rc);
			
			rc = pResFile->Write( pszPath, len);
			I3_CHKIO( rc);
		}
	}

	return result;
}

UINT32 CObjectTemplate::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::OBJECT_TEMPLATE data;
	INT32 i;

	result = i3GameObj::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setMaxHP( data.m_MaxHP);

	rc = LoadFromResourceFileToList(m_StateList, pResFile, true);		//	m_StateList.LoadFrom( pResFile, true);
	I3_CHKIO( rc);
	result += rc;

	// Owner의 재설정
	for( i = 0; i < getStateCount(); i++)
	{
		CObjectState * pState = getState( i);

		pState->setOwner( this);
	}

	// Effect List
	{
		char szName[ MAX_PATH];
		UINT32 len;

		for( i = 0; i < (INT32) data.m_EffectCount; i++)
		{
			rc = pResFile->Read( &len, sizeof(len));
			I3_CHKIO( rc);

			rc = pResFile->Read( szName, len);
			I3_CHKIO( rc);

			szName[ len] = 0;

			LoadEffect( szName);
		}
	}

	i3Matrix::Copy( getSceneObject()->GetCacheMatrix(), getSceneObject()->GetMatrix());

	if( getStateCount() > 0)
	{
		SetCurrentState( 0);
	}

	return result;
}

bool CObjectTemplate::Flush(void)
{
	CObjectState * pObjState;
	INT32 i;

	for( i = 0; i < getStateCount(); i++)
	{
		pObjState = getState( i);

		pObjState->Serialize();
	}

	return true;
}

bool CObjectTemplate::Export( const char * pszPath)
{
	i3Object * pObj;
	INT32 i;
	CObjectState * pObjState;

	if( getStateCount() <= 0)
		return false;

	pObj = i3Object::new_object();

	pObj->setMaxHP( getMaxHP());
	pObj->SetStateCount( getStateCount());

	for( i = 0; i < getStateCount(); i++)
	{
		pObjState = getState( i);

		pObj->setHP( i, pObjState->getHPRatio());
		
		i3TimeEventGen* pTimeEvent = i3TimeEventGen::new_object();
		pObjState->getTimeEvent()->CopyTo(pTimeEvent, I3_COPY_INSTANCE);
		pObj->setTimeEvent( i, pTimeEvent);

		pObj->setSceneGraphInfo( i, pObjState->getSgResource(), NULL);

		pObjState->Serialize();
	}
	
	// Reference Object의 설정
	for( i = 0; i < getRefObjectCount(); i++)
	{
		i3ResourceObject * pTemp = getRefObject( i);

		pObj->addRefObject( pTemp);
	}

	{
		i3ResourceFile file;

		file.setKeyObject( pObj);

		if( file.Save( pszPath) == STREAM_ERR)
		{
			NotifyBox( NULL, "%s 파일을 저장할 수 없습니다.", pszPath);
			return false;
		}
	}
	for (i = 0; i < getStateCount(); i++)
	{
		i3TimeEventGen* pTimeEvent = pObj->getTimeEvent(i);
		I3_SAFE_RELEASE(pTimeEvent);
	}

	I3_SAFE_RELEASE(pObj);
	

	return true;
}

bool	CObjectTemplate::OnLostDevice( bool bLostDevice)
{
	INT32 i;
	CObjectState * pState;

	for( i = 0; i < getStateCount(); i++)
	{
		pState = getState( i);
		pState->OnDestroy();
	}

	return i3GameObj::OnLostDevice( bLostDevice);
}

bool	CObjectTemplate::OnRevive( i3RenderContext * pCtx)
{
	INT32 i;
	CObjectState * pState;

	for( i = 0; i < getStateCount(); i++)
	{
		pState = getState( i);
		pState->OnRevive();
	}

	return i3GameObj::OnRevive( pCtx);
}
