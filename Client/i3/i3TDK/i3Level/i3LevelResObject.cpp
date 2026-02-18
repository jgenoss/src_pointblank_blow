#include "stdafx.h"
#include "i3LevelResObject.h"
#include "i3LevelObject.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelResObject);

i3LevelResObject::i3LevelResObject(void)
{
	m_ResType		= I3LV_TYPE_OBJECT;
}

i3LevelResObject::~i3LevelResObject(void)
{
	I3_SAFE_RELEASE( m_pObject);

	I3_SAFE_RELEASE( m_pSgRes);
}

void i3LevelResObject::setObject( i3Object * pObj)
{
	I3_REF_CHANGE( m_pObject, pObj);
}

bool i3LevelResObject::OnValidateBrief( bool bForce)
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

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return true;
}

bool i3LevelResObject::OnValidate( i3LevelScene * pScene, bool bForce)
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
		{
			EndValidate();
			return false;
		}

		I3_SAFE_RELEASE( m_pObject);
	}
	EndValidate();

	m_pObject = (i3Object *) file.getKeyObject();
	I3_MUST_ADDREF(m_pObject);

	// Object에서 사용하는 모든 SgInfo들을 검색한다.
	if( m_pObject->getStateCount() > 0)
	{
		I3_OBJECT_STATE * pState = m_pObject->getState( 0);
		i3SceneGraphInfo * pSgInfo = pState->m_pSg;

		if( pSgInfo != nullptr)
		{
			m_pSgRes = (i3LevelResSceneGraph *) g_pResDB->FindResByPath( pSgInfo->GetName());

			if( m_pSgRes != nullptr)
			{
				I3_MUST_ADDREF(m_pSgRes);

				m_pSgRes->OnValidate( pScene, bForce);
			}
		}
	}

	m_pObject->SetExternRes( TRUE);
	m_pObject->setDynamicActiveState( false);
	m_pObject->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC);
	
	addResState( I3LV_RES_STATE_LOADED);

	return true;
}


i3Node * i3LevelResObject::GetElementAttachNode( i3LevelScene * pScene)
{
	if( m_pSgRes != nullptr)
	{
		return m_pSgRes->GetElementAttachNode( pScene);
	}

	return nullptr;
}

i3GameRes *			i3LevelResObject::CreateGameRes(void)
{
	i3GameResObject * pRes;

	pRes = i3GameResObject::new_object();

	pRes->SetGameObject( m_pObject);

	return pRes;
}

i3LevelElement3D *	i3LevelResObject::CreateInstance( i3LevelScene * pScene)
{
	i3LevelObject * pObj;

	OnValidate( pScene, false);

	pObj = i3LevelObject::new_object_ref();
	pObj->BindRes( this);

	pScene->AddElement( pObj, GetName());

	return pObj;
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)			// [test required] 패킹사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct RES_OBJECT
	{
		UINT8			m_ID[4] = { 'O', 'B', 'J', '1' };
		UINT32			pad[16] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3LevelResObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::RES_OBJECT data;

	Result = i3LevelRes::OnSave( pResFile);
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3LevelResObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::RES_OBJECT data;

	Result = i3LevelRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return Result;
}
