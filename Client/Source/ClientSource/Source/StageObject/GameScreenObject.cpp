#include "pch.h"
#include "GameScreenObject.h"
#include "GlobalVariables.h"

#if defined( USE_EDITDLG)
#include "i3Level/i3LevelElement3D.h"
#endif

I3_CLASS_INSTANCE( GameScreenObject);//, i3Object);

GameScreenObject::GameScreenObject(void)
{
	m_pCamObj				= nullptr;

	m_pCam					= nullptr;
	m_pTargetTextureAttr	= nullptr;
}

GameScreenObject::~GameScreenObject(void)
{
	I3_SAFE_RELEASE( m_pCamObj);
	I3_SAFE_RELEASE( m_pTargetTextureAttr);
}

void	GameScreenObject::SetCameraObj( i3CameraObject * pObj)
{
	I3_REF_CHANGE( m_pCamObj, pObj);

	I3_SAFE_RELEASE( m_pTargetTextureAttr);

	_BindCameraObj();
}

void GameScreenObject::_BindCameraObj()
{
	if( m_pCamObj != nullptr)
	{
		m_pCam = m_pCamObj->getCamera();
		if( m_pCam == nullptr)
		{
			m_pTargetTextureAttr  = nullptr;
			I3TRACE("게임스크린 오브젝트가 참조하고있는 카메라가 없습니다.\n");
		}

		if((m_pTargetTextureAttr == nullptr) && (getAttachNode() != nullptr))
		{
			INT32 i;
			i3Node * pChild;

			for( i = 0; i < getAttachNode()->GetChildCount(); i++)
			{
				pChild = getAttachNode()->GetChild( i);

				m_pTargetTextureAttr = (i3TextureBindAttr*)i3Scene::FindAttrByExactType( pChild, i3TextureBindAttr::static_meta(), true);

				if( m_pTargetTextureAttr != nullptr)
					break;
			}

			if( m_pTargetTextureAttr == nullptr)
			{
				i3AttrSet * pAttrSet = nullptr;

				if( getAttachNode()->GetChildCount() > 0)
				{
					pChild = getAttachNode()->GetChild( 0);

					pAttrSet = (i3AttrSet*)i3Scene::FindNodeByExactType( pChild, i3AttrSet::static_meta(), true);
				}

				if( pAttrSet != nullptr)
				{	
					m_pTargetTextureAttr = i3TextureBindAttr::new_object();

					pAttrSet->AddAttr( m_pTargetTextureAttr);
				}
				else
				{
					I3TRACE("게임스크린 오브젝트에 유효한 텍스쳐바인드가 존재하지 않습니다.\n");
					I3_SAFE_RELEASE( m_pTargetTextureAttr);
				}
			}
			else
			{
				I3_MUST_ADDREF(m_pTargetTextureAttr);
			}
		}
	}
}

void GameScreenObject::OnBindResource(void)
{
	i3Object::OnBindResource();

	_BindCameraObj();
}

void	GameScreenObject::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pCamObj != nullptr)
	{
		m_pCamObj->OnBuildObjectList( List);
	}

	i3Object::OnBuildObjectList( List);
}

bool s_bDump = false;

void GameScreenObject::OnUpdate( REAL32 rDeltaSeconds)
{
	i3Object::OnUpdate( rDeltaSeconds);

	if((m_pCamObj != nullptr) && (m_pCam != nullptr) && (m_pTargetTextureAttr != nullptr))
	{
		i3RenderTargetAttr * pRT = m_pCam->getRenderTargetAttr();
		if( pRT != nullptr)
		{
			i3Texture * pTex = pRT->GetColorRenderTarget();
			I3ASSERT( pTex != nullptr);

			m_pTargetTextureAttr->SetTexture( pTex);
		}
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

struct ALIGN4 PERSIST_SCREEN_OBJECT
{
	INT8		m_ID[4];
	OBJREF64	m_pCamObj;
	UINT32		pad[16];
} ;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32	GameScreenObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;

	Result = i3Object::OnSave( pResFile);
	I3_CHKIO( Result);

	PERSIST_SCREEN_OBJECT data;
	i3mem::FillZero( &data, sizeof( data));

	i3mem::Copy( data.m_ID, "PSO1", sizeof( UINT8) * 4);
	data.m_pCamObj = (OBJREF64)pResFile->GetObjectPersistID( m_pCamObj);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;
	
	return Result;
}

UINT32	GameScreenObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;

	Result = i3Object::OnLoad( pResFile);
	I3_CHKIO( Result);

	PERSIST_SCREEN_OBJECT data;

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_pCamObj > 0)
	{
		i3CameraObject * pCamObj = (i3CameraObject*)pResFile->FindObjectByID( (OBJREF)data.m_pCamObj);
		if( pCamObj != nullptr)
		{
			SetCameraObj( pCamObj);
		}
	}

	return Result;
}

UINT32	GameScreenObject::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{
	UINT32 sz;
	PERSIST_SCREEN_OBJECT * pData;

#if defined( USE_EDITDLG)
	if( pRefObj != nullptr)
	{
		i3LevelElement3D * pElm = (i3LevelElement3D *) pRefObj;
		INT32 i;

		for( i = 0; i < pElm->getTargetCount(); i++)
		{
			i3LevelElement3D * pDestElm = pElm->getTarget( i);

			i3GameObj * pObj = pDestElm->CreateGameInstance();

			I3_MUST_RELEASE(pObj);

			if(i3::kind_of<i3CameraObject*>(pObj))
			{
				SetCameraObj( (i3CameraObject*) pObj);

				i = pElm->getTargetCount(); 
				continue;
			}
		}
	}
#endif

	sz = sizeof(PERSIST_SCREEN_OBJECT);

	*ppData = pData = (PERSIST_SCREEN_OBJECT *) i3MemAlloc( sz);
	
	i3mem::FillZero( pData, sz);
	i3mem::Copy( pData->m_ID, "PSO1", 4);

	if( pInfo != nullptr)
	{
		pData->m_pCamObj = pInfo->GetObjectPersistID( getCameraObj());
		I3ASSERT( pData->m_pCamObj != -1);
	}
	else
	{
		pData->m_pCamObj = 0;
	}

	return sz;

}

UINT32	GameScreenObject::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	PERSIST_SCREEN_OBJECT data;
	i3Stream * pStream = pInfo->getStream();
	UINT32 rc, result = 0;

	rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_pCamObj > 0)
	{
		i3CameraObject * pObj = (i3CameraObject *) pInfo->FindObjectByIndex( (INT32)data.m_pCamObj);
		if( pObj != nullptr)
		{
			SetCameraObj( (i3CameraObject*) pObj);
		}
	}
	
	if( pRefObj != nullptr)
	{
#if defined(USE_EDITDLG)
		i3LevelElement3D * pElm = (i3LevelElement3D *) pRefObj;
		UINT32 i;

		for( i = 0; i < (UINT32) pElm->getTargetCount(); i++)
		{
			i3LevelElement3D * pDestElm = pElm->getTarget( i);

			i3GameObj * pObj = pDestElm->CreateGameInstance();

			I3_MUST_RELEASE(pObj);

			if( i3::kind_of<i3CameraObject*>(pObj))
			{
				SetCameraObj( (i3CameraObject*) pObj);

				i = pElm->getTargetCount(); 
				continue;
			}
		}
#endif
	}

	I3ASSERT( result == sz);

	return result;
}

#if defined(USE_EDITDLG)//XML
bool GameScreenObject::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3Object::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	// i3XMLFile은 내부적으로 i3PersistantElement *의 Stack을 가지고 있다.
	// 이 Stack은 Loading하는 Object를 관리하는 구조로,
	// 이 함수가 호출되는 시점에는 이 ScreenObject를 소유하는 i3LevelElement3D Class의
	// Pointer가 담겨인다.
	i3LevelElement3D * pLVObj = (i3LevelElement3D *) pFile->getCurrentObject();
	I3ASSERT( pLVObj != nullptr);

	i3LevelElement3D * pCamObj;

	pCamObj = pLVObj->FindTargetByGameObj( m_pCamObj);
	I3ASSERT( pCamObj != nullptr);

	pXML->addAttr( "CamObj",  pFile->GetObjectPersistID( pCamObj));

	return true;
}

bool GameScreenObject::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3Object::OnLoadXML( pFile, pXML);
	if( rv == false)
		return false;

	i3LevelElement3D * pCamObj;

	{
		OBJREF ref;

		if( pXML->getAttr( "CamObj", &ref) == nullptr)
		{
			I3ERRLOG( i3Error::_INVALIDFORMAT);

			return false;
		}

		if( ref != 0)
		{
			pCamObj = (i3LevelElement3D *) pFile->FindObjectByID( ref);

			I3ASSERT( pCamObj != nullptr);

			i3GameObj * pObj = pCamObj->CreateGameInstance();
		
			I3ASSERT( i3::same_of<i3CameraObject*>(pObj));

			SetCameraObj( (i3CameraObject *) pObj);

			I3_MUST_RELEASE(pObj);
		}
	}

	return true;
}
#endif
